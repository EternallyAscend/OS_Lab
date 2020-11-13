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

#define THINKING 2
#define HUNGRY 1
#define EATING 0

// int number = 5;
// int* forks = NULL;
int* status = NULL; // 2 means thinking, 1 means hungry, 0 means eating.

sem_t shareMutex;
sem_t** eatMutexs;

void displayStatus() {
    int cursor = 0;
    for(; cursor < NUMBER; cursor++) {
        printf(" %d,", status[cursor]);
    }
    printf("\n");
}

void test(int ID) {
    displayStatus();
    // if (status[ID] == 1 && status[LEFT] != 0 && status[RIGHT] != 0) {
    if (status[ID] == HUNGRY && status[LEFT] != EATING && status[RIGHT] != EATING) {
        printf("Left %d, middle %d, right %d.\n", LEFT, ID, RIGHT);
        // status[ID] = 0;
        status[ID] = EATING;
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
        // status[ID] = 2;
        status[ID] = THINKING;
        test(LEFT);
        test(RIGHT);
        sem_post(&shareMutex);
        counter--;
        if (counter) {
            // Thinking.
            // sleep(rand() % 5 + 3);
            sleep(rand() % 1 + 1);
            // status[ID] = 1;
            status[ID] = HUNGRY;
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

/*
 1, 1, 1, 1, 1,
Left 4, middle 0, right 1.
 0, 1, 1, 1, 1,
P0 start  to eat 1.
 0, 1, 1, 1, 1,
 0, 1, 1, 1, 1,
Left 1, middle 2, right 3.
P2 start  to eat 1.
 0, 1, 0, 1, 1,
P2 finish eating 1.
 0, 1, 2, 1, 1,
 0, 1, 2, 1, 1,
Left 2, middle 3, right 4.
P3 start  to eat 1.
P0 finish eating 1.
 2, 1, 2, 0, 1,
 2, 1, 2, 0, 1,
Left 0, middle 1, right 2.
P1 start  to eat 1.
 2, 0, 1, 0, 1,
P3 finish eating 1.
 2, 0, 1, 2, 1,
 2, 0, 1, 2, 1,
Left 3, middle 4, right 0.
P4 start  to eat 1.
 1, 0, 1, 2, 0,
P0 start  to eat 2.
 1, 0, 1, 1, 0,
P1 finish eating 1.
 1, 2, 1, 1, 0,
 1, 2, 1, 1, 0,
Left 1, middle 2, right 3.
P2 start  to eat 2.
P4 finish eating 1.
 1, 2, 0, 1, 2,
 1, 2, 0, 1, 2,
Left 4, middle 0, right 1.
P0 finish eating 2.
 2, 2, 0, 1, 2,
 2, 2, 0, 1, 2,
 2, 1, 0, 1, 2,
 2, 1, 0, 1, 1,
Left 3, middle 4, right 0.
P4 start  to eat 2.
P2 finish eating 2.
 2, 1, 2, 1, 0,
Left 0, middle 1, right 2.
 1, 0, 2, 1, 0,
P1 start  to eat 2.
 1, 0, 2, 1, 0,
P0 start  to eat 3.
 1, 0, 1, 1, 0,
P4 finish eating 2.
 1, 0, 1, 1, 2,
Left 2, middle 3, right 4.
 1, 0, 1, 0, 2,
P3 start  to eat 2.
P1 finish eating 2.
 1, 2, 1, 0, 2,
Left 4, middle 0, right 1.
 0, 2, 1, 0, 2,
P0 finish eating 3.
 2, 2, 1, 0, 2,
 2, 2, 1, 0, 2,
 2, 2, 1, 0, 1,
P3 finish eating 2.
 2, 2, 1, 2, 1,
Left 1, middle 2, right 3.
 2, 2, 0, 2, 1,
Left 3, middle 4, right 0.
P2 start  to eat 3.
P4 start  to eat 3.
 2, 1, 0, 2, 0,
 1, 1, 0, 2, 0,
P0 start  to eat 4.
 1, 1, 0, 1, 0,
P2 finish eating 3.
 1, 1, 2, 1, 0,
Left 0, middle 1, right 2.
 1, 0, 2, 1, 0,
P1 start  to eat 3.
P0 finish eating 4.
 2, 0, 2, 1, 0,
 2, 0, 2, 1, 0,
P4 finish eating 3.
 2, 0, 2, 1, 2,
Left 2, middle 3, right 4.
 2, 0, 2, 0, 2,
P3 start  to eat 3.
 2, 0, 1, 0, 2,
 1, 0, 1, 0, 2,
P0 start  to eat 5.
 1, 0, 1, 0, 1,
P3 finish eating 3.
 1, 0, 1, 2, 1,
 1, 0, 1, 2, 1,
P1 finish eating 3.
Left 3, middle 4, right 0.
 1, 2, 1, 2, 0,
 1, 2, 1, 2, 0,
Left 1, middle 2, right 3.
P4 start  to eat 4.
P2 start  to eat 4.
P0 finish eating 5.
 2, 2, 0, 2, 0,
 2, 2, 0, 2, 0,
 2, 2, 0, 1, 0,
 2, 1, 0, 1, 0,
 1, 1, 0, 1, 0,
P0 start  to eat 6.
P4 finish eating 4.
 1, 1, 0, 1, 2,
 1, 1, 0, 1, 2,
P2 finish eating 4.
Left 4, middle 0, right 1.
 0, 1, 2, 1, 2,
 0, 1, 2, 1, 2,
Left 2, middle 3, right 4.
P3 start  to eat 4.
P0 finish eating 6.
 2, 1, 2, 0, 2,
 2, 1, 2, 0, 2,
Left 0, middle 1, right 2.
P1 start  to eat 4.
 2, 0, 2, 0, 1,
 2, 0, 1, 0, 1,
 1, 0, 1, 0, 1,
P0 start  to eat 7.
P1 finish eating 4.
 1, 2, 1, 0, 1,
Left 4, middle 0, right 1.
 0, 2, 1, 0, 1,
P3 finish eating 4.
 0, 2, 1, 2, 1,
Left 1, middle 2, right 3.
 0, 2, 0, 2, 1,
P2 start  to eat 5.
P0 finish eating 7.
 2, 2, 0, 2, 1,
Left 3, middle 4, right 0.
 2, 2, 0, 2, 0,
 2, 1, 0, 2, 0,
P4 start  to eat 5.
 2, 1, 0, 1, 0,
 1, 1, 0, 1, 0,
P0 start  to eat 8.
P4 finish eating 5.
 1, 1, 0, 1, 2,
 1, 1, 0, 1, 2,
Left 4, middle 0, right 1.
P2 finish eating 5.
 0, 1, 2, 1, 2,
 0, 1, 2, 1, 2,
Left 2, middle 3, right 4.
P3 start  to eat 5.
P0 finish eating 8.
 2, 1, 2, 0, 2,
 2, 1, 2, 0, 2,
Left 0, middle 1, right 2.
P1 start  to eat 5.
 2, 0, 2, 0, 1,
 2, 0, 1, 0, 1,
 1, 0, 1, 0, 1,
P0 start  to eat 9.
P3 finish eating 5.
 1, 0, 1, 2, 1,
 1, 0, 1, 2, 1,
Left 3, middle 4, right 0.
P4 start  to eat 6.
P1 finish eating 5.
 1, 2, 1, 2, 0,
 1, 2, 1, 2, 0,
Left 1, middle 2, right 3.
P2 start  to eat 6.
 1, 2, 0, 1, 0,
P0 finish eating 9.
 1, 1, 0, 1, 0,
 2, 1, 0, 1, 0,
 2, 1, 0, 1, 0,
P2 finish eating 6.
 2, 1, 2, 1, 0,
Left 0, middle 1, right 2.
 2, 0, 2, 1, 0,
P1 start  to eat 6.
P4 finish eating 6.
 2, 0, 2, 1, 2,
Left 2, middle 3, right 4.
 2, 0, 2, 0, 2,
P3 start  to eat 6.
 1, 0, 1, 0, 2,
P0 start  to eat 10.
 1, 0, 1, 0, 1,
 1, 0, 1, 0, 1,
P1 finish eating 6.
 1, 2, 1, 0, 1,
Left 4, middle 0, right 1.
 0, 2, 1, 0, 1,
P0 finish eating 10.
 2, 2, 1, 0, 1,
 2, 2, 1, 0, 1,
P3 finish eating 6.
 2, 2, 1, 2, 1,
Left 1, middle 2, right 3.
 2, 2, 0, 2, 1,
Left 3, middle 4, right 0.
P4 start  to eat 7.
P2 start  to eat 7.
 2, 1, 0, 2, 0,
P4 finish eating 7.
 2, 1, 0, 1, 0,
P2 finish eating 7.
 2, 1, 0, 1, 2,
 2, 1, 0, 1, 2,
 2, 1, 2, 1, 2,
Left 0, middle 1, right 2.
 2, 0, 2, 1, 2,
P1 start  to eat 7.
Left 2, middle 3, right 4.
P3 start  to eat 7.
 2, 0, 1, 0, 1,
P1 finish eating 7.
 2, 0, 1, 0, 1,
 2, 2, 1, 0, 1,
 2, 2, 1, 0, 1,
P3 finish eating 7.
 2, 2, 1, 2, 1,
Left 1, middle 2, right 3.
 2, 2, 0, 2, 1,
Left 3, middle 4, right 0.
P4 start  to eat 8.
P2 start  to eat 8.
 2, 1, 0, 2, 0,
 2, 1, 0, 1, 0,
P4 finish eating 8.
 2, 1, 0, 1, 2,
 2, 1, 0, 1, 2,
P2 finish eating 8.
 2, 1, 2, 1, 2,
Left 0, middle 1, right 2.
 2, 0, 2, 1, 2,
Left 2, middle 3, right 4.
P1 start  to eat 8.
P3 start  to eat 8.
 2, 0, 2, 0, 1,
 2, 0, 1, 0, 1,
P1 finish eating 8.
 2, 2, 1, 0, 1,
 2, 2, 1, 0, 1,
P3 finish eating 8.
 2, 2, 1, 2, 1,
Left 1, middle 2, right 3.
 2, 2, 0, 2, 1,
Left 3, middle 4, right 0.
P2 start  to eat 9.
P4 start  to eat 9.
 2, 1, 0, 2, 0,
 2, 1, 0, 1, 0,
P4 finish eating 9.
 2, 1, 0, 1, 2,
 2, 1, 0, 1, 2,
P2 finish eating 9.
 2, 1, 2, 1, 2,
Left 0, middle 1, right 2.
 2, 0, 2, 1, 2,
Left 2, middle 3, right 4.
P1 start  to eat 9.
P3 start  to eat 9.
 2, 0, 2, 0, 1,
 2, 0, 1, 0, 1,
P3 finish eating 9.
 2, 0, 1, 2, 1,
 2, 0, 1, 2, 1,
Left 3, middle 4, right 0.
P4 start  to eat 10.
P1 finish eating 9.
 2, 2, 1, 2, 0,
 2, 2, 1, 2, 0,
Left 1, middle 2, right 3.
P2 start  to eat 10.
 2, 2, 0, 1, 0,
P4 finish eating 10.
 2, 2, 0, 1, 2,
 2, 2, 0, 1, 2,
Finish, exit.

*/
