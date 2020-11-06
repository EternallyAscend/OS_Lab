#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <syst/sem.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 15
#define PRODUCER_NUMBER 5
#define CUSTOMER_NUMBER 4
#define SHARED_POOL_SIZE 3
#define NIL 0

// FinishProduct 15 -> stop producers.
// FinishCustom 15 -> stop customers.
// Full 0 -> protect customer.
// Empty 3 -> protect producer.
// Mutex -> protect sharePool[SHARED_POOL_SIZE] and tail.
sem_t finishProductMutex;
sem_t finishCustomMutex;
sem_t full;
sem_t empty;
sem_t mutex;

int finishProduct = 0;
int finishCustom = 0;

struct Product** sharedPool;
// struct Product** sharedPool = (struct Product**)malloc(sizeof(struct Product*) * SHARED_POOL_SIZE);
// struct Product[SHARED_POOL_SIZE] sharedPool; 
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
    printf("Product ID: %d\n", product->ID);
    printf("Product producer: %d\n", product->producer);
    printf("Product createTime: %d\n", product->createTime);
    printf("Product bufferID: %d\n", product->bufferID);
    printf("Product custormer: %d\n", product->customer);
}

void echoSharedPool() {
    printf("______________________\n");
    int cursor = 0;
    for(; cursor < tail; cursor++) {
        echoProduct(sharedPool[cursor]);
    }
    printf("______________________\n");
    printf("\n");
}

void purchaseProduct(int customer, struct Product* product) {
    sem_wait(&full);
    product->customer = customer;
    sem_post(&full);
}

void runProducer(int ID) {
    srand((unsigned)(time(NULL)));
    while(1) {
        struct Product* product = createProduct(ID);
        sem_wait(&finishProductMutex);
        if (MAX_SIZE == finishProduct) {
            sem_post(&finishCustomMutex);
            free(product);
            return;
        }
        else {
            product->ID = finishProduct;
            finishProduct++;
            sem_wait(&empty);
            echoSharedPool();
            sharedPool[tail++] = product;
            echoSharedPool();
            sem_post(&full);
            sem_post(&empty);
        }
        sem_post(&finishProductMutex);

        // usleep(rand()%4000000+1000000);
        usleep(rand()%40000+10000);
    }
}

void runCustomer(int ID) {
srand((unsigned)(time(NULL)));
    while(1) {
        createProduct(ID);
        sem_wait(&finishCustomMutex);
        if (MAX_SIZE == finishCustom) {
            sem_post(&finishCustomMutex);
            return;
        }
        else {
            finishCustom++;
            sem_wait(&full);
            // purchaseProduct(sharedPool[tail--]);
            sharedPool[tail - 1]->customer = ID;
            echoSharedPool();
            free(sharedPool[tail--]);
            echoSharedPool();
            sem_post(&empty);
            sem_post(&full);
        }
        sem_post(&finishCustomMutex);
        // usleep(rand()%4000000+1000000);
        usleep(rand()%40000+10000);
    }
}

int main(void) {
    sharedPool = (struct Product**)malloc(sizeof(struct Product*) * SHARED_POOL_SIZE);
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
    int cursor = 0;
    void *ret;
    pthread_t** pids = (pthread_t*)malloc(sizeof(pthread_t*) * (PRODUCER_NUMBER + CUSTOMER_NUMBER));
    for (; cursor < PRODUCER_NUMBER; cursor++) {
        pthread_t pid;
        pids[cursor] = &pid;
        pthread_create(&pid, NULL, runProducer, cursor);
    }
    for (cursor = 0; cursor < CUSTOMER_NUMBER; cursor++) {
        pthread_t pid;
        pids[cursor + PRODUCER_NUMBER] = &pid;
        pthread_create(&pid, NULL, runCustomer, cursor);
    }
    for (cursor = 0; cursor < PRODUCER_NUMBER + CUSTOMER_NUMBER; cursor++) {
        pthread_join(pids[cursor], &ret);
    }
    free(pids);
    free(sharedPool);
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);
    sem_destroy(&finishCustomMutex);
    sem_destroy(&finishProductMutex);
    printf("Exit pragram successfully.\n");
    return 0;
}
