#define BUF_SIZE 64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("Argc: %d, argv[]: %s", argc, argv[0]);
	int infd, outfd;
	char buffer[BUF_SIZE];
	int i;
	
	if ((infd = open(argv[1], O_RDONLY)) < 0) {
		exit(2);	
	}

	if ((outfd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) < 0){
		exit(3);	
	}

	while(1) {
        i = read(infd, buffer, BUF_SIZE);
		if (i <= 0) {
            break;
        }
		write(outfd, buffer, i);
	}

	close(outfd);
	close(infd);

	exit(0);
}