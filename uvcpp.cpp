//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <memory>
#include "uvcpp.h"


int main() {
    auto loopy = uv::EventLoop();
    auto server = uv::TCPServer(loopy);
    server.listen("0.0.0.0", 8000);

    loopy.run();


    printf("Now quitting.\n");

    return 0;
}