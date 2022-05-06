// Cole Kincaid 822602112

#include "broker.h"


/**
 * Broker object constructor function
 * 
 * Inputs: sem_t* [mutexSem, availiableSlotsSem, AvailiableHuamnSlotsSem, unconsumedSem, barrierProduction, barrierConsumption],
 *         int roboDelay, int humanDelay, int fastMatDelay, int costSavDelay, int numToProduce 
 * 
 * initializes all of the class variables within a new instance of a broker object
 */
broker::broker(sem_t* mutexSem, sem_t* availiableSlotsSem, sem_t* availiableHumanSlotsSem, sem_t* unconsumedSem, sem_t* barrierProduction, sem_t* barrierConsumption, int roboDelay, int humanDelay, int fastMatDelay, int costSavDelay, int numToProduce){
    // semaphores
    mutex = mutexSem;
    availiableSlots = availiableSlotsSem;
    availiableHumanSlots = availiableHumanSlotsSem;
    unconsumed = unconsumedSem;
    mainProductionBarrier = barrierProduction;
    mainConsumptionBarrier = barrierConsumption;

    // storing delays of producer adn consumer threads
    productionDelay[HumanDriver] = humanDelay;
    productionDelay[RoboDriver] = roboDelay;
    consumptionDelay[CostAlgoDispatch] = costSavDelay;
    consumptionDelay[FastAlgoDispatch] = fastMatDelay;

    // the number of requests that will be produced
    totalNumberToProduce = numToProduce;

    // sets consuption and production stats variables to zero
    numRequestsConsumedPerType[HumanDriver] = 0;
    numRequestsConsumedPerType[RoboDriver] = 0;
    numRequestsProducedPerType[HumanDriver] = 0;
    numRequestsProducedPerType[RoboDriver] = 0;
    currRequestsInQueueStats[HumanDriver] = 0;
    currRequestsInQueueStats[RoboDriver] = 0;
    totalProduced = 0;
}

/**
 * produceRideRequest function
 * 
 * Inputs: rideRequest requestToProduce (an object containing a class var that holds what type of request it is, robo or human)
 * 
 * pushes rideRequest object to the boundedBuffer that is within the broker object (called brokerQueue in main)
 * also increments the number of requests produced per type, how many of each type are in the queue at the moment,
 * and the total number of requests that have been produced so far 
 */
void broker::produceRideRequest(rideRequest requestToProduce){
    boundedBuffer.push(requestToProduce);
    if(requestToProduce.requestType == HumanDriver){
        numRequestsProducedPerType[HumanDriver]++;
        currRequestsInQueueStats[HumanDriver]++;
        totalProduced++;
    }
    if(requestToProduce.requestType == RoboDriver){
        numRequestsProducedPerType[RoboDriver]++;
        currRequestsInQueueStats[RoboDriver]++;
        totalProduced++;
    }  
}

/**
 * consumeRideRequest function
 * 
 * Inputs: rideRequest requestToConsume (the request that will be removed, which is the front of the queue),
 *         Consumers consumerAlgo (the type of algo that is consuming)
 * 
 * removes the rideRequest that is at the front of the boundedBuffer queue within the broker object,
 * increments the number of requests consumed by type(robo or human), the number of requests of each type consumed by
 * consumer thread, also decrements the number of requests that are within the queue at the moment
 * 
 * Returns: a pointer to the rideRequest that was removed(if needed at some point, but it isn't used in this
 *          implementation of a consumer)
 *  
 */
rideRequest broker::consumeRideRequest(rideRequest requestToConsume, Consumers consumerAlgo){
    rideRequest* tmp;
    tmp = &requestToConsume;
    boundedBuffer.pop();
    if(requestToConsume.requestType == HumanDriver){
        numRequestsConsumedPerType[HumanDriver]++;
        if(consumerAlgo == CostAlgoDispatch)
            costAlgoConsumed[HumanDriver]++;
        else
            fastAlgoConsumed[HumanDriver]++;
        currRequestsInQueueStats[HumanDriver]--;
    }
        
    if(requestToConsume.requestType == RoboDriver){
        numRequestsConsumedPerType[RoboDriver]++;
        if(consumerAlgo == CostAlgoDispatch)
            costAlgoConsumed[RoboDriver]++;
        else
            fastAlgoConsumed[RoboDriver]++;
        currRequestsInQueueStats[RoboDriver]--;
    }
        

    return *tmp;
}