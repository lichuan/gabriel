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
    m_cancel_write = true;
    m_last_decode_msg_length = 0;
    m_state = CONNECTION_STATE::INVALID;
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

void Connection::send(uint32 msg_type, uint32 msg_id, void *data, uint32 size)
{
    if(!connected())
    {
        return;
    }
    
    const uint32 msg_size = sizeof(uint32) * 3 + size;    
    ACE_Message_Block *msg_block = new ACE_Message_Block(msg_size);
    uint32 *uint32_msg = reinterpret_cast<uint32*>(msg_block->base());
    uint32_msg[0] = ACE_HTONL(msg_size);
    uint32_msg[1] = ACE_HTONL(msg_type);
    uint32_msg[2] = ACE_HTONL(msg_id);
    msg_block->wr_ptr(sizeof(uint32) * 3);
    msg_block->copy(static_cast<char*>(data), size);
    m_send_queue_1.enqueue_tail(msg_block);    
}

void Connection::send(uint32 msg_type, uint32 msg_id, google::protobuf::Message &msg)
{
    char buf[gabriel::base::MSG_SERIALIZE_BUF_HWM];
    const uint32 byte_size = msg.ByteSize();
    msg.SerializeToArray(buf, byte_size);
    send(msg_type, msg_id, buf, byte_size);
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
        dispatch();
    }
}

void Connection::dispatch()
{
    if(m_recv_queue.is_empty())
    {
        return;
    }

    ACE_Message_Block *msg_block;
    m_recv_queue.dequeue(msg_block);
    uint32 *uint32_msg = reinterpret_cast<uint32*>(msg_block->base());
    const uint32 msg_type = ACE_NTOHL(uint32_msg[0]);
    const uint32 msg_id = ACE_NTOHL(uint32_msg[1]);
    msg_block->rd_ptr(2 * sizeof(uint32));    
    dispatch(msg_type, msg_id, msg_block->rd_ptr(), msg_block->length());
    msg_block->release();
}
    
void Connection::encode()
{
    if(!connected())
    {
        return;
    }
    
    if(m_send_queue_1.is_empty())
    {
        return;
    }

    ACE_Message_Block *msg_block;
    m_send_queue_1.dequeue(msg_block);
    m_send_queue_2.enqueue_tail(msg_block);
    
    if(m_cancel_write)
    {
        reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);
        m_cancel_write = false;
    }
}

uint32 Connection::decode_msg_length()
{
    if(m_last_decode_msg_length != 0)
    {
        return m_last_decode_msg_length;
    }
    
    if(msg_queue()->message_length() < sizeof(uint32))
    {
        return 0;
    }
    
    ACE_Message_Block *cur_msg_block = NULL;
    uint32 remain_bytes = sizeof(uint32);
    char *cur_ptr = reinterpret_cast<char*>(&m_last_decode_msg_length);
    
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

    m_last_decode_msg_length = ACE_NTOHL(m_last_decode_msg_length);

    return m_last_decode_msg_length;
}
    
void Connection::decode()
{
    if(!connected())
    {
        return;
    }
    
    if(decode_msg_length() == 0)
    {
        return;
    }

    if(m_last_decode_msg_length <= 3 * sizeof(uint32))
    {
        return;
    }

    uint32 remain_bytes = m_last_decode_msg_length - sizeof(uint32);
    
    if(msg_queue()->message_length() < remain_bytes)
    {
        return;
    }

    ACE_Message_Block *msg_block = new ACE_Message_Block(remain_bytes);
    ACE_Message_Block *cur_msg_block = NULL;
    
    for(;;)
    {
        getq(cur_msg_block);

        if(cur_msg_block->length() >= remain_bytes)
        {
            msg_block->copy(cur_msg_block->rd_ptr(), remain_bytes);
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

        msg_block->copy(cur_msg_block->rd_ptr(), cur_msg_block->length());
        remain_bytes -= cur_msg_block->length();
        cur_msg_block->release();
    }

    m_recv_queue.enqueue_tail(msg_block);
    m_last_decode_msg_length = 0;    
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
    int32 send_size = peer().send(msg_block->rd_ptr(), msg_block->length());

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
