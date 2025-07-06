#include "tcp_connection.hh"

#include <iostream>

// Dummy implementation of a TCP connection

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_received; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    TCPState tcpstate{_sender, _receiver, _active, _linger_after_streams_finish};
    TCPHeader tcpheader = seg.header();
    _time_since_last_segment_received = 0;
    if(tcpstate == TCPState::State::LISTEN) {
        _receiver.segment_received(seg);
    } else if(tcpstate == TCPState::State::SYN_SENT) {
        _sender.ack_received(tcpheader.ackno, tcpheader.win);
        _sender.fill_window();
    } else if(tcpstate == TCPState::State::SYN_RCVD) {
        if(tcpheader.ack) {
            _receiver.segment_received(seg);
            _sender.ack_received(tcpheader.ackno, tcpheader.win);
        }
    } else if(tcpstate == TCPState::State::ESTABLISHED) {
        _receiver.segment_received(seg);
        _sender.ack_received(tcpheader.ackno, tcpheader.win);
    }
    if(tcpheader.rst) {
        _active = false;
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        return;
    }
    collect_output();
}

bool TCPConnection::active() const { return _active; }

size_t TCPConnection::write(const string &data) {
    size_t written_bytes = _sender.stream_in().write(data);
    if(_active && _sender.next_seqno_absolute() > bytes_in_flight() && !_sender.stream_in().input_ended()) {
        _sender.fill_window();
        collect_output();
    }
    return written_bytes;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) { 
    _time_since_last_segment_received += ms_since_last_tick;
    _sender.tick(ms_since_last_tick); 
    collect_output();
}

void TCPConnection::end_input_stream() {_sender.stream_in().end_input();}

void TCPConnection::connect() {
    if(_sender.next_seqno_absolute() == 0) {
        _sender.fill_window();
        collect_output();
        while(_sender.next_seqno_absolute() > _sender.bytes_in_flight() && !_sender.stream_in().eof()) {};
        _active = true;
    }
}

void TCPConnection::collect_output() {
    auto &sender_queue = _sender.segments_out(); 
    while(! sender_queue.empty()) {
        TCPSegment &seg = sender_queue.front();
        TCPHeader &header = seg.header();
        if(_receiver.ackno().has_value()) {
            header.ack = true;
            header.ackno = _receiver.ackno().value();
        }
        header.win = _receiver.window_size();
        _segments_out.push(sender_queue.front());
        sender_queue.pop();
    }
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
            _active = false;
            // Your code here: need to send a RST segment to the peer
            _sender.stream_in().set_error();
            _receiver.stream_out().set_error();
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
