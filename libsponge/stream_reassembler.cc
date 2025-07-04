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

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _first_unacceptable(capacity), unassembled_strs() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if(index + data.size() < this->_first_unassembled) {
        return;
    }
    if(eof == true) {
        this->_eof = true;
        this->_eof_bytes = index + data.size();
        // cout << "eof:" << this->_eof_bytes << endl;
    }
    
    size_t start_idx = index;
    auto it = unassembled_strs.upper_bound(index);
    /*
        if (pos_iter != _unassemble_strs.end() && pos_iter->first <= index) {
        const size_t up_idx = pos_iter->first;

        // 如果当前子串前面出现了重叠
        if (index < up_idx + pos_iter->second.size())
            new_idx = up_idx + pos_iter->second.size();
    }
    */
    if(it != unassembled_strs.begin()) {
        it--;
    } 
    if(index <= _first_unassembled) {
        start_idx = _first_unassembled;
    } else if(it != unassembled_strs.end() && it->first <= index) {
        const size_t left_idx = it->first;
        if(index < left_idx + it->second.size()) {
            start_idx = left_idx + it->second.size();
        }
    }
    const size_t data_start_pos = start_idx - index;
    ssize_t data_size = data.size() - (start_idx - index);
    it = unassembled_strs.lower_bound(start_idx);
    for(;it != unassembled_strs.end();) {
        const size_t data_end_idx = start_idx + data_size;
        if(data_end_idx > it->first) {
            if(data_end_idx > it->first + it->second.size()) {
                _unassembled_bytes -= it->second.size();
                it = unassembled_strs.erase(it);
                continue;
            } else {
                data_size = it->first - start_idx;
                break;
            }
        } else {
            break;
        }
    }
    size_t first_unacceptable_idx = _first_unassembled + _capacity - _output.buffer_size();
    if (first_unacceptable_idx <= start_idx)
        return;
    if (data_size > 0) {
        const string new_data = data.substr(data_start_pos, data_size);
        _unassembled_bytes += new_data.size();
        unassembled_strs.insert(make_pair(start_idx, std::move(new_data)));
    }
    while (true) {
        it = unassembled_strs.find(_first_unassembled);
        _first_unacceptable = _first_unassembled + _capacity - _output.buffer_size();
        if (it == unassembled_strs.end() || it->first >= _first_unacceptable) {
            break;
        }
        size_t written_bytes = _output.write(it->second);

        _first_unassembled += written_bytes;
        _unassembled_bytes -= written_bytes;
        _first_unacceptable = _first_unassembled + _capacity - _output.buffer_size();
        _w_bytes += written_bytes;
        if(written_bytes < it->second.size()) {
            unassembled_strs.insert(make_pair(_first_unassembled, std::move(it->second.substr(written_bytes))));
        }
        unassembled_strs.erase(it);

    }
    // cout << "_w_bytes = " << _w_bytes << endl;
    if(_w_bytes == _eof_bytes && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return this->_unassembled_bytes; }

bool StreamReassembler::empty() const { return this->_unassembled_bytes == 0; }
