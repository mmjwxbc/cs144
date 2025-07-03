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
    if(index >= this->_first_unacceptable) {
        unassembled_strs[index] = data;
        return;
    }
    if(index + data.size() < this->_first_unassembled) {
        return;
    }
    if(eof == true) {
        this->_eof = true;
        this->_eof_bytes = index + data.size();
        // cout << "eof:" << this->_eof_bytes << endl;
    }
    
    if(index <= _first_unassembled) {
        auto it = unassembled_strs.lower_bound(index);
        size_t start = this->_first_unassembled - index;
        size_t end = data.size() - 1;
        if(it != unassembled_strs.end()) {
            if(index + data.size() > it->first + it->second.size()) {
                _unassembled_bytes -= it->second.size();
                unassembled_strs.erase(it);
            } else {
                end = it->first - index - 1;
            }
        }
        size_t length = end - start + 1;
        unassembled_strs[index + start] = data.substr(start, length);
        _unassembled_bytes += length;
    } else {
        auto right_it = unassembled_strs.upper_bound(index);
        auto left_it = unassembled_strs.lower_bound(index);
        size_t start = 0, end = data.size() - 1;
        // if(right_it != unassembled_strs.end() && right_it != unassembled_strs.begin()) {
        //     auto left_it = right_it;
        //     left_it--;
        //     auto left_it_end_idx = left_it->first + left_it->second.size() - 1;
        //     if(left_it_end_idx > index) {
        //         start = left_it_end_idx + 1 - index;
        //     }
        // }

        if(left_it != unassembled_strs.end() && left_it->first == index) {
            auto left_it_end_idx = left_it->first + left_it->second.size() - 1;
            start = left_it_end_idx - index + 1;
            if(index + data.size() - 1 <= left_it_end_idx) {
                end = 0;
            }
        } else if(right_it != unassembled_strs.begin()) {
            left_it = right_it;
            left_it--;
            auto left_it_end_idx = left_it->first + left_it->second.size() - 1;
            if(left_it_end_idx > index) {
                start = left_it_end_idx - index + 1;
                if(index + data.size() - 1 <= left_it_end_idx) {
                    end = 0;
                }
            }
        }
        if(right_it != unassembled_strs.end()) {
            auto right_it_start_idx = right_it->first;
            if(right_it_start_idx < index + data.size()) {
                if(right_it_start_idx + right_it->second.size() <= index + data.size()) {
                    _unassembled_bytes -= right_it->second.size();
                    unassembled_strs.erase(right_it);
                } else {
                    end = right_it_start_idx - index - 1;
                }
            }
        }
        
        if(end >= start) {
            size_t length = end - start + 1;
            unassembled_strs[index + start] = data.substr(start, length);
            _unassembled_bytes += length;
        }
        // cout << "unassembled_strs[" << index + start << "] = " << unassembled_strs[index + start] << endl;
    }
    // cout << "unassembled_strs size = " << unassembled_strs.size() << endl;
    while (true) {
        auto it = unassembled_strs.find(_first_unassembled);
        if (it == unassembled_strs.end() || it->first >= _first_unacceptable) {
            break;
        }
        size_t end_idx = it->first + it->second.size() - 1;
        if(end_idx < _first_unacceptable) {
            _output.write(it->second);
            _first_unassembled += it->second.size();
            _unassembled_bytes -= it->second.size();
            _first_unacceptable += it->second.size();
            _w_bytes += it->second.size();
            unassembled_strs.erase(it);
        } else {
            size_t exceed_len = _first_unacceptable - end_idx + 1;
            _output.write(it->second.substr(0, it->second.size() - exceed_len));

            unassembled_strs.erase(it);
            unassembled_strs[_first_unacceptable] = it->second.substr(it->second.size() - exceed_len, exceed_len);
            _first_unassembled += it->second.size() - exceed_len;
            _unassembled_bytes -= it->second.size() - exceed_len;
            _first_unacceptable += it->second.size() - exceed_len;
            _w_bytes += it->second.size() - exceed_len;
        }

    }
    // cout << "_w_bytes = " << _w_bytes << endl;
    if(_w_bytes == _eof_bytes && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return this->_unassembled_bytes; }

bool StreamReassembler::empty() const { return this->_unassembled_bytes == 0; }
