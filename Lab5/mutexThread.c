#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 15
#define PRODUCER_NUMBER 5
#define CUSTOMER_NUMBER 4
#define SHARED_POOL_SIZE 3
#define NIL 0

// FinishProduct 0 to MAX_SIZE -> stop producers.
// FinishCustom 0 to MAX_SIZE -> stop customers.
int finishProduct = 0;
int finishCustom = 0;

// Threads 0 -> PRODUCER_NUMBER + CUSTOMER_NUMBER count thread numbers, notice main thread exit.
int threads = 0;

// FinishProductMutex -> protect finishProduct.
// FinsihCustomMutex -> protect finishCustom.
// Full 0 -> protect customer.
// Empty 3 -> protect producer.
// Mutex -> protect sharePool[SHARED_POOL_SIZE] and tail.
// ThreadMutex -> protect threads counter.
// stopMutex -> block main thread.
sem_t finishProductMutex;
sem_t finishCustomMutex;
sem_t full;
sem_t empty;
sem_t mutex;
sem_t threadMutex;
sem_t stopMutex;

struct Product** sharedPool;
int tail = 0;

struct Product {
    int ID;
    int producer;
    int createTime;
    int bufferID;
    int customer;
};

struct Product* createProduct(int producer) {
    struct Product* pointer = (struct Product*)malloc(sizeof(struct Product));
    pointer->ID = 0;
    pointer->producer = producer;
    pointer->createTime = time(NULL);
    pointer->bufferID = 0;
    pointer->customer = -1;
}

void echoProduct(struct Product* product) {
    printf(" - Product ID: %d\n", product->ID);
    printf("    Product producer: %d\n", product->producer);
    printf("    Product createTime: %d\n", product->createTime);
    printf("    Product bufferID: %d\n", product->bufferID);
    printf("    Product custormer: %d\n", product->customer);
}

void echoSharedPool() {
    printf("____________________________________________\n");
    if (0 != tail) {
        int cursor = 0;
        for(; cursor < tail; cursor++) {
            printf("%d", cursor);
            echoProduct(sharedPool[cursor]);
        }
    }
    else {
        printf(" Empty SharedPool.\n");
    }
    printf("============================================\n");
}

void exitThread() {
    sem_wait(&threadMutex);
    threads--;
    if (0 == threads) {
        sem_post(&stopMutex);
    }
    sem_post(&threadMutex);
}

void *runProducer(int ID) {
    srand((unsigned)(time(NULL)));
    while(1) {
        struct Product* product = createProduct(ID);
        sem_wait(&finishProductMutex);
        // printf("P%d Lock. Current: %d.\n", ID, finishProduct);
        if (MAX_SIZE == finishProduct) {
            sem_post(&finishProductMutex);
            printf("P%d exit without create.\n", ID);
            free(product);
            exitThread();
            // pthread_exit(0);
            return 0;
        }
        else {
            product->ID = finishProduct;
            finishProduct++;
            sem_wait(&empty);
            sem_wait(&mutex);
            printf("Before P%d create.\n", ID);
            echoSharedPool();
            product->bufferID = tail;
            sharedPool[tail++] = product;
            printf("P%d Create %d.\n", ID, finishProduct - 1);
            echoSharedPool();
            sem_post(&mutex);
            sem_post(&full);
        }
        sem_post(&finishProductMutex);
        // printf("P%d Unlock.\n", ID);
        sleep(rand() % 4 + 1);
    }
}

void *runCustomer(int ID) {
    srand((unsigned)(time(NULL)));
    while(1) {
        sem_wait(&finishCustomMutex);
        // printf("C%d Lock. \n", ID);
        if (MAX_SIZE == finishCustom) {
            sem_post(&finishCustomMutex);
            printf("C%d exit without Purchase.\n", ID);
            exitThread();
            // pthread_exit(0);
            return 0;
        }
        else {
            finishCustom++;
            sem_wait(&full);
            // purchaseProduct(sharedPool[tail--]);
            sem_wait(&mutex);
            sharedPool[tail - 1]->customer = ID;
            printf("Before C%d purchase.\n", ID);
            echoSharedPool();
            printf("C%d Purchase %d.\n", ID, sharedPool[tail - 1]->ID);
            free(sharedPool[--tail]);
            echoSharedPool();
            sem_post(&mutex);
            sem_post(&empty);
        }
        sem_post(&finishCustomMutex);
        // printf("C%d Unlock.\n", ID);
        sleep(rand() % 4 + 1);
    }
}

int main(void) {
    int err = 0;
    err = sem_init(&finishProductMutex, 0, 1);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&finishCustomMutex, 0, 1);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&mutex, 0, 1);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&full, 0, 0);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&empty, 0, SHARED_POOL_SIZE);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&threadMutex, 0, 1);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    err = sem_init(&stopMutex, 0, 0);
    if (NIL != err) {
        printf("Semaphore initialization failed.\n");
        exit(-1);
    }
    sharedPool = (struct Product**)malloc(sizeof(struct Product*) * SHARED_POOL_SIZE);
    int cursor = 0;
    for (; cursor < PRODUCER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runProducer, cursor);
        sem_wait(&threadMutex);
        threads++;
        sem_post(&threadMutex);
    }
    for (cursor = 0; cursor < CUSTOMER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runCustomer, cursor);
        sem_wait(&threadMutex);
        threads++;
        sem_post(&threadMutex);
    }
    sem_wait(&stopMutex);
    free(sharedPool);
    sem_destroy(&finishProductMutex);
    sem_destroy(&finishCustomMutex);
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&mutex);
    sem_destroy(&threadMutex);
    sem_destroy(&stopMutex);
    printf("----------------------------------------\nExit pragram successfully.\n");
    return 0;
}
