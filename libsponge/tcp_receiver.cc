#include "tcp_receiver.hh"
#include <iostream>
// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// void TCPReceiver::segment_received(const TCPSegment &seg) {
//     TCPHeader header = seg.header();
//     if(!header.syn && !_syn) {
//         return;
//     }
//     if(header.syn) {
//         _syn = true;
//         _isn = WrappingInt32(header.seqno);
//     }
//     if(header.fin) {
//         _fin = true;
//     }
//     Buffer buffer = seg.payload();
//     if(buffer.size()) {
//         WrappingInt32 n = header.seqno;
//         uint64_t index = unwrap(n, _isn, _reassembler._unassembled_index());
//         if(!header.syn) {
//             index --;
//         }
//         _reassembler.push_substring(buffer.copy(), index, header.fin);
//         // cout << "syn = " << _syn << " index = " << index << " data = " << buffer.copy() << endl;
//     }
// }

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();
    const Buffer &payload = seg.payload();

    // Ignore segments if we haven't received SYN yet and this segment isn't SYN
    if (!_syn && !header.syn) {
        return;
    }

    // Handle SYN
    if (header.syn) {
        _syn = true;
        _isn = header.seqno;
    }

    // Compute absolute stream index
    uint64_t abs_index = 0;
    if (header.syn) {
        abs_index = 0;
    } else {
        abs_index = unwrap(header.seqno, _isn, _reassembler._unassembled_index()) - 1;
    }

    // Push the payload (if any) and FIN (if any)
    _reassembler.push_substring(std::move(payload.copy()), abs_index, header.fin);
    
    // No need to call stream_out().end_input() manually — it's handled by eof flag in push_substring
}


optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_syn) {
        return nullopt;
    }
    uint64_t no = _reassembler._unassembled_index() + is_syn();
    // cout << "input_ended() == " << _reassembler.stream_out().input_ended() << endl;
    if(_reassembler.stream_out().input_ended()) {
        no += 1;
    }
    return wrap(no, _isn);
}

size_t TCPReceiver::window_size() const {
    return _reassembler.window_size();
}
