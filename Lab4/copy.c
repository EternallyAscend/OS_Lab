#define BUF_SIZE 64
#define NAME_LENGTH 256
#define NIL 0

#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct PathPair {
    char** origin;
    char** target;
};

struct PathPair* initPathPair(char** origin, char** target) {
    struct PathPair* pointer = (struct PathPair*)malloc(sizeof(struct PathPair));
    pointer->origin = origin;
    pointer->target = target;
    return pointer;
}

void makeDir(char* path) {
	char* str;
	str = (char*)malloc(sizeof(char) * NAME_LENGTH + 6);
	strcpy(str, "mkdir ");
	strcat(str, (const char*)path);
	system(str);
    free(str);
}

void copyFiles(char* inputFile, char* outputFile) {
    // printf("Copy files: %s %s\n", inputFile, outputFile);
		int inputStream;
		int outputStream;
		int item;
		char buffer[BUF_SIZE];
		if ((inputStream = open(inputFile, O_RDONLY)) < 0) {
            printf("Open file %s failed.\n", inputFile);
		}

        struct stat bufferStatus;
        stat(inputFile, &bufferStatus);
		while ((outputStream = open(outputFile, O_WRONLY | O_CREAT | O_EXCL, bufferStatus.st_mode)) < 0) {
			remove(outputFile);
		}

		while(1) {
        item = read(inputStream, buffer, BUF_SIZE);
			if (item <= 0) {
    	        break;
        	}
			write(outputStream, buffer, item);
		}
		close(outputStream);
		close(inputStream);
}

void copyFile(struct PathPair* path) {
    copyFiles(*path->origin, *path->target);
}

int copyDir(char* origin, char* target) {
    DIR* dir;
    struct dirent* ptr = NULL;
    int curosr;
    dir = opendir(origin);
    // makeDir(target);
    struct stat bufferStatus;
    stat(origin, &bufferStatus);
    // mkdir(target, S_IRWXU);
    mkdir(target, bufferStatus.st_mode);
    while ((ptr = readdir(dir)) != NULL) {
        if (4 == (int)ptr->d_type) {
            if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
                continue;
            }
            char* subDirOrigin = (char*)malloc(sizeof(char) * NAME_LENGTH);
            strcpy(subDirOrigin, origin);
            strcat(subDirOrigin, "/");
            strcat(subDirOrigin, (const char*)ptr->d_name);
            char* subDirTarget = (char*)malloc(sizeof(char) * NAME_LENGTH);
            strcpy(subDirTarget, target);
            strcat(subDirTarget, "/");
            strcat(subDirTarget, (const char*)ptr->d_name);
            copyDir(subDirOrigin, subDirTarget);
            free(subDirOrigin);
            free(subDirTarget);
        }
        else if (8 == (int)ptr->d_type) {
            char* subPathOrigin = (char*)malloc(sizeof(char) * NAME_LENGTH);
            strcpy(subPathOrigin, origin);            
            strcat(subPathOrigin, "/");
            strcat(subPathOrigin, (const char*)ptr->d_name);
            char* subPathTarget = (char*)malloc(sizeof(char) * NAME_LENGTH);
            strcpy(subPathTarget, target);
            strcat(subPathTarget, "/");
            strcat(subPathTarget, (const char*)ptr->d_name);
            // printf("Copy: %s %s\n", subPathOrigin, subPathTarget);
            pthread_t tidp;
            struct PathPair* path = initPathPair(&subPathOrigin, &subPathTarget);
            int err = (pthread_create(&tidp, NULL, copyFile, (void*)path));
            if (NIL != err) {
                printf("Create New Copy Thread Error.\n");
            }
            pthread_join(tidp, NULL);
            free(path);
            free(subPathOrigin);
            free(subPathTarget);
        }
    }
    closedir(dir);
    return 1;
}

int main(int argc, char* argv[]) {
    if (3 != argc) {
        printf("Wrong parameters.\n");
        return 0;
    }
    char* origin = (char*)malloc(sizeof(char) * NAME_LENGTH);
    char* target = (char*)malloc(sizeof(char) * NAME_LENGTH);
    if (47 == argv[1][strlen(argv[1]) - 1]) {
        strncpy(origin, argv[1], strlen(argv[1]) - 1);
    }
    else {
        strcpy(origin, argv[1]);
    }
    if (47 == argv[2][strlen(argv[2]) - 1]) {
        strncpy(target, argv[2], strlen(argv[2]) - 1);
    }
    else {
        strcpy(target, argv[2]);
    }
    copyDir(origin, target);
    free(origin);
    free(target);
    return 0;
}
