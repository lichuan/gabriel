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

Connection::Connection()
{
    water_marks(ACE_IO_Cntl_Msg::SET_HWM, MSG_QUEUE_HWM);
    water_marks(ACE_IO_Cntl_Msg::SET_LWM, MSG_QUEUE_LWM);
    m_send_queue.high_water_mark(MSG_QUEUE_HWM);    
    m_send_queue.low_water_mark(MSG_QUEUE_LWM);
    m_dispatch_queue.high_water_mark(MSG_QUEUE_HWM);
    m_dispatch_queue.low_water_mark(MSG_QUEUE_LWM);
    m_holder = NULL;
    m_last_msg_length = 0;
    m_state = CONNECTION_STATE::INVALID;
    m_write_disable = true;    
}
    
Connection::~Connection()
{
}

uint32 Connection::state() const
{
    return m_state;    
}

void Connection::state(uint32 _state)
{
    m_state = _state;
}

bool Connection::connected() const
{
    return m_state == CONNECTION_STATE::CONNECTED;
}
    
void Connection::send(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg)
{
    const uint32 byte_size = msg.ByteSize();
    ACE_Message_Block *msg_block = new ACE_Message_Block(sizeof(uint32) * 3 + byte_size);    
    msg.SerializeToArray(msg_block->base() + sizeof(uint32) * 3, byte_size);
    const uint32 msg_size = sizeof(uint32) * 2 + byte_size;
    uint32 *uint32_msg = reinterpret_cast<uint32*>(msg_block->base());    
    uint32_msg[0] = ACE_HTONL(msg_size);
    uint32_msg[1] = ACE_HTONL(msg_type);
    uint32_msg[2] = ACE_HTONL(msg_id);
    msg_block->wr_ptr(msg_block->size());    
    m_send_queue.enqueue_tail(msg_block);

    if(m_write_disable)
    {
        reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);
        m_write_disable = false;        
    }
}
    
void Connection::do_main()
{
    if(state() == CONNECTION_STATE::SHUTTING_DOWN)
    {
        state(CONNECTION_STATE::SHUTTING_DOWN_1);
        on_shutdown();
    }
    else if(connected())
    {
        do_main_i();
    }
}

void Connection::do_main_i()
{
    if(m_dispatch_queue.is_empty())
    {
        return;
    }
    
    Message *msg;
    m_dispatch_queue.dequeue(msg);
    dispatch(msg->m_msg_type, msg->m_msg_id, msg->m_msg_block->rd_ptr(), msg->m_msg_block->length());
    delete msg;
}
    
int Connection::open(void *acceptor_or_connector)
{
    if(Super::open() < 0)
    {
        return -1;
    }
    
    state(CONNECTION_STATE::CONNECTED);
    peer().get_remote_addr(m_addr);
    
    return 0;
}

const ACE_INET_Addr& Connection::inet_addr() const
{
    return m_addr;
}

uint16 Connection::port() const
{
    return m_addr.get_port_number();
}

const char* Connection::ip_addr() const
{
    return m_addr.get_host_addr();
}

const char* Connection::host_name() const
{
    return m_addr.get_host_name();
}

int Connection::handle_input(ACE_HANDLE hd)
{
    ACE_Message_Block *recv_msg_block = new ACE_Message_Block(RECV_REQUEST_SIZE);
    int32 recv_size = peer().recv(recv_msg_block->base(), RECV_REQUEST_SIZE);

    if(recv_size <= 0)
    {
        recv_msg_block->release();
        shutdown();

        return 0;        
    }    

    recv_msg_block->wr_ptr(recv_size);
    putq(recv_msg_block);
        
    if(m_last_msg_length == 0)
    {
        if(msg_queue()->message_length() < sizeof(uint32))
        {
            return 0;
        }
        
        ACE_Message_Block *cur_msg_block = NULL;
        uint32 remain_bytes = sizeof(uint32);
        char *cur_ptr = reinterpret_cast<char*>(&m_last_msg_length);
        
        for(;;)
        {
            getq(cur_msg_block);
            
            if(cur_msg_block->length() >= remain_bytes)
            {
                ACE_OS::memcpy(cur_ptr, cur_msg_block->rd_ptr(), remain_bytes);
                cur_msg_block->rd_ptr(remain_bytes);
            
                if(cur_msg_block->length() > 0)
                {
                    ungetq(cur_msg_block);
                }
                else
                {
                    cur_msg_block->release();
                }

                break;
            }

            ACE_OS::memcpy(cur_ptr, cur_msg_block->rd_ptr(), cur_msg_block->length());
            remain_bytes -= cur_msg_block->length();
            cur_ptr += cur_msg_block->length();
            cur_msg_block->release();
        }

        m_last_msg_length = ACE_NTOHL(m_last_msg_length);
    }

    if(m_last_msg_length < 2 * sizeof(uint32))
    {
        return 0;
    }
    
    uint32 remain_bytes = m_last_msg_length;
    
    if(msg_queue()->message_length() < remain_bytes)
    {
        return 0;
    }
        
    ACE_Message_Block *msg_block = new ACE_Message_Block(remain_bytes);
    Message *msg = new Message;        
    msg->m_msg_block = msg_block;        
    char *cur_ptr = msg_block->base();        
    ACE_Message_Block *cur_msg_block = NULL;
    const uint32 remain_bytes_bak = remain_bytes;
    
    for(;;)
    {
        getq(cur_msg_block);

        if(cur_msg_block->length() >= remain_bytes)
        {
            ACE_OS::memcpy(cur_ptr, cur_msg_block->rd_ptr(), remain_bytes);
            cur_msg_block->rd_ptr(remain_bytes);
            
            if(cur_msg_block->length() > 0)
            {
                ungetq(cur_msg_block);
            }
            else
            {
                cur_msg_block->release();
            }

            break;
        }

        ACE_OS::memcpy(cur_ptr, cur_msg_block->rd_ptr(), cur_msg_block->length());
        remain_bytes -= cur_msg_block->length();
        cur_ptr += cur_msg_block->length();
        cur_msg_block->release();
    }
    
    m_last_msg_length = 0;
    uint32 *uint32_msg = reinterpret_cast<uint32*>(msg_block->base());        
    msg->m_msg_type = ACE_NTOHL(uint32_msg[0]);
    msg->m_msg_id = ACE_NTOHL(uint32_msg[1]);
    msg_block->rd_ptr(sizeof(uint32) * 2);
    msg_block->wr_ptr(msg_block->size());
    m_dispatch_queue.enqueue_tail(msg);

    return 0;
}
    
int Connection::handle_output(ACE_HANDLE hd)
{
    if(m_send_queue.is_empty())
    {
        reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);
        m_write_disable = true;

        return 0;
    }
    
    ACE_Message_Block *msg_block;
    m_send_queue.dequeue(msg_block);
    int32 send_size = peer().send(msg_block->rd_ptr(), msg_block->length());
        
    if(send_size == msg_block->length())
    {
        msg_block->release();

        return 0;        
    }
    
    if(send_size > 0)
    {
        msg_block->rd_ptr(send_size);
        m_send_queue.enqueue_head(msg_block);
    }
    else if(send_size == 0 || errno != EWOULDBLOCK && errno != EAGAIN)
    {
        msg_block->release();
        shutdown();
    }
    else
    {
        m_send_queue.enqueue_head(msg_block);
    }
    
    return 0;
}

bool Connection::lost_connection() const
{
    if(state() == CONNECTION_STATE::INVALID)
    {
        return false;
    }

    return !connected();
}

void Connection::shutdown()
{
    ACE_Svc_Handler::shutdown();
    state(CONNECTION_STATE::SHUTTING_DOWN);
    reactor(0);
    recycler(0, 0);
}

}
}
