// Cole Kincaid 822602112

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <pthread.h>
#include "producer.h"
#include "consumer.h"
#include "ridesharing.h"
#include "broker.h"
#include "semaphore.h"
#include "io.h"

#define DEFAULT_ARG -1
#define DEFAULT_DELAY 0
#define DEFAULT_NUM_REQUESTS 120
#define QUEUE_MAX_SIZE 12
#define MAX_NUM_HUMAN_DRIVERS 4
#define MUTEX 1
#define ZERO 0
#define MAX_NUM_ARGS 11

int main(int argc, char **argv){

    // error checking to make sure that there aren't more optional arguements that allowed
    if (argc > MAX_NUM_ARGS){
        std::cout << "Too many command line arguments." << std::endl;
        exit(1);
    }

    // Optional arguments default values
    int totalNumRequests = DEFAULT_NUM_REQUESTS; // -n
    int costSavingDispatchDelay = DEFAULT_DELAY, fastMatchingDispatchDelay = DEFAULT_DELAY; // -c, -f
    int produceHumanDriverDelay = DEFAULT_DELAY, produceAutonomousDriverDelay = DEFAULT_DELAY; // -h, -a

    // takes care of the optional arguments that are passed via command line using getopt
    int option;
    while((option = getopt(argc, argv, "n:c:f:h:a:")) != DEFAULT_ARG) {
        switch(option){
            case 'n':
                totalNumRequests = std::stoi(optarg);
                break;
            case 'c':
                costSavingDispatchDelay = std::stoi(optarg);
                break;
            case 'f':
                fastMatchingDispatchDelay = std::stoi(optarg);
                break;
            case 'h':
                produceHumanDriverDelay = std::stoi(optarg);
                break;
            case 'a':
                produceAutonomousDriverDelay = std::stoi(optarg);
                break;
            case ':':
                break;
            case '?':
                break;
        }
    }

    // Mutual Exclusion Semaphore
    sem_t mutex;
    sem_init(&mutex, ZERO, MUTEX);

    // availiable spots in queue Semaphore
    sem_t availiableSlots;
    sem_init(&availiableSlots, ZERO, QUEUE_MAX_SIZE);

    // availiable human slots in queue Semaphore
    sem_t availiableHumanSlots;
    sem_init(&availiableHumanSlots, ZERO, MAX_NUM_HUMAN_DRIVERS);

    // unconsumed Semaphore
    sem_t unconsumed;
    sem_init(&unconsumed, ZERO, ZERO);

    // Barrier with semaphores (set to zero and will be used to block main until the thread is done completeing)
    sem_t mainProductionBarrier;
    sem_init(&mainProductionBarrier, ZERO, ZERO);

    sem_t mainConsumptionBarrier;
    sem_init(&mainConsumptionBarrier, ZERO, ZERO);
    
    

    // creating shared data structure
    broker *brokerQueue = new broker(&mutex, &availiableSlots, &availiableHumanSlots, &unconsumed, &mainProductionBarrier, &mainConsumptionBarrier, produceAutonomousDriverDelay, produceHumanDriverDelay, fastMatchingDispatchDelay, costSavingDispatchDelay, totalNumRequests);

    // creating and initializing a structure to be passed to the robo request producer
    // holds the shared data: the broker information as well as the type of producer that the thread is
    production_arg roboArgs;
    roboArgs.typeRequest = RoboDriver;
    roboArgs.brokerPtr = brokerQueue;

    // creating and initializing a structure to be passed to the human request producer
    // holds the shared data: the broker information as well as the type of producer that the thread is
    production_arg humanArgs;
    humanArgs.typeRequest = HumanDriver;
    humanArgs.brokerPtr = brokerQueue;

    // creating and initializing a structure to be passed to the cost saving algorithm consumer
    // holds the shared data: the broker information as well as the type of consumer that the thread is
    consumption_arg costSavingArgs;
    costSavingArgs.typeConsumer = CostAlgoDispatch;
    costSavingArgs.brokerPtr = brokerQueue;

    // creating and initializing a structure to be passed to the fast matching algo consumer
    // holds the shared data: the broker information as well as the type of consumer that the thread is
    consumption_arg fastMatchingArgs;
    fastMatchingArgs.typeConsumer = FastAlgoDispatch;
    fastMatchingArgs.brokerPtr = brokerQueue;

    // creating of the thread variables for each of the 2 producer and consumer threads
    pthread_t humanDriverProducer, roboDriverProducer, costSavingConsumer, fastMatchingConsumer;

    // creation of human driver producer thread
    pthread_create(&humanDriverProducer, NULL, &producer, (void *) &humanArgs);

    // creation of the robo driver producer thread
    pthread_create(&roboDriverProducer, NULL, &producer, (void *) &roboArgs);

    // creation of the cost saving consumer thread
    pthread_create(&costSavingConsumer, NULL, &consumer, (void *) &costSavingArgs);

    // creation of the fast matching consumer thread
    pthread_create(&fastMatchingConsumer, NULL, &consumer, (void *) &fastMatchingArgs);

    //blocks main till the producers are done
    sem_wait(&mainProductionBarrier);
    sem_wait(&mainProductionBarrier);

    // blocks main till the consumers have consumed all of the requests (the first consumer to finish causes both consumer threads to be killed)
    sem_wait(&mainConsumptionBarrier);
    
    // prints the production report to screen
    int *consumed[ConsumerTypeN];
    consumed[CostAlgoDispatch] = brokerQueue->costAlgoConsumed;
    consumed[FastAlgoDispatch] = brokerQueue->fastAlgoConsumed;

    // print report of the producction and consumption
    io_production_report(brokerQueue->numRequestsProducedPerType, consumed); 
}