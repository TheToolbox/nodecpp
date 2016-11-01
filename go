#!/bin/bash
clang-3.9 helloworld.cpp -Ilibuv/include -Llibuv/out/Debug -luv -lpthread && ./a.out