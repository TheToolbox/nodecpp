#!/bin/bash
clang-3.8 -O3 -stdlib=libstdc++ -std=c++14 uvcpp.cpp -Ilibuv/include -Llibuv/out/Debug -luv -lpthread -lstdc++ && ./a.out
# Reference: http://stackoverflow.com/questions/37096062/get-a-basic-c-program-to-compile-using-clang-on-ubuntu-16