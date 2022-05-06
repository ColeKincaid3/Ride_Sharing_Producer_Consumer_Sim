// Cole Kincaid 822602112

#include "broker.h"

#ifndef PRODUCTION_ARG_H
#define PRODUCTION_ARG_H

// struct that is used to pass multiple arguements to a producer thread
struct production_arg {
    Requests typeRequest;
    broker *brokerPtr;
};

#endif

// producer function (used by thread)
void* producer( void* ptr);