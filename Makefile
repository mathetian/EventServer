CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./include -I. -I./utils
CXXFLAGS = -g -O0

SOURCES = cache/*.cpp core/*.cpp helpers/*.cpp dbimpl/*/*.cpp utils/*.cpp

PROGS = server client

all: ${PROGS}
	
server: tests/echo_server.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} $^ -o $@ 

client: tests/echo_client.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} $^ -o $@ 

test_squeue: tests/test_squeue.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} -lpthread -pthread $^ -o $@ 

clean: 
	rm ${PROGS}