/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is         *
 *   named gabriel, standing for bring good news to people. the        *
 *   goal of gabriel server engine is to help people to develop        *
 *   various online games, welcome you to join in.                     *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2013-11-28 21:31:28                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include "ace/SOCK_Connector.h"
#include "ace/Svc_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Connector.h"
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
        return this->reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);        
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
        const char *buf = "hello serverrrrrrrrrr";        
        this->peer().send(buf, strlen(buf) + 1);
        ACE_OS::sleep(1);
        

        return 0;
    }
};

int ACE_TMAIN (int, ACE_TCHAR *[])
{
    ACE_INET_Addr port_to_connect (20000);
    ACE_Connector<TCPTask, ACE_SOCK_CONNECTOR> connector;
    TCPTask task;
    TCPTask *ptask = &task;
    
    if (connector.connect (ptask, port_to_connect) == -1)
        ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                           ACE_TEXT ("connect")), 1);
    ACE_Reactor::instance()->run_event_loop();
}
