CXX     = g++
AR	    = ar
LIBMISC	= libcustomserver.a
RANLIB  = ranlib
HEADER  = -I./include -I. -I./utils
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = cache/*.cpp core/*.cpp helpers/*.cpp dbimpl/*/*.cpp utils/*.cpp

tests = test_squeue test_buffer test_callback test_log test_slice test_tostring

PROGS = server client ${tests}

all: clean prepare ${PROGS}
	
server: tests/echo_server.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

client: tests/echo_client.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_squeue: tests/test_squeue.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_buffer: tests/test_buffer.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_callback: tests/test_callback.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_log: tests/test_log.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_slice: tests/test_slice.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

test_tostring: tests/test_tostring.cpp utils/Thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ 
	mv $@ bin

prepare:
	mkdir bin

clean: 
	-rm -rf bin