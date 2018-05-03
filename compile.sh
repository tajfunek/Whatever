#!/bin/sh

gcc $1.c -I/usr/local/include/python3.6m -I/usr/local/include/python3.6m  -Wno-unused-result -Wsign-compare  -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -L/usr/local/lib/python3.6/config-3.6m-arm-linux-gnueabihf -L/usr/local/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -o $1.out

