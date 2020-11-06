#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syst/sem.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 15
#define PRODUCER_NUMBER 5
#define CUSTOMER_NUMBER 4
#define SHARED_POOL_SIZE 3

// FinishProduct 15 -> stop producers.
// FinishCustom 15 -> stop customers.
// Full 0 -> protect customer.
// Empty 3 -> protect tail.
// Mutex -> protect sharePool[SHARED_POOL_SIZE].

struct Product {
    int ID;
    int producer;
    int createTime;
    int bufferID;
    int customer;
};

struct Product* createProduct(int producer) {
    struct Product* pointer = (struct Product*)malloc(sizeof(struct Product));
    pointer->ID;
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

void purchaseProduct(int customer, struct Product* product) {
    product->customer = customer;
}

void runProducer(int ID) {
    srand((unsigned)(time(NULL)));
    while(1) {
        // createProduct(ID);
        // usleep(rand()%4000000+1000000);
        usleep(rand()%400000+100000);
    }
}

void runCustomer(int ID) {

}

int main(void) {
    int cursor = 0;
    void *ret;
    struct Product** sharedPool = (struct Product**)malloc(sizeof(struct Product*) * SHARED_POOL_SIZE);
    int tail = 0;

    for (; cursor < PRODUCER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runProducer, cursor);
        pthread_join(pid, &ret);
    }
    for (cursor = 0; cursor < CUSTOMER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runCustomer, cursor);
    }
    return 0;
}
