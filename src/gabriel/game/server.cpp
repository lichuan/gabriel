#include <iostream>
#include "ace/Svc_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Signal.h"
#include "ace/SOCK_Stream.h"
#include "ace/Acceptor.h"
#include "ace/Dev_Poll_Reactor.h"
#include "gabriel/base/astar.hpp"

using namespace std;

class TCPTask : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
    typedef ACE_Svc_Handler super;    
public:
    TCPTask()
    {
        cout << "new taskkkkkkkkk" << endl;
    }


    virtual ~TCPTask()
    {
        ACE_INET_Addr addr;
        peer().get_remote_addr(addr);        
        cout << "`~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << addr.get_host_addr() << " " << addr.get_ip_address() << endl;        
    }
    
    virtual int open(void*)
    {
        cout << "open.........." << endl;       
        ACE_Time_Value iter_delay (2);   // Two seconds
        if (super::open () == -1)
            return -1;
        ACE_Time_Value tv(1);        
        ACE_Reactor::instance()->schedule_timer(this, 0, ACE_Time_Value::zero, tv);        
        return this->reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);        
    }
    
    virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE)
    {
        char buf[64];
        ssize_t recv_cnt = this->peer ().recv (buf, sizeof (buf) - 1);

        if(recv_cnt < 0)
        {
            cout << "<0000000000000000000000" << endl;

            if(errno != EWOULDBLOCK && errno != EAGAIN)
            {
                cout << "errno: " << errno << "  " << EWOULDBLOCK << " " << EAGAIN << endl;
            }
            else
            {
                cout << "yes ewouldblockkkkkkkkkkkkkkkk" << endl;
            }            
        }
        
        if (recv_cnt > 0)
        {
            cout << "recv: " << buf << endl;
            ACE_INET_Addr addr;
            peer().get_remote_addr(addr);
            cout << "recv: ip: " << addr.get_host_addr() << " " << addr.get_ip_address() <<endl;

        
            return 0;
        }

        if (recv_cnt == 0)
        {
            ACE_INET_Addr addr;
            peer().get_remote_addr(addr);
            cout << "close: ip: " << addr.get_host_addr() << " " << addr.get_ip_address() <<endl;
            

            cout << "closeeeeeeeeeeeeeeeeeeeee" << endl;            

            return -1;
        }
        
        return 0;
    }
    
    virtual int handle_output (ACE_HANDLE fd = ACE_INVALID_HANDLE)
    {
        const char *buf = "client hellor";
        int ret = peer().send(buf, sizeof(buf) + 1);
        cout << "outttttttttttttttttttttt: ret = " << ret << endl;
        reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);

        return 0;     
    }

    virtual int handle_timeout (const ACE_Time_Value &current_time, const void *act = 0)
    {
        cout << "time outtttttttttttttttt" << endl;

        return 0;        
    }    
};

int ACE_TMAIN (int, ACE_TCHAR *[])
{
    gabriel::base::ASTAR_1000::instance();    
    ACE_Sig_Action no_sigpipe ((ACE_SignalHandler) SIG_IGN);
    ACE_Sig_Action original_action;
    no_sigpipe.register_action (SIGPIPE, &original_action);
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(10000), true), true);
    ACE_Acceptor<TCPTask, ACE_SOCK_ACCEPTOR> acceptor;
    acceptor.open(ACE_INET_Addr(20000));    
    ACE_Reactor::instance()->run_event_loop();
}
