#/bin/bash
if [ -f "pcp" ]; then
	rm pcp -f
fi
gcc mutexThread.c -o pcp -lpthread -w
./pcp

