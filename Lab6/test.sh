#/bin/bash
rm phi -f
gcc mutexThread.c -o phi -lpthread -w
./phi
