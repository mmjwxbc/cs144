#include "tcp_receiver.hh"
#include <iostream>
// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader header = seg.header();
    if(!header.syn && !_syn) {
        return;
    }
    if(header.syn) {
        _syn = true;
        _isn = WrappingInt32(header.seqno);
    }
    if(header.fin) {
        _fin = true;
    }
    Buffer buffer = seg.payload();
    if(buffer.size()) {
        WrappingInt32 n = header.seqno;
        uint64_t index = unwrap(n, _isn, _reassembler._unassembled_index());
        if(!header.syn) {
            index --;
        }
        _reassembler.push_substring(buffer.copy(), index, header.fin);
        // cout << "syn = " << _syn << " index = " << index << " data = " << buffer.copy() << endl;
    } else if(header.fin) {
        _reassembler.stream_out().end_input();
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_syn) {
        return nullopt;
    }
    uint64_t no = _reassembler._unassembled_index() + is_syn();
    if(_reassembler.stream_out().input_ended()) {
        no += is_fin();
    }
    return wrap(no, _isn);
}

size_t TCPReceiver::window_size() const {
    return _reassembler.window_size();
}
