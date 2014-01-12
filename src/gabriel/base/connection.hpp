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
#include "gabriel/base/common.hpp"
#include "gabriel/base/entity.hpp"

namespace gabriel {
namespace base {
    
class Server;
    
class Connection : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH>, public Entity<>
{
    typedef ACE_Svc_Handler Super;    
public:
    Connection();
    virtual ~Connection();    
    virtual int open(void *acceptor_or_connector);
    virtual int handle_input(ACE_HANDLE hd = ACE_INVALID_HANDLE);
    virtual int handle_output(ACE_HANDLE hd = ACE_INVALID_HANDLE);
    uint32 state() const;
    void state(uint32 _state);
    bool connected() const;
    void decode();
    void encode();
    void shutdown();
    void dispatch();    
    void send(uint32 msg_type, uint32 msg_id, void *data, uint32 size);
    void do_main();
    
protected:
    Server *m_holder;

private:
    virtual void dispatch(uint32 msg_type, uint32 msg_id, void *data, uint32 size) = 0;
    virtual void on_shutdown() = 0;    
    uint32 decode_msg_length();
    ACE_Message_Queue<ACE_MT_SYNCH> m_recv_queue;
    ACE_Message_Queue<ACE_MT_SYNCH> m_send_queue_1;
    ACE_Message_Queue<ACE_MT_SYNCH> m_send_queue_2;
    uint32 m_state;
    bool m_cancel_write;
    uint32 m_last_decode_msg_length;
};
    
}
}

#endif
