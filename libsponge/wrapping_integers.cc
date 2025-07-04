#include "wrapping_integers.hh"
#include <vector>
#include <algorithm>
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    return isn +  static_cast<uint32_t>(n);
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
// uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
//     uint32_t val = n - isn;
//     uint64_t prev_shift = 0ull;
//     uint64_t base = checkpoint & ~((1ULL << 32) - 1);
//     uint64_t last_shift = (1ul << 32);
//     while(checkpoint >= last_shift) {
//         prev_shift = last_shift;
//         last_shift <<= 1;
//     }
//     checkpoint = last_shift - checkpoint >= checkpoint - prev_shift ? prev_shift : last_shift;
//     return checkpoint + static_cast<uint64_t>(val);
// }


uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    uint32_t offset = n - isn;
    uint64_t base = checkpoint & ~((1ULL << 32) - 1);
    uint64_t candidate = base + offset;
    uint64_t prev_candidate = base != 0 ? candidate - (1ULL << 32) : candidate;
    uint64_t last_candidate = base != 0xFFFFFFFF00000000 ? candidate + (1ULL << 32) : candidate;

    if(candidate > checkpoint) {
        return (candidate - checkpoint) < (checkpoint - prev_candidate) ? candidate : prev_candidate;
    } else {
        return (last_candidate - checkpoint) < (checkpoint - candidate) ? last_candidate : candidate;
    }
}
