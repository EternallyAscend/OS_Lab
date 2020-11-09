#/bin/bash
gcc copy.c -o copy -lpthread -w
./copy ./temp/ ./temp.bak
tree ./temp
tree ./temp.bak
