#include <iostream>
#include "ace/Svc_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Acceptor.h"
#include "ace/Dev_Poll_Reactor.h"

using namespace std;

class TCPTask : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>
{
    typedef ACE_Svc_Handler super;    
public:
    TCPTask()
    {
        cout << "new taskkkkkkkkk" << endl;
    }

    virtual int open(void*)
    {
        cout << "open.........." << endl;       
        ACE_Time_Value iter_delay (2);   // Two seconds
        if (super::open () == -1)
            return -1;
        return this->reactor ()->schedule_timer
            (this, 0, ACE_Time_Value::zero, iter_delay);
    }
    
    virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE)
    {
        char buf[64];
        ssize_t recv_cnt = this->peer ().recv (buf, sizeof (buf) - 1);

        if(recv_cnt < 0)
        {
            cout << "<0000000000000000000000" << endl;

            if(errno != EWOULDBLOCK)
            {
                cout << "not ewouldblockkkkkkkkkkkk" << endl;
            }
            else
            {
                cout << "yes ewouldblockkkkkkkkkkkkkkkk" << endl;
            }            
        }
        
        if (recv_cnt > 0)
        {
            cout << "recv: " << buf << endl;            
        
            return 0;
        }

        if (recv_cnt == 0)
        {
            cout << "closeeeeeeeeeeeeeeeeeeeee" << endl;            

            return -1;
        }
        
        return 0;
    }
    
    virtual int handle_output (ACE_HANDLE fd = ACE_INVALID_HANDLE)
    {
        ACE_Message_Block *mb = 0;
        ACE_NEW_RETURN (mb, ACE_Message_Block (128), -1);
        int nbytes = ACE_OS::sprintf
            (mb->wr_ptr (), "server outputttttttttttttt");        
        ACE_ASSERT (nbytes > 0);
        mb->wr_ptr (static_cast<size_t> (nbytes));
        this->putq (mb);

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
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(10000), true), true);
    ACE_Acceptor<TCPTask, ACE_SOCK_ACCEPTOR> acceptor;
    acceptor.open(ACE_INET_Addr(20000));    
    ACE_Reactor::instance()->run_event_loop();
}
