EventServer
======

EventServer is a light-weight asynchronous network programming framework and supports I/O events. Also, we provide support for time event and signal event. 

Based on this framework, we provide a simple encapsulation for HTTP protocol. 
I have used this framework to build the [CComet](https://github.com/mathetian/CComet).

Our work is mainly inspired by [dirlt](http://dirlt.com/‎), [hpserver](http://code.google.com/p/hpserver/), [<span color="red">nmstl</span>](http://nmstl.sourceforge.net/) and [libevent](http://libevent.org). Especially, I adopt some concepts from [yohub](https://github.com/kedebug/yohub). Thanks again for OSC (Open Source Community).

## Features
1. Native Support for Multiple-threading Selector.
2. Mutliple Demux Threads and Each Thread Can Proceed Tasks Individual.
3. Reactor Pattern for Event-driven.
4. Support Pseduo-Asynchronous I/O.
5. Support IO/Timer/Signal Events.
6. Provide a Simple Garbage Collection. 
7. Use SocketPair as the Method of Transforming Information Between Threads

## Benchmark

#### 1. QPS

In this part, I will give out the comparison between our framework and libevent.

The benchmark is very simple: first a number of socket pairs is created, then event watchers for those pairs are installed and then a (smaller) number of "active clients" send and receive data on a subset of those sockets.

The benchmark program used was test/bench_library.c, taken from the libevent distribution, modified to using the api provided by EventServer.

```
EventServer:    version 1.0
Date        :   Sunday July 13 16:14:09 2014
CPU         :   4 * Intel(R) Core(TM) i5-3317U CPU @ 1.70GHz
CPUCache    :   3072 KB
G++         :   4.8.1
libevent    :   2.0
Demux       :   EPoll
```

Each run of the benchmark program consists of two iterations outputting the total time per iteration as well as the time used in handling the requests only. The program will be run in different atomsphere, especially, use different number of threads.

![100](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/bench100.png)
![1000](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/bench1000.png)

In the above graph, the X of EventServer-X means the number of threads created by bench_library.

The two graphs above show the overall time spent for different atomsphere. The top row represents 100 active clients (clients doing I/O), the bottom row uses 1000. All graphs have a logarithmic fd-axis to sensibly display the large range of file descriptor numbers of 10000 to 200000 (in reality, its actually socket pairs, thus there are actually twice the number of file descriptors in the process).

### __Discussion__
The total time of our framework will be better than libevent when we use at least two threads. In my computer, we can achieve a maximum 40% performance boost.

### 2. Number of Connections can be support
For a server, we usually consider about the qps. However, sometimes, we would care about the maximum connections can be supported. That application mainly is used in push server, which will be discussed in [CComet](https://github.com/mathetian/CComet). As we have said, we provide a encapsulation for Http Protocal. Therefore, in that part, we test the maximum number of connections can be supported.

For Server,
``` 
                VRT             RES
 10, 000 :    
 50, 000 :    
100, 000 :    
200, 000 :    
```

For Client,
``` 
                VRT             RES
 10, 000 :    
 50, 000 :    
100, 000 :    
200, 000 :    
```

## Usage

### 1. __EventServer__
```C++
class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock) : MSGHandler(loop, sock) { }
    virtual ~EchoServer() { }
private:
    /// Invoked when a message is received.
    virtual void received(STATUS status, Buffer &receivedBuff)
    { }
    // Invoked when a msg has been sent
    virtual void sent(STATUS status, int len, int targetLen)
    { }
    // Invoked when the socket has been closed
    virtual void closed(ClsMtd st)
    { }
};

class EchoClient : public MSGHandler
{
public:
    EchoClient(EventLoop *loop, Socket sock) : MSGHandler(loop, sock)
    { }
    virtual ~EchoClient() { }
protected:
    /// Invoked when a connection we try either succeeds or fails.
    virtual void connected()
    { }
    virtual void received(STATUS status, Buffer &receivedBuff)
    { }
    /// Invoked when a msg has been sent
    virtual void sent(STATUS status, int len, int targetLen)
    { }
    /// Invoke when the socket has been closed
    virtual void closed(ClsMtd st)
    { }
};
```

### 2. __HttpModule__

Although we provde a simple package for event driven server, it is still hard to use it. Therefore, we provide httpmoudle for it. We show the simplest form below.

```C++
    void sign_handler(HttpRequest *req, HttpResponse *rep, void *arg) { }
    
    HttpServer server(Port, PortNum);

    server.add("/sign", sign_handler, NULL);
    server.add("/pub",  pub_handler, NULL);
    server.add("/sub",  sub_handler, NULL);
    server.error(error_handler, NULL);

    server.start();
```

```C++
    void get(HttpRequest *req, void *arg) { }
    void error(HttpRequest *req, void *arg) { }
    
    HttpClient client;

    /// bbs.sjtu.edu.cn
    client.request("202.120.58.161", get, error, NULL);

    /// yulongti.info/?p=2761
    client.request("128.199.204.82/?p=2761", get, error, NULL);
```
## Design Principle
每个程序员都有造轮子的梦想，这就是这一个框架的由来。

## EventServer UML Graph

![EventServer UML Graph](https://raw.githubusercontent.com/mathewes/blog-dot-file/master/SealedServer.png)


## Todo List

1. Add support for unlimited file descriptor
2. Optimization for Thread affinity
3. Truly Asynchronous I/O