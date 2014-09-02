/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *      _____       ___   _____   _____    _   _____   _               *
 *     /  ___|     /   | |  _  \ |  _  \  | | | ____| | |              *
 *     | |        / /| | | |_| | | |_| |  | | | |__   | |              *
 *     | |  _    / / | | |  _  { |  _  /  | | |  __|  | |              *
 *     | |_| |  / /  | | | |_| | | | \ \  | | | |___  | |___           *
 *     \_____/ /_/   |_| |_____/ |_|  \_\ |_| |_____| |_____|          *
 *                                                                     *
 *     gabriel is an angel from the Holy Bible, this engine is named   *
 *   gabriel, means bringing people good news. the goal of gabriel     *
 *   server engine is to help people to develop various online games,  *
 *   welcome you to join in.                                           *
 *                                                                     *
 *   @author: lichuan                                                  *
 *   @qq: 308831759                                                    *
 *   @email: 308831759@qq.com                                          *
 *   @site: www.lichuan.me                                             *
 *   @github: https://github.com/lichuan/gabriel                       *
 *   @date: 2013-11-29 09:00:07                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__CONNECTION
#define GABRIEL__BASE__CONNECTION

#include "ace/Svc_Handler.h"
#include "ace/SOCK_Stream.h"
#include "google/protobuf/message.h"
#include "gabriel/base/ref.hpp"
#include "gabriel/base/common.hpp"
#include "gabriel/base/entity.hpp"

namespace gabriel {
namespace base {
    
class Server;
    
class Connection : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>, public Entity<>, public Ref<ACE_SYNCH_MUTEX>
{
    struct Message
    {
        uint32 m_msg_type;
        uint32 m_msg_id;
        ACE_Message_Block *m_msg_block;

        void release()
        {
            m_msg_block->release();
            delete this;            
        }
    };
    
    typedef ACE_Svc_Handler Super;    
public:
    Connection();
    virtual ~Connection();    
    virtual int open(void *acceptor_or_connector);
    uint32 state() const;
    void state(uint32 _state);
    bool connected() const;
    bool lost_connection() const;
    void send(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg);
    void do_main();
    const ACE_INET_Addr& inet_addr() const;
    uint16 port() const;
    const char* ip_addr() const;
    const char* host_name() const;
    void shutdown();
    
protected:
    Server *m_holder;
    
private:
    virtual int handle_input(ACE_HANDLE hd = ACE_INVALID_HANDLE);
    virtual int handle_output(ACE_HANDLE hd = ACE_INVALID_HANDLE);
    virtual void dispatch(uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;    
    virtual void on_shutdown() = 0;
    void do_main_i();    
    uint32 m_state;
    uint32 m_last_msg_length;
    ACE_INET_Addr m_addr;
    ACE_Message_Queue<ACE_MT_SYNCH> m_send_queue;
    ACE_Message_Queue_Ex<Message, ACE_MT_SYNCH> m_dispatch_queue;
    bool m_write_disable;    
};
    
}
}

#endif
