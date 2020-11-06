#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 15
#define PRODUCER_NUMBER 5
#define CUSTOMER_NUMBER 4
#define SHARE_SIZE 3

struct Product {
    int ID;
    int producer;
    int createTime;
    int bufferID;
    int customer;
}

struct Product* createProduct(int producer) {
    struct Product* pointer = (struct Product*)malloc(sizeof(struct Product));
    pointer->ID;
    pointer->producer = producer;
    pointer->createTime = time();
    pointer->bufferID = ;

}

void purchaseProduct(int customer, struct Product* product) {
    product->customer = customer;
}

void runProducer(int ID) {

}

void runCustomer(int ID) {

}

int main(void) {
    int cursor = 0;
    for (; cursor < PRODUCER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runProducer, cursor);
    }
    for (cursor = 0; cursor < CUSTOMER_NUMBER; cursor++) {
        pthread_t pid;
        pthread_create(&pid, NULL, runCustomer, cursor);
    }
    return 0;
}
