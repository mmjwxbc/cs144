#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}
#include <iostream>
#include <algorithm>
using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), buffer(capacity + 1), is_order(capacity + 1, false), _first_unacceptable(capacity), hh(0), tt(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if(index >= this->_first_unacceptable) {
        return;
    }
    if(eof == true) {
        this->_eof = true;
        this->_eof_bytes = index + data.size();
        // cout << "eof:" << this->_eof_bytes << endl;
    }
    size_t i = 0;
    if(index < this->_first_unassembled) {
        i = this->_first_unassembled - index;
    }
    // cout << "before hh = " << hh << endl;
    // cout << "before tt  = " << tt << endl;
    // cout << "_output buffer size = " << this->_output.remaining_capacity() << endl;

    for(; i < data.size(); i++) {
        size_t idx = (hh + index - this->_first_unread + i) % (this->_capacity + 1);
        // cout << "idx = " << idx << endl;
        this->buffer[idx] = data[i];
        if(!this->is_order[idx]) this->_unassembled_bytes ++;
        this->is_order[idx] = true;
    }
    // cout << "after hh = " << hh << endl;
    // cout << "after tt  = " << tt << endl;
    // cout << "_unassembled_bytes = " << _unassembled_bytes << endl;
    // for(size_t i = 0; i < this->_capacity + 1; i++)
    //     cout << "buffer[" << i  << "] = " <<  this->buffer[i] << " is_order[" << i << "] = " << this->is_order[i] << endl;
    // cout << "before _output buffer can write size = " << this->_output.remaining_capacity() << endl;
    // cout << "before _output buffer can read size = " << this->_output.buffer_size() << endl;
    // cout << "before _output buffer empty = " << this->_output.buffer_empty()<< endl;
    string send_data = "";
    while(this->_output.remaining_capacity() && this->is_order[this->hh]) {
        cout << "before _output buffer can write size = " << this->_output.remaining_capacity() << endl;
        send_data += this->buffer[this->hh];
        this->is_order[this->hh] = false;
        // cout << "write--> " << s << " before hh = " << this->hh;
        this->hh = (this->hh + 1) % (this->_capacity + 1);
        // cout << " after hh = " << this->hh << endl;
        this->tt = (this->tt + 1) % (this->_capacity + 1);
        this->_first_unassembled ++;
        this->_unassembled_bytes --;
        this->_first_unread ++;
        this->w_bytes ++;
        this->_first_unacceptable ++;
    }
    this->_output.write(send_data);
    // cout << "after _output buffer can write size = " << this->_output.remaining_capacity() << endl;
    // cout << "after _output buffer can read size = " << this->_output.buffer_size() << endl;
    // cout << "after _output buffer empty = " << this->_output.buffer_empty()<< endl;
    if(this->w_bytes == this->_eof_bytes && this->_eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return this->_unassembled_bytes; }

bool StreamReassembler::empty() const { return this->_unassembled_bytes == 0; }
