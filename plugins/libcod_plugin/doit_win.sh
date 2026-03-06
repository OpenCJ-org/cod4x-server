#!/bin/bash
NAME='libcod4_1_8x'
no_warning_spam="-Wall -Wno-pragmas -Wno-pragma -Wno-write-strings -Wno-pointer-arith -Wno-format -Wno-parentheses -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-return-type -Wno-sign-compare -Wno-unused-local-typedefs"

BUILD=${1:-debug} # default is debug

if [ "$BUILD" = "release" ]; then
    CFLAGS="$no_warning_spam -s -m32 -O1 -mtune=core2"
else # debug
    CFLAGS="$no_warning_spam -g -m32 -O0 -mtune=core2"
fi

#Compiling
g++ $CFLAGS -c *.cpp

#Linking
g++ -m32 -shared -static-libgcc -static-libstdc++ -o ${NAME}.dll *.o -L.. -lcom_plugin

#Cleaning up
rm -f *.o
