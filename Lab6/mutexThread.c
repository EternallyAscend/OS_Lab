#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUMBER 5
#define SHARE 5
#define NIL 0
#define LEFT (ID+NUMBER-1)%NUMBER
#define RIGHT (ID+1)%NUMBER

// int number = 5;
// int* forks = NULL;
int* status = NULL; // 2 means thinking, 1 means hungry, 0 means eating.

sem_t shareMutex;
sem_t** eatMutexs;

void test(int ID) {
    if (status[ID] == 1 && status[LEFT] != 0 && status[RIGHT] != 0) {
        printf("Left %d, middle %d, right %d.\n", LEFT, ID, RIGHT);
        status[ID] = 0;
        sem_post(eatMutexs[ID]);
    }
}

void* runPhilosophers(int ID) {
    int counter = 10;
    srand((unsigned)(time(NULL)));
    while(counter) {
        // Eating.
        sem_wait(&shareMutex);
        test(ID);
        sem_post(&shareMutex);
        sem_wait(eatMutexs[ID]);
        printf("P%d start  to eat %d.\n", ID, 11 - counter);
        // sleep(rand() % 8 + 2);
        sleep(rand() % 2 + 1);
        printf("P%d finish eating %d.\n", ID, 11 - counter);
        sem_wait(&shareMutex);
        status[ID] = 2;
        test(LEFT);
        test(RIGHT);
        sem_post(&shareMutex);
        counter--;
        if (counter) {
            // Thinking.
            // sleep(rand() % 5 + 3);
            sleep(rand() % 1 + 1);
            status[ID] = 1;
        }
    }
    return 0;
}

int main(void) {
    eatMutexs = (sem_t**)malloc(sizeof(sem_t*) * NUMBER);
    int err = 0;
    err = sem_init(&shareMutex, 0, 1);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }

    // forks = (int*)malloc(sizeof(int) * NUMBER);
    status = (int*)malloc(sizeof(int) * NUMBER);
    pthread_t** threadPool = (pthread_t**)malloc(sizeof(pthread_t*));
    int cursor = 0;
    for(; cursor < NUMBER; cursor++) {
        eatMutexs[cursor] = (sem_t*)malloc(sizeof(sem_t));
        err = sem_init(eatMutexs[cursor], 0, 0);
        if (NIL != err) {
            printf("Semaphore initialization failed.\n");
            exit(-1);
        }
        status[cursor] = 1;
    }
    for(cursor = 0; cursor < NUMBER; cursor++) {
        pthread_t pid;
        threadPool[cursor] = &pid;
        pthread_create(&pid, NULL, runPhilosophers, cursor);
    }
    for(; cursor > 0; cursor--) {
        pthread_join(*threadPool[cursor - 1], NULL);
    }
    printf("Finish, exit.\n");
    free(threadPool);
    free(status);
}


// // FinishProduct 0 to MAX_SIZE -> stop producers.
// // FinishCustom 0 to MAX_SIZE -> stop customers.
// int finishProduct = 0;
// int finishCustom = 0;

// // Threads 0 -> PRODUCER_NUMBER + CUSTOMER_NUMBER count thread numbers, notice main thread exit.
// int threads = 0;

// // FinishProductMutex -> protect finishProduct.
// // FinsihCustomMutex -> protect finishCustom.
// // Full 0 -> protect customer.
// // Empty 3 -> protect producer.
// // Mutex -> protect sharePool[SHARED_POOL_SIZE] and tail.
// // ThreadMutex -> protect threads counter.
// // stopMutex -> block main thread.
// sem_t finishProductMutex;
// sem_t finishCustomMutex;
// sem_t full;
// sem_t empty;
// sem_t mutex;
// sem_t threadMutex;
// sem_t stopMutex;
