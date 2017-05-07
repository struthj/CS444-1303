#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mt19937ar.c"
//32 total items
#define BUFFERSIZE 32

//reference: https://linux.die.net/man/3/sleep
//reference: https://www.guyrutenberg.com/2014/05/03/c-mt19937-example/
//reference: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
//reference: https://www.cs.nmsu.edu/~jcook/Tools/pthreads/pc.c
//reference: http://nirbhay.in/blog/2013/07/producer_consumer_pthreads/
//reference: http://stackoverflow.com/questions/35403892/creating-threads-in-a-loop
//Partner: Kevin T.

//shared buffer for data
struct buffer itemBuffer;
//index for itemBuffer
int consumerNum = 0;
int producerNum = 0;


//Conditions to signal that an item is ready to consume
//and that an item has been consumed and needs another
pthread_cond_t consumerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t producerCond = PTHREAD_COND_INITIALIZER;


struct item
{
    int number;
    int wait;
};

struct buffer
{
    struct item items[BUFFERSIZE];
    pthread_mutex_t shareLock;
};

int checkForx86()
{
    //general purpose registers
    unsigned int eax = 0x01;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
    //if data size greater than 32bit
    if (ecx & 0x40000000)
    {
        //32 bit system use rdrand
        return 1;
    }
    else{
        //64 bit
        return 0;
    }
}

int genRandomNumber(int floor, int ceiling)
{
    int num = 0;
    if(checkForx86() == 1)
    {
        //use rdrand
        __asm__ __volatile__("rdrand %0":"=r"(num));
    }
    else
    {
        //use mersenne twister
        num =  (int)genrand_int32();
    }
    num = abs(num % (ceiling - floor));
    if(num < floor)
    {
        return floor;
    }
    return num;
}

void printItem(struct item *myItem)
{
    printf("Item number: %d\n", myItem->number);
    printf("Item wait: %d\n", myItem->wait);
}

void *consumer(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&itemBuffer.shareLock);
        if(consumerNum >= BUFFERSIZE)
        {
            consumerNum = 0;
        }
        //item to be consumed
        struct item consumeItem;
        //signal producer consumer is ready
        pthread_cond_signal(&producerCond);
        //wait for a number from producer
        pthread_cond_wait(&consumerCond, &itemBuffer.shareLock);
        //if a consumer thread arrives while the buffer is empty
        //it blocks until a producer adds a new item.
        if(producerNum == 0)
        {
            printf("AT MAX size3\n");
            pthread_cond_wait(&consumerCond, &itemBuffer.shareLock);
        }
        //get item to consume from buffer
        consumeItem = itemBuffer.items[consumerNum];
        //increase the count of consumed items
        consumerNum++;
        //random waiting period
        sleep(consumeItem.wait);
        //consume item from buffer
        printf("Consuming Item data: %d\n", consumeItem.number);
        //resize if at max buffer size
        if(consumerNum >= BUFFERSIZE)
        {
            consumerNum = 0;
        }
        //ready to consume again
        //pthread_cond_signal(&producerCond);
        //unlock shared buffer
        pthread_mutex_unlock(&itemBuffer.shareLock);
        //exit condition for testing/debug
//        if(consumerNum == BUFFERSIZE)
//        {
//            printf("MAX ITEMS!\n");
//            break;
//        }
    }
}

void *producer(void *foo)
{
    while(1)
    {
        //lock shared buffer
        pthread_mutex_lock(&itemBuffer.shareLock);
        //item to be produced
        struct item newItem;
        //data value and wait time using Mersenne Twister
        newItem.number = genRandomNumber(1, 100);
        newItem.wait = genRandomNumber(2, 9);
        if(consumerNum >= BUFFERSIZE)
        {
            consumerNum = 0;
        }
        printf("Producing Item:\n");
        printItem(&newItem);
        //block until consumer removes an item
        if(producerNum == 31)
        {
            printf("AT MAX size 1\n");
            pthread_cond_signal(&consumerCond);
            pthread_cond_wait(&producerCond, &itemBuffer.shareLock);
        }
        //add item to buffer
        itemBuffer.items[producerNum] = newItem;
        producerNum++;
        //tell consumer a new item is ready
        pthread_cond_signal(&consumerCond);
        //wait for consumer to consume
        pthread_cond_wait(&producerCond, &itemBuffer.shareLock);
        //resize if at max buffer size
        if(producerNum >= BUFFERSIZE)
        {
            printf("AT MAX size 2\n");
            producerNum = 0;
        }
        //ready to consume
        //pthread_cond_signal(&consumerCond);
        //shared buffer unlock
        pthread_mutex_unlock(&itemBuffer.shareLock);
    }

}

int main(int argc, char *argv[])
{
    //check for user input number of threads
    if(argc != 2)
    {
        printf("Usage is: %s <number of threads>\n", argv[0]);
        exit(0);
    }
    //user enters number of threads
    int numThreads = atoi(argv[1]);
    //producer and consumer threads with enough space for user entered number of threads
    pthread_t threads[2 * numThreads];
    //index var
    int i = 0;
    //create threads
    for(i = 0; i < numThreads; i++)
    {
        pthread_create(&threads[i], NULL, consumer, NULL);
        pthread_create(&threads[i + 1], NULL, producer, NULL);
    }
    //join threads
    for(i = 0; i < 2 * numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
