// Cole Kincaid 822602112

#include <pthread.h>
#include "consumer.h"
#include "io.h"


/**
 * consumer function (called within a consumer thread)
 * 
 * Input: void* ptr (passes the shared data to the thread)
 * 
 * consumes requests from the boundedBuffer within the broker object, uses semaphores to control the consumption of the requests
 * from the queue which is a critical section, needs mutal exclusivity to make changes to the queue
 */
void* consumer( void* ptr ){
    struct consumption_arg* tmp = (struct consumption_arg*) ptr;
    Consumers consumerType = tmp->typeConsumer;
    broker *brokerQueue;
    brokerQueue = tmp->brokerPtr;
    int consDelay = brokerQueue->consumptionDelay[consumerType];

    rideRequest* consumableRequest;

    // used for sleeping the thread when am item is consumed
    struct timespec SleepTime;
    SleepTime.tv_sec = consDelay / MSPERSEC; // number of seconds
    SleepTime.tv_nsec = (consDelay % MSPERSEC) * NSPERMS; // number of nanoseconds
    int* tmpSemVal;

    // keep trying to consume until both the producers are done producing and the boundedBuffer is empty
    while(brokerQueue->totalProduced != brokerQueue->totalNumberToProduce || brokerQueue->boundedBuffer.size() != 0){
        // Block until something to consume
        sem_wait(brokerQueue->unconsumed);

        // requesting access to the broker queue exclusively
        sem_wait(brokerQueue->mutex);

        // finding the request that is at the front of the queue to pass to the consume func
        consumableRequest = &brokerQueue->boundedBuffer.front(); 
        brokerQueue->consumeRideRequest(*consumableRequest, consumerType);
        io_remove_type(consumerType, consumableRequest->requestType, brokerQueue->currRequestsInQueueStats, brokerQueue->numRequestsConsumedPerType);

        // signaling that it is done in the critial section and that another thread is allowed to get access 
        sem_post(brokerQueue->mutex);

        if (consumableRequest->requestType == HumanDriver){ // if the removed request was human then signal that there is room for another human request or robo request
            sem_post(brokerQueue->availiableHumanSlots);
            sem_post(brokerQueue->availiableSlots);
        }
        else{
            sem_post(brokerQueue->availiableSlots); // if removed request is robo signal that there is another open spot in the queue
        }
        nanosleep(&SleepTime, NULL); // simulate consuming request
    }
    sem_post(brokerQueue->mainConsumptionBarrier); // signals to main that this thread has finished
    return NULL;
}