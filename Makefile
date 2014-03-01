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
	
clean: 
	rm ${PROGS}