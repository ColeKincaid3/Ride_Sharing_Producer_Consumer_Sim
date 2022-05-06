// Cole Kincaid 822602112

#include "broker.h"

#ifndef CONSUMPTION_ARG_H
#define CONSUMPTION_ARG_H

// struct that is used to pass multiple arguements to a consumer thread
struct consumption_arg {
    Consumers typeConsumer;
    broker* brokerPtr; 
};

#endif

// consumer function (used by thread)
void* consumer( void* ptr );