// Cole Kincaid 822602112

#include "producer.h"
#include "io.h"
#include <pthread.h>
#include <iostream>

#define TAKESCAREOFOVERPRODUCTION 1

/**
 * producer function (called within a producer thread)
 * 
 * Input: void* ptr (passes the shared data to the thread)
 * 
 * produced requests and pushes them to the bounded buffer, thread waits until there is room to add the type of request
 * that that thread produces to the boundedBuffer (needs mutual exclisivity to the boundedBuffer to add to the boundedBuffer)
 */
void* producer( void* ptr){
    struct production_arg* tmp = (struct production_arg*) ptr;
    Requests rideType = tmp->typeRequest;
    broker *brokerQueue;
    brokerQueue = tmp->brokerPtr;
    int prodDelay = brokerQueue->productionDelay[rideType];
    
    rideRequest* newRequest;

    struct timespec SleepTime;
    SleepTime.tv_sec = prodDelay / MSPERSEC; // number of seconds
    SleepTime.tv_nsec = (prodDelay % MSPERSEC) * NSPERMS; // number of nanoseconds

    // continue producing until you have produced the max number to produce (specified by optional args, or is default 120)
    // TAKESCAREOFOVERPRODUCTION is used because it kept overproducing requests by one and this is because one thread could 
    // be producing the last request and the other production thread might have gotten stuck in a wait before the production
    // from the other is complete and would cause the production of an extra request
    while (brokerQueue->totalProduced < brokerQueue->totalNumberToProduce - TAKESCAREOFOVERPRODUCTION){

        newRequest = new rideRequest(rideType);
        // make sure ther is room in the queue
        if (rideType == HumanDriver){ // check to see if there is room for a human driver (you can have a max of 4)
            sem_wait(brokerQueue->availiableHumanSlots);
        }
        sem_wait(brokerQueue->availiableSlots); //checks to see if the is room for within the queue

        // wait certain amount of time to produce the request
        nanosleep(&SleepTime, NULL); // simulate producing request

        // access broker queue exclusively
        sem_wait(brokerQueue->mutex);

        // calling produceRideRequest, which will add a new request to the boundedBuffer
        brokerQueue->produceRideRequest(*newRequest);
        io_add_type(rideType, brokerQueue->currRequestsInQueueStats, brokerQueue->numRequestsProducedPerType);

        // signaling to other threads that they can have the mutex
        sem_post(brokerQueue->mutex);

        sem_post(brokerQueue->unconsumed); //inform consumer that there is a request that can be consumed
    }
    sem_post(brokerQueue->mainProductionBarrier); //signals to main that this thread has finished
    return NULL;
}