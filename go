#!/bin/bash
clang-3.8 -stdlib=libc++ -std=c++1z uvcpp.cpp -Ilibuv/include -Llibuv/out/Debug -lstdc++ -luv -lpthread && ./a.out
