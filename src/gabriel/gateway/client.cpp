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
 *   @date: 2013-11-29 09:01:07                                        *
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

int ACE_TMAIN (int, ACE_TCHAR *[])
{
    
    // ACE_INET_Addr port_to_connect (20000);
    // ACE_Connector<TCPTask, ACE_SOCK_CONNECTOR> connector;
    // TCPTask task;
    // TCPTask *ptask = &task;
    
    // if (connector.connect (ptask, port_to_connect) == -1)
    //     ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
    //                        ACE_TEXT ("connect")), 1);
    // ACE_Reactor::instance()->run_event_loop();
    
}
