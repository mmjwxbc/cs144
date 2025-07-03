#include "byte_stream.hh"
#include <vector>
#include <iostream>
#include <algorithm>
// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// ByteStream::ByteStream(const size_t capacity) {this->capacity = capacity; this->buffer = vector<unsigned char>(capacity)}
ByteStream::ByteStream(const size_t cap) : capacity(cap), unused_capacity(cap), buffer(){}

size_t ByteStream::write(const string &data) {
    if (_end)
        return 0;
    size_t write_size = min(data.size(), unused_capacity);
    w_bytes += write_size;
    this->unused_capacity -= write_size;
    for(size_t i = 0; i < write_size; i++) {
        this->buffer.push_back(data[i]);
    }
    // cout << "unused_capacity = " << this->unused_capacity << endl;
    // for(int i = 0; i < this->buffer.size(); i++)
    //     cout << buffer[i];
    // cout << endl;
    // cout << buffer.size() << endl;
    // cout << this->unused_capacity << endl;
    // cout << this->capacity << endl;
    return write_size;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t pop_size = min(len, buffer.size());
    return string(buffer.begin(), buffer.begin() + pop_size);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    this->unused_capacity += len;
    this->r_bytes+=len;
    for(size_t i = 0; i < len; i++) {
        buffer.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string read_data;
    read_data = peek_output(len);
    pop_output(len);
    return read_data;
}

void ByteStream::end_input() {this->_end = true;}

bool ByteStream::input_ended() const { return this->_end; }

size_t ByteStream::buffer_size() const { return buffer.size(); }

bool ByteStream::buffer_empty() const { return buffer.empty(); }

bool ByteStream::eof() const { return this->_end && this->buffer.empty(); }

size_t ByteStream::bytes_written() const { return this->w_bytes; }

size_t ByteStream::bytes_read() const { return this->r_bytes; }

size_t ByteStream::remaining_capacity() const { return {this->unused_capacity}; }
