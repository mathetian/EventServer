include Make.defines

CXX     = g++
AR	    = ar
LIBMISC	= libsealedserver.a
RANLIB  = ranlib
HEADER  = -I. -I./include -I./utils
CXXFLAGS = -g -O0
PTHRFLAGS = -lpthread -pthread

SOURCES = utils/*.cpp core/*.cpp
LDLIBS  = -L. -lsealedserver

tests = test_buffer test_c test_callback test_log test_slice test_squeue test_tester test_thread

PROGS = server client bench_library ${tests}

lib: clean prepare compile
	${AR} rv ${LIBMISC} *.o
	${RANLIB} ${LIBMISC}
	rm *.o

compile:
	${CXX} ${CXXFLAGS} ${HEADER} -lpthread -c ${SOURCES} 

echo: prepare server client
	
server: tests/echo_server.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

client: tests/echo_client.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

http_server: tests/http_server.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

http_client: tests/http_client.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

bench_library: prepare tests/bench_library.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

tests: prepare ${tests}
	
test_buffer: tests/test_buffer.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_c: tests/test_c.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_callback: tests/test_callback.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_log: tests/test_log.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_parser: tests/test_parser.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin
	
test_slice: tests/test_slice.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_squeue: tests/test_squeue.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_status: tests/test_status.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_tester: tests/test_tester.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin

test_thread: tests/test_thread.cpp
	$(CXX) ${CXXFLAGS} ${HEADER} ${PTHRFLAGS} $^ -o $@ ${LDLIBS}
	mv $@ bin
	
prepare:
	-mkdir bin

clean: 
	-rm -rf bin ${CLEAN}