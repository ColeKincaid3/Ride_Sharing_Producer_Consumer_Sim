CXX=g++

CXXFLAGS=-std=c++11 -g

rideshare : main.o producer.o consumer.o broker.o rideRequest.o io.o
	$(CXX) $(CXXFLAGS)  -o rideshare $^ -lpthread -lrt

main.o : main.cpp

producer.o : producer.h producer.cpp broker.h rideRequest.h ridesharing.h

consumer.o : consumer.h consumer.cpp broker.h rideRequest.h ridesharing.h

broker.o : broker.h broker.cpp ridesharing.h rideRequest.h

rideRequest.o : rideRequest.h rideRequest.cpp 

io.o : io.h io.cpp

clean :
	rm *.o