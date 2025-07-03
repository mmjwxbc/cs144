#include <iostream>
#include <memory>
#include "libsponge/stream_reassembler.hh"
#include "libsponge/byte_stream.hh"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>
#include "libsponge/util/util.hh"
using namespace std;

static constexpr unsigned NREPS = 32;
static constexpr unsigned NSEGS = 128;
static constexpr unsigned MAX_SEG_LEN = 2048;

void print(StreamReassembler &streamreassembler) {
    printf("********************************\n");
    cout << "bytes_written = " << streamreassembler.stream_out().bytes_written() << endl;
    cout << "unassembled_bytes = " << streamreassembler.unassembled_bytes() << endl;
    cout << "assembled_bytes = " << streamreassembler.stream_out().bytes_written() << endl;
    cout << "_output end_input = " << streamreassembler.stream_out().input_ended() << endl;
    printf("********************************\n");
}

string read(StreamReassembler &reassembler) {
    return reassembler.stream_out().read(reassembler.stream_out().buffer_size());
}



int main()
{
    /*
 
            test.execute(SubmitSegment{"b", 1});
            test.execute(BytesAssembled(0));
            test.execute(BytesAvailable(""));
            test.execute(NotAtEof{});

            test.execute(SubmitSegment{"d", 3});
            test.execute(BytesAssembled(0));
            test.execute(BytesAvailable(""));
            test.execute(NotAtEof{});

            test.execute(SubmitSegment{"abc", 0});

            test.execute(BytesAssembled(4));
            test.execute(BytesAvailable("abcd"));
            test.execute(NotAtEof{});   
    */
    StreamReassembler streamreassembler(2);
    streamreassembler.push_substring("ab", 0, 0);
    print(streamreassembler);
    streamreassembler.push_substring("cd", 2, 0);
    print(streamreassembler);
    streamreassembler.stream_out().read(2);
    print(streamreassembler);
    streamreassembler.push_substring("cd", 2, 0);
    print(streamreassembler);
    // streamreassembler.push_substring("c", 2, 0);
    // print(streamreassembler);
    // streamreassembler.push_substring("a", 0, 0);
    // print(streamreassembler);

    // auto rd = get_random_generator();

    //     // buffer a bunch of bytes, make sure we can empty and re-fill before calling close()
    // for (unsigned rep_no = 0; rep_no < NREPS; ++rep_no) {
    //     StreamReassembler buf{MAX_SEG_LEN * NSEGS};

    //     vector<tuple<size_t, size_t>> seq_size;
    //     size_t offset = 0;
    //     for (unsigned i = 0; i < NSEGS; ++i) {
    //         const size_t size = 1 + (rd() % (MAX_SEG_LEN - 1));
    //         seq_size.emplace_back(offset, size);
    //         offset += size;
    //     }
    //     shuffle(seq_size.begin(), seq_size.end(), rd);

    //     string d(offset, 0);
    //     generate(d.begin(), d.end(), [&] { return rd(); });

    //     for (auto [off, sz] : seq_size) {
    //         string dd(d.cbegin() + off, d.cbegin() + off + sz);
    //         buf.push_substring(move(dd), off, off + sz == offset);
    //     }

    //     auto result = read(buf);
    //     if (buf.stream_out().bytes_written() != offset) {  // read bytes
    //         throw runtime_error("test 1 - number of bytes RX is incorrect");
    //     }
    //     if (!equal(result.cbegin(), result.cend(), d.cbegin())) {
    //         throw runtime_error("test 1 - content of RX bytes is incorrect");
    //     }
    // }

    // cout << streamreassembler.unassembled_strs.size() << endl;

    // streamreassembler.push_substring("cd", 2, 0);
    // streamreassembler.push_substring("ab", 0, 0);
    // cout << "bytes_written = " << streamreassembler.stream_out().bytes_written() << endl;
    // cout << "unassembled_bytes = " << streamreassembler.unassembled_bytes() << endl;
    // cout << "unassembled_bytes = " << streamreassembler.stream_out().bytes_written() << endl;
    // cout << streamreassembler.stream_out().input_ended() << endl;

/*
    test.execute(SubmitSegment{"abc", 0});
    test.execute(BytesAssembled(3));

    test.execute(SubmitSegment{"bcdefgh", 1}.with_eof(true));

    test.execute(BytesAssembled(8));
    test.execute(BytesAvailable{"abcdefgh"});
    test.execute(AtEof{});
*/
    // ByteStream bytesteam(2);
    // bytesteam.write("cat");
    // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    // bytesteam.pop_output(2);
    // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    // bytesteam.write("tac");
    // cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    // cout << "Pop output = " << bytesteam.peek_output(2) << endl;
}