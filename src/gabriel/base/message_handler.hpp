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
 *   @date: 2014-01-12 13:55:43                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__MESSAGE_HANDLER
#define GABRIEL__BASE__MESSAGE_HANDLER

#include <map>
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base{

template<typename T, typename Connection_Type>
struct Message_Hanlder_Info
{
    Message_Hanlder_Info()
    {
        m_obj = NULL;
    }
    
    T *m_obj;
    void (T::*m_func_1)(Connection_Type *connection, void *data, uint32 size);
    void (T::*m_func_2)(Connection_Type *connection, uint32 msg_id, void *data, uint32 size);
    void (T::*m_func_3)(Connection_Type *connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size);
};

template<typename T, typename Connection_Type>
class Message_Handler
{
public:
    Message_Handler()
    {
    }
    
    void register_handler(uint32 msg_type, uint32 msg_id, T *obj, void (T::*handler)(Connection_Type *connection, void *data, uint32 size))
    {
        Message_Hanlder_Info<T, Connection_Type> info;
        info.m_obj = obj;
        info.m_func_1 = handler;
        m_handlers_1[msg_type][msg_id] = info;
    }
    
    void register_handler(uint32 msg_type, T *obj, void (T::*handler)(Connection_Type *connection, uint32 msg_id, void *data, uint32 size))
    {
        Message_Hanlder_Info<T, Connection_Type> info;
        info.m_obj = obj;
        info.m_func_2 = handler;
        m_handlers_2[msg_type] = info;
    }
    
    void register_handler(T *obj, void (T::*handler)(Connection_Type *connection, uint32 msg_type, uint32 msg_id, void *data, uint32 size))
    {
        m_handler.m_obj = obj;
        m_handler.m_func_3 = handler;
    }
    
    void handle_message(uint32 msg_type, uint32 msg_id, Connection_Type *connection, void *data, uint32 size)
    {
        auto iter_1 = m_handlers_1.find(msg_type);

        if(iter_1 != m_handlers_1.end())
        {
            auto &func_map = iter_1->second;                
            auto iter_func = func_map.find(msg_id);
                
            if(iter_func != func_map.end())
            {
                auto &info = iter_func->second;
                (info.m_obj->*info.m_func_1)(connection, data, size);
            }
        }
        
        auto iter_2 = m_handlers_2.find(msg_type);

        if(iter_2 != m_handlers_2.end())
        {
            auto &info = iter_2->second;
            (info.m_obj->*info.m_func_2)(connection, msg_id, data, size);
        }

        if(m_handler.m_obj != NULL)
        {
            (m_handler.m_obj->*m_handler.m_func_3)(connection, msg_type, msg_id, data, size);
        }
    }
    
private:
    std::map<uint32, std::map<uint32, Message_Hanlder_Info<T, Connection_Type>>> m_handlers_1;
    std::map<uint32, Message_Hanlder_Info<T, Connection_Type>> m_handlers_2;
    Message_Hanlder_Info<T, Connection_Type> m_handler;    
};

}
}

#endif
