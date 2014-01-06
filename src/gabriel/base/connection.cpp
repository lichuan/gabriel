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
 *   @date: 2013-11-29 16:38:55                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gabriel/base/connection.hpp"

namespace gabriel {
namespace base {

Connection::Connection() :
    m_recv_queue(MSG_QUEUE_HWM, MSG_QUEUE_LWM),
    m_send_queue_1(MSG_QUEUE_HWM, MSG_QUEUE_LWM),
    m_send_queue_2(MSG_QUEUE_HWM, MSG_QUEUE_LWM)
{
    water_marks(ACE_IO_Cntl_Msg::SET_HWM, MSG_QUEUE_HWM);
    water_marks(ACE_IO_Cntl_Msg::SET_LWM, MSG_QUEUE_LWM);
    m_holder = NULL;
    m_state = INVALID_STATE;
    m_cancel_write = true;
}

Connection::~Connection()
{
}

CONNECTION_STATE Connection::state() const
{
    return m_state;    
}

void Connection::state(CONNECTION_STATE _state)
{
    m_state = _state;
}

bool Connection::connected() const
{
    return m_state == CONNECTED_STATE;
}

void Connection::send_msg(uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    Message *msg = new Message;
    msg->m_msg_type = msg_type;
    msg->m_msg_id = msg_id;
    ACE_Message_Block *msg_data = new ACE_Message_Block(size);
    msg_data->copy(static_cast<char*>(data), size);
    msg->m_msg_data = msg_data;    
    m_send_queue_1.enqueue_tail(msg);
}
    
void Connection::encode()
{
    if(m_send_queue_1.is_empty())
    {
        return;
    }
    
    Message *msg;
    m_send_queue_1.dequeue(msg);
    const uint32 encoded_msg_size = 3 * sizeof(uint32) + msg->m_msg_data->size();    
    ACE_Message_Block *encoded_msg = new ACE_Message_Block(encoded_msg_size);    
    uint32 *uint32_msg = reinterpret_cast<uint32*>(encoded_msg);
    uint32_msg[0] = ACE_HTONL(encoded_msg_size);    
    uint32_msg[1] = ACE_HTONL(msg->m_msg_type);
    uint32_msg[2] = ACE_HTONL(msg->m_msg_id);
    encoded_msg->wr_ptr(3 * sizeof(uint32));
    encoded_msg->copy(msg->m_msg_data->base(), msg->m_msg_data->size());
    m_send_queue_2.enqueue_tail(encoded_msg);
    msg->m_msg_data->release();
    delete msg;
    
    if(m_cancel_write)
    {
        reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);
        m_cancel_write = false;
    }
}
    
void Connection::decode()
{
    ACE_Message_Block *msg_block;

    if(msg_queue()->is_empty())
    {
        return;
    }

    getq(msg_block);    
}
    
int Connection::open(void *acceptor_or_connector)
{
    if(Super::open() < 0)
    {
        return -1;
    }
    
    if(reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK) < 0)
    {
        return -1;
    }

    state(CONNECTED_STATE);

    return 0;
}

int Connection::handle_input(ACE_HANDLE hd)
{
    ACE_Message_Block *msg_block = new ACE_Message_Block(RECV_REQUEST_SIZE);
    int32 recv_size = peer().recv(msg_block->base(), RECV_REQUEST_SIZE);

    if(recv_size > 0)
    {
        msg_block->wr_ptr(recv_size);
        putq(msg_block);
    }
    else
    {
        msg_block->release();
        shutdown();
    }

    return 0;
}

void Connection::shutdown()
{
    ACE_Svc_Handler::shutdown();
    state(CLOSED_STATE);
    reactor(0);
    recycler(0, 0);
}

int Connection::handle_output(ACE_HANDLE hd)
{
    if(m_send_queue_2.is_empty())
    {
        reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);
        m_cancel_write = true;

        return 0;        
    }
    
    ACE_Message_Block *msg_block;
    m_send_queue_2.dequeue(msg_block);    
    int32 send_size = peer().send(msg_block, msg_block->length());
    
    if(send_size > 0)
    {
        msg_block->rd_ptr(send_size);

        if(msg_block->length() > 0)
        {
            m_send_queue_2.enqueue_head(msg_block);
        }
        else
        {
            msg_block->release();
        }
    }
    else
    {
        msg_block->release();
        shutdown();
    }
    
    return 0;
}

}
}
