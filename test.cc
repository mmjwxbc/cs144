// #include <iostream>
// #include <memory>
// #include "libsponge/stream_reassembler.hh"
// #include "libsponge/byte_stream.hh"
// #include <algorithm>
// #include <cstdint>
// #include <cstdlib>
// #include <exception>
// #include <iostream>
// #include <stdexcept>
// #include <tuple>
// #include <utility>
// #include <vector>
// #include "libsponge/util/util.hh"
// #include "libsponge/wrapping_integers.hh"
// using namespace std;

// static constexpr unsigned NREPS = 32;
// static constexpr unsigned NSEGS = 128;
// static constexpr unsigned MAX_SEG_LEN = 2048;

// void print(StreamReassembler &streamreassembler) {
//     printf("********************************\n");
//     cout << "bytes_written = " << streamreassembler.stream_out().bytes_written() << endl;
//     cout << "unassembled_bytes = " << streamreassembler.unassembled_bytes() << endl;
//     cout << "assembled_bytes = " << streamreassembler.stream_out().bytes_written() << endl;
//     cout << "_output end_input = " << streamreassembler.stream_out().input_ended() << endl;
//     cout << "streamreassembler._unassembled_index() = " << streamreassembler._unassembled_index() << endl;
//     cout << "streamreassembler.stream_out().buffer_size() = " << streamreassembler.stream_out().buffer_size() << endl;
//     cout << "window size = " << streamreassembler.window_size() << endl;
//     printf("********************************\n");
// }

// string read(StreamReassembler &reassembler) {
//     return reassembler.stream_out().read(reassembler.stream_out().buffer_size());
// }



// int main()
// {
//     /*
 
//             test.execute(SubmitSegment{"b", 1});
//             test.execute(BytesAssembled(0));
//             test.execute(BytesAvailable(""));
//             test.execute(NotAtEof{});

//             test.execute(SubmitSegment{"d", 3});
//             test.execute(BytesAssembled(0));
//             test.execute(BytesAvailable(""));
//             test.execute(NotAtEof{});

//             test.execute(SubmitSegment{"abc", 0});

//             test.execute(BytesAssembled(4));
//             test.execute(BytesAvailable("abcd"));
//             test.execute(NotAtEof{});   
//     */
//     StreamReassembler streamreassembler(2);
//     // streamreassembler.push_substring("abcd", 0, 0);
//     // print(streamreassembler);
//     // streamreassembler.push_substring("ghx", 6, 1);
//     // print(streamreassembler);
//     // streamreassembler.push_substring("cdefg", 2, 0);
//     // print(streamreassembler);
//     streamreassembler.push_substring("ab", 0, 0);
//     print(streamreassembler);
//     streamreassembler.stream_out().read(2);
//     print(streamreassembler);
//     streamreassembler.push_substring("cd", 2, 0);
//     print(streamreassembler);

//     // auto rd = get_random_generator();

//     //     // buffer a bunch of bytes, make sure we can empty and re-fill before calling close()
//     // for (unsigned rep_no = 0; rep_no < NREPS; ++rep_no) {
//     //     StreamReassembler buf{MAX_SEG_LEN * NSEGS};

//     //     vector<tuple<size_t, size_t>> seq_size;
//     //     size_t offset = 0;
//     //     for (unsigned i = 0; i < NSEGS; ++i) {
//     //         const size_t size = 1 + (rd() % (MAX_SEG_LEN - 1));
//     //         seq_size.emplace_back(offset, size);
//     //         offset += size;
//     //     }
//     //     shuffle(seq_size.begin(), seq_size.end(), rd);

//     //     string d(offset, 0);
//     //     generate(d.begin(), d.end(), [&] { return rd(); });

//     //     for (auto [off, sz] : seq_size) {
//     //         string dd(d.cbegin() + off, d.cbegin() + off + sz);
//     //         buf.push_substring(move(dd), off, off + sz == offset);
//     //     }

//     //     auto result = read(buf);
//     //     if (buf.stream_out().bytes_written() != offset) {  // read bytes
//     //         throw runtime_error("test 1 - number of bytes RX is incorrect");
//     //     }
//     //     if (!equal(result.cbegin(), result.cend(), d.cbegin())) {
//     //         throw runtime_error("test 1 - content of RX bytes is incorrect");
//     //     }
//     // }

//     // cout << streamreassembler.unassembled_strs.size() << endl;

//     // streamreassembler.push_substring("cd", 2, 0);
//     // streamreassembler.push_substring("ab", 0, 0);
//     // cout << "bytes_written = " << streamreassembler.stream_out().bytes_written() << endl;
//     // cout << "unassembled_bytes = " << streamreassembler.unassembled_bytes() << endl;
//     // cout << "unassembled_bytes = " << streamreassembler.stream_out().bytes_written() << endl;
//     // cout << streamreassembler.stream_out().input_ended() << endl;

// /*
//     test.execute(SubmitSegment{"abc", 0});
//     test.execute(BytesAssembled(3));

//     test.execute(SubmitSegment{"bcdefgh", 1}.with_eof(true));

//     test.execute(BytesAssembled(8));
//     test.execute(BytesAvailable{"abcdefgh"});
//     test.execute(AtEof{});
// */
//     // ByteStream bytesteam(2);
//     // bytesteam.write("cat");
//     // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
//     // bytesteam.pop_output(2);
//     // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
//     // bytesteam.write("tac");
//     // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
//     // cout << "Pop output = " << bytesteam.peek_output(2) << endl;

//     // unwrap(WrappingInt32(1), WrappingInt32(0), 0);
//     // uint64_t ret = unwrap(WrappingInt32(1), WrappingInt32(0), UINT32_MAX);
//     // cout <<  (ret == ((1ul << 32) + 1)) << endl;
// }

// // #include "libsponge/util/util.hh"
// // #include "libsponge/wrapping_integers.hh"

// // #include <cstdint>
// // #include <iostream>
// // #include <sstream>
// // #include <stdexcept>

// // using namespace std;

// // void check_roundtrip(const WrappingInt32 isn, const uint64_t value, const uint64_t checkpoint) {
// //     if (unwrap(wrap(value, isn), isn, checkpoint) != value) {
// //         ostringstream ss;

// //         ss << "Expected unwrap(wrap()) to recover same value, and it didn't!\n";
// //         ss << "  unwrap(wrap(value, isn), isn, checkpoint) did not equal value\n";
// //         ss << "  where value = " << value << ", isn = " << isn << ", and checkpoint = " << checkpoint << "\n";
// //         ss << "  (Difference between value and checkpoint is " << value - checkpoint << ".)\n";
// //         throw runtime_error(ss.str());
// //     }
// // }

// // int main() {
// //     try {
// //         auto rd = get_random_generator();
// //         uniform_int_distribution<uint32_t> dist31minus1{0, (uint32_t{1} << 31) - 1};
// //         uniform_int_distribution<uint32_t> dist32{0, numeric_limits<uint32_t>::max()};
// //         uniform_int_distribution<uint64_t> dist63{0, uint64_t{1} << 63};

// //         const uint64_t big_offset = (uint64_t{1} << 31) - 1;

// //         for (unsigned int i = 0; i < 1000000; i++) {
// //             const WrappingInt32 isn{dist32(rd)};
// //             const uint64_t val{dist63(rd)};
// //             const uint64_t offset{dist31minus1(rd)};

// //             check_roundtrip(isn, val, val);
// //             check_roundtrip(isn, val + 1, val);
// //             check_roundtrip(isn, val - 1, val);
// //             check_roundtrip(isn, val + offset, val);
// //             check_roundtrip(isn, val - offset, val);
// //             check_roundtrip(isn, val + big_offset, val);
// //             check_roundtrip(isn, val - big_offset, val);
// //         }
// //     } catch (const exception &e) {
// //         cerr << e.what() << endl;
// //         return 1;
// //     }

// //     return EXIT_SUCCESS;
// // }


#include "./tests/sender_harness.hh"
#include "./libsponge/wrapping_integers.hh"


#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

using namespace std;

int main() {
    try {
        auto rd = get_random_generator();

        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN sent test", cfg};
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(ExpectBytesInFlight{1});
        }

        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN acked test", cfg};
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(ExpectBytesInFlight{1});
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(ExpectNoSegment{});
            test.execute(ExpectBytesInFlight{0});
        }

        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN -> wrong ack test", cfg};
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(ExpectBytesInFlight{1});
            test.execute(AckReceived{WrappingInt32{isn}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectNoSegment{});
            test.execute(ExpectBytesInFlight{1});
        }

        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN acked, data", cfg};
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(ExpectBytesInFlight{1});
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(ExpectNoSegment{});
            test.execute(ExpectBytesInFlight{0});
            test.execute(WriteBytes{"abcdefgh"});
            test.execute(Tick{1});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(ExpectSegment{}.with_seqno(isn + 1).with_data("abcdefgh"));
            test.execute(ExpectBytesInFlight{8});
            test.execute(AckReceived{WrappingInt32{isn + 9}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(ExpectNoSegment{});
            test.execute(ExpectBytesInFlight{0});
            test.execute(ExpectSeqno{WrappingInt32{isn + 9}});
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return EXIT_SUCCESS;
}
