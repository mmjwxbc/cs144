#include <iostream>
#include <memory>
#include "libsponge/stream_reassembler.hh"
#include "libsponge/byte_stream.hh"
using namespace std;
int main()
{
    // StreamReassembler streamreassembler(8);
    // streamreassembler.push_substring("b", 1, 0);
    // cout << "bytes_written = " << streamreassembler.stream_out().bytes_written() << endl;
    // cout << "unassembled_bytes = " << streamreassembler.unassembled_bytes() << endl;
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
    ByteStream bytesteam(2);
    bytesteam.write("cat");
    cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    bytesteam.pop_output(2);
    cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    bytesteam.write("tac");
    cout << "buffer empty = " << bytesteam.buffer_empty() << endl;
    cout << "Pop output = " << bytesteam.peek_output(2) << endl;
}