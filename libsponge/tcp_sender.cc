#include "tcp_sender.hh"

#include "tcp_config.hh"
#include "tcp_state.hh"
#include <algorithm>
#include <random>
#include <iostream>
// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _cur_retransmission_timeout(retx_timeout)
    , _stream(capacity)
    , _unack_index()
    , _unack_segments() {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    while (_recv_window_size != 0) {
        TCPSegment tcpsegment;
        TCPHeader &tcpheader = tcpsegment.header();
        size_t send_size = 0;
        if(_next_seqno == 0) {
            send_size = 1;
            tcpheader.syn = true;
            tcpheader.seqno = wrap(_next_seqno, _isn);
        } else if(_stream.buffer_size() && _next_seqno > _bytes_in_flight && !_stream.input_ended()) {
            tcpheader.seqno = wrap(_next_seqno, _isn);
            if (_stream.buffer_size()) {
                size_t max_payload = TCPConfig::MAX_PAYLOAD_SIZE;
                size_t available_window = _recv_window_size;

                if (available_window == 0) {
                    available_window = 1;
                }

                send_size = min({_stream.buffer_size(), available_window, max_payload});
                if (_stream.input_ended() && send_size == _stream.buffer_size() && available_window > send_size) {
                    tcpheader.fin = 1;
                }
                if (tcpheader.syn || tcpheader.fin) {
                    send_size = min(send_size, available_window - 1);
                }
                string data = _stream.read(send_size);
                tcpsegment.payload() = Buffer(move(data));
            } 
            _recv_window_size -= send_size;
            _fin_sent |= tcpheader.fin;
            _segments_out.push(tcpsegment);
            if(_unack_index.empty()) {
                _cur_ms = 0;
            }
            _unack_index.push(_next_seqno);
            _unack_segments.insert(make_pair(_next_seqno, tcpsegment));

            _next_seqno += tcpsegment.length_in_sequence_space();
            _bytes_in_flight += tcpsegment.length_in_sequence_space();
            break;
        } else if (_stream.input_ended() && _recv_window_size && !_fin_sent) {
            tcpheader.fin = true;
            send_size = 1;
            _fin_sent = true;
        } else {
            break;
        }
        if(send_size != 0) {
            _recv_window_size -= send_size;
            _fin_sent |= tcpheader.fin;
            _segments_out.push(tcpsegment);
            if(_unack_index.empty()) {
                _cur_ms = 0;
            }
            _unack_index.push(_next_seqno);
            _unack_segments.insert(make_pair(_next_seqno, tcpsegment));

            _next_seqno += tcpsegment.length_in_sequence_space();
            _bytes_in_flight += tcpsegment.length_in_sequence_space();
        }
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t abs_seqno = unwrap(ackno, _isn, _next_seqno);
    if(abs_seqno > _next_seqno) {
        return;
    }
    if(window_size == 0) {
        _recv_window_size = 1u;
        _zero_win = true;
    } else {
        _zero_win = false;
        _recv_window_size = static_cast<uint16_t>(window_size - static_cast<uint16_t>(_next_seqno - abs_seqno));
    }
    _retransmission_times = 0;
    // while (_unack_index.size()) {
    //     auto index = _unack_index.top();
    //     auto it = _unack_segments.find(index);
    //     if (it != _unack_segments.end()) {
    //         if (index + it->second.length_in_sequence_space() <= abs_seqno) {
    //             if(index + it->second.length_in_sequence_space() <= abs_seqno) {
    //                 _bytes_in_flight -= it->second.length_in_sequence_space();
    //                 _unack_segments.erase(it);
    //                 _unack_index.pop();
    //             } else {
    //                 _bytes_in_flight -= (abs_seqno - index);
    //                 break;
    //             }
    //         } else {
    //             break;
    //         }
    //     }
    // }
    while(!_unack_index.empty()) {
        uint64_t index = _unack_index.front();
        auto it = _unack_segments.find(index);
        if (it != _unack_segments.end()) {
            if (index + it->second.length_in_sequence_space() <= abs_seqno) {
                _cur_ms = 0;
                if(index + it->second.length_in_sequence_space() <= abs_seqno) {
                    _bytes_in_flight -= it->second.length_in_sequence_space();
                    _unack_segments.erase(it);
                    _unack_index.pop();
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    _cur_ms += ms_since_last_tick;
    // unsigned int cur_retransmission_times = _retransmission_times + 1;
    unsigned int cur_tto = _initial_retransmission_timeout << _retransmission_times;
    if(_cur_ms >= cur_tto) {
        if(!_unack_index.empty()) {
            _cur_ms = 0;
            if(!_zero_win) {
                _retransmission_times ++;
            }
            _segments_out.push(_unack_segments.at(_unack_index.front()));
        }
    }
    // vector<pair<uint64_t, uint64_t>> expired_indices;
    // while (_unack_time_index.size()) {
    //     auto p = _unack_time_index.top();
    //     auto expired_time = p.first, index = p.second;
    //     if(_unack_segments.find(index) == _unack_segments.end()) {
    //         _unack_time_index.pop();
    //         continue;
    //     }
    //     if (expired_time <= _cur_ms && !_unack_index.empty()) {
    //         _unack_time_index.pop();
    //         expired_indices.push_back(make_pair(_cur_ms + cur_tto, _unack_index.top()));
    //     } else {
    //         break;
    //     }
    // }
    // if(!_unack_time_index.empty()) {
    //     // expired_indices.push_back(_unack_index.top());
    //     auto p = _unack_time_index.top();
    //     if(p.first <= _cur_ms) {
    //         _segments_out.push(_unack_segments.at(_unack_index.top()));
    //         _unack_time_index.pop();
    //         _unack_time_index.push(make_pair(_cur_ms + cur_tto, p.second));
    //         _retransmission_times = cur_retransmission_times;
    //     }
    // }
    // for (auto p : expired_indices) {
    //     _retransmission_times = cur_retransmission_times;
    //     _segments_out.push(_unack_segments.at(p.second));
    //     _unack_time_index.push(p);
    // }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _retransmission_times; }

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    TCPHeader header = seg.header();
    header.ack = 1;
    header.ackno = wrap(_next_seqno, _isn);
    header.seqno = wrap(_next_seqno, _isn);
    _segments_out.push(seg);
}
