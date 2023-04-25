#include <iostream>
#include "bytes.h"

int main(int, char**) {
    Bytes initvector(8);
    Bytes message(2189);
    initvector.print();
    initvector.addByte(76);
    initvector.print();
    std::cout << initvector.getLen() << std::endl;
    initvector.popFirstBytes(6).value().print();
    initvector.print();


    return 0;
}
