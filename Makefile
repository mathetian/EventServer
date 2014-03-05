CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./include -I. -I./utils
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = cache/*.cpp core/*.cpp helpers/*.cpp dbimpl/*/*.cpp utils/*.cpp

PROGS = server client

all: ${PROGS}
	
server: tests/echo_server.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 

client: tests/echo_client.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 

test_squeue: tests/test_squeue.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 

clean: 
	rm ${PROGS}