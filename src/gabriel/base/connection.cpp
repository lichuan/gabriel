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
    m_recv_msg_queue(MSG_QUEUE_HWM, MSG_QUEUE_LWM),
    m_send_queue_1(MSG_QUEUE_HWM, MSG_QUEUE_LWM),
    m_send_queue_2(MSG_QUEUE_HWM, MSG_QUEUE_LWM)
{
    water_marks(ACE_IO_Cntl_Msg::SET_HWM, MSG_QUEUE_HWM);
    water_marks(ACE_IO_Cntl_Msg::SET_LWM, MSG_QUEUE_LWM);
    m_holder = NULL;    
}

Connection::~Connection()
{
}

int Connection::open(void *acceptor_or_connector)
{
    if(Super::open() == -1)
    {
        return -1;
    }
    
    return reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);
}

int Connection::handle_input(ACE_HANDLE hd)
{
    ACE_Message_Block *msg_block = new ACE_Message_Block(RECV_REQUEST_SIZE);
    int32 recv_size = peer().recv(msg_block->base(), RECV_REQUEST_SIZE);

    if(recv_size > 0)
    {
        msg_block->wr_ptr(recv_size);
        
        if(putq(msg_block) < 0)
        {
            delete msg_block;
        }
    }
    else
    {
        delete msg_block;

        if(recv_size == 0 || errno != EWOULDBLOCK)
        {
            //close
            return -1;
        }
    }

    return 0;
}

int Connection::handle_output(ACE_HANDLE hd)
{
    ACE_Message_Block *msg_block;

    if(getq(msg_block) < -1)
    {
        return 0;
    }
    
    int32 send_size = peer().send(msg_block, msg_block->length());

    if(send_size > 0)
    {        
    }    
}

}
}
