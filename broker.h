// Cole Kincaid 822602112

#include <queue>
#include <time.h>
#include "ridesharing.h"
#include "rideRequest.h"
#include "semaphore.h"

#define NSPERMS 1000000
#define MSPERSEC 1000

#ifndef BROKER_H
#define BROKER_H

class broker {
    public:
        // queue (bounded buffer)
        std::queue<rideRequest> boundedBuffer; 
        
        // array for keeping track of how many requests have been produced of each type (human or robot)
        int numRequestsProducedPerType[RequestTypeN]; 

        // array for keeping track of how many requests have been consumed of each type (human or robot)
        int numRequestsConsumedPerType[ConsumerTypeN]; 

        int costAlgoConsumed[RequestTypeN];
        int fastAlgoConsumed[RequestTypeN];

        // the delay in ms that each production and consumption will have
        int productionDelay[RequestTypeN];
        int consumptionDelay[ConsumerTypeN];

        // The number of requests within the broker queue of each type
        int currRequestsInQueueStats[RequestTypeN];

        // total number of requests that need to be produced
        int totalNumberToProduce;

        // total number of requests that have been produced
        int totalProduced;

        // semaphor pointers 
        sem_t* mutex; 
        sem_t* availiableSlots;
        sem_t* availiableHumanSlots;
        sem_t* unconsumed;
        sem_t* mainProductionBarrier;
        sem_t* mainConsumptionBarrier;

        // brokerQueue constructor
        broker(sem_t* mutexSem, sem_t* availiableSlotsSem, sem_t* availiableHumanSlotsSem, sem_t* unconsumedSem, sem_t* barrierProduction, sem_t* barrierConsumption, int roboDelay, int humanDelay, int fastMatDelay, int costSavDelay, int numToProduce);
        
        // produce request function
        void produceRideRequest(rideRequest requestToProduce);

        // consume request function
        rideRequest consumeRideRequest(rideRequest requestToConsume, Consumers consumerAlgo); // updates broker queue when consuming a request
};

#endif