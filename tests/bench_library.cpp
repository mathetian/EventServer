#include <vector>
using namespace std;

#include <sys/socket.h>
#include <sys/resource.h>

#include "../include/Socket.h"
#include "../include/EventPool.h"
#include "../include/EventLoop.h"
#include "../include/MsgHandler.h"
#include "../utils/TimeStamp.h"

Atomic count, writes, fired;
int num_pipes, num_active, num_writes;

class EchoServer;

vector<Socket> pipes;
vector<EchoServer*> servers;

int thrnum = 4;
EventPool pool(thrnum);

TimeStamp start, before, end;

class EchoServer : public MSGHandler
{
public:
    EchoServer(EventLoop *loop, Socket sock, int idx) : MSGHandler(loop, sock, 1), idx(idx)
    { 
        //printf("EchoServer %d %d\n",sock.get_fd(), idx);
    } 

    void clear()
    {
        m_status = 0; m_delflag = 0;
    }

private:
    virtual void receivedMsg(STATUS status, Buffer &buf)
    {
        if(status == SUCC)
        {
            INFO << "Received: " << (string)buf << " through fd " << m_sock.get_fd();
            count += buf.length();

            if (writes > 0) 
            {
                int widx = idx + 1; if (widx >= num_pipes) widx -= num_pipes;
                INFO << "widx: " << widx ;
                if(::write(pipes[2 * widx + 1].get_fd(), "m", 1)!=1)
                {
                    INFO << strerror(errno) ;
                    assert(0);
                }
                writes--; fired++;
            }

            if(count % 20000 == 0)  WARN << count << " " << writes;
            if(count % num_pipes == 0)
            {
                TimeStamp end1 = TimeStamp::now();     
                fprintf(stdout, "%8ld %8ld\n", end1.to_usecs() - before.to_usecs(), end1.to_usecs() - start.to_usecs());
                fprintf(stdout, "writes=%d, fired=%d, recv=%d\n", (int)writes, (int)fired, (int)count);
            }
        }
        else
            INFO << "BAD" ;
    }

    virtual void onCloseSocket(int st)
    {
        if(errno != 0) 
            DEBUG << strerror(errno);
        errno = 0;    
        DEBUG << "onCloseSocket: " << st << " " << m_sock.get_fd();
    }

private:
    int idx;
};

void run_once() 
{    
    start = TimeStamp::now();

    int space = num_pipes / num_active;
    space *= 2;
    for (int i = 0; i < num_active; i++)
        ::write(pipes[i*space+1].get_fd(),"m",1);

    if(errno != 0) printf("error: %s\n",strerror(errno));
    
    fired  = num_active;
    count  = 0;
    writes = num_writes;

    before = TimeStamp::now();
    WARN << "Begin runforever" ;
    pool.runforever();
    WARN << "End runforever" ;
    end = TimeStamp::now();

    fprintf(stdout, "%8ld %8ld\n", end.to_usecs() - before.to_usecs(), end.to_usecs() - start.to_usecs());
}

int setlimit(int num_pipes)
{
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = num_pipes * 2 + 50;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
        fprintf(stderr, "setrlimit error: %s", strerror(errno));
        return 1;
    }
}

int main(int argc, char* argv[]) 
{
    num_pipes  = 150000;
    num_active = 1000;
    num_writes = num_pipes*25;

   // setlimit(num_pipes);

    pipes = vector<Socket>(num_pipes*2,-1);

    for (int i = 0; i < num_pipes; i++) 
    {
        pair<Socket, Socket> ss = Socket::pipe();
        pipes[i*2]=ss.first;pipes[i*2+1]=ss.second;
    }

    for (int i = 0; i < num_pipes; i++)
        servers.push_back(new EchoServer(pool.getRandomLoop(), pipes[i*2],i));

    for (int i = 0; i < 1; i++) 
    {
        run_once();
        fprintf(stdout, "writes=%d, fired=%d, recv=%d\n", (int)writes, (int)fired, (int)count);
        for(int i=0;i<num_pipes;i++) servers[i]->clear();
    }
}
