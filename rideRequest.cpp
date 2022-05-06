// Cole Kincaid 822602112

#include "rideRequest.h"

/**
 * rideRequest object Constructor
 * 
 * Input: int rType
 * 
 * initializes the requestType of the rideRequest object
 */
rideRequest::rideRequest(int rType){
    requestType = (Requests) rType;
}