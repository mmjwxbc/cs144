#include <iostream>
#include <memory>
#include "./libsponge/byte_stream.hh"

int main()
{
    ByteStream bytestram(15);
    bytestram.write("cat");
    std::cout << bytestram.read(3) << std::endl;
}