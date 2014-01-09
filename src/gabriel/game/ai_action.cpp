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
 *   @date: 2014-01-09 12:26:34                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gabriel/game/ai_action.hpp"

namespace gabriel {
namespace game {

Ai_Action::~Ai_Action()
{
}

Ai_Controller* Ai_Action::ai_controller() const
{
    return &m_holder->m_ai_controller;
}

bool Ai_Action::on_enter()
{
    return true;    
}
    
void Ai_Action::do_sibling()
{
    for(std::vector<Ai_Action_List>::iterator iter = m_sibling_actions.begin(); iter != m_sibling_actions.end(); ++iter)
    {
        Ai_Action_List &action_list = *iter;
        action_list.doing();
    }
}

bool Ai_Action::on_leave()
{
    return true;    
}
    
Stand_Ai_Action::Stand_Ai_Action(Ai_Entity *holder) : Ai_Action(holder)
{
}

Stand_Ai_Action::~Stand_Ai_Action()
{
}

bool Stand_Ai_Action::can_do()
{
    return true;    
}

bool Stand_Ai_Action::doing()
{
    return true;
}

bool Stand_Ai_Action::on_enter()
{
    return true;
}

bool Stand_Ai_Action::on_leave()
{
    return true;
}

Follow_Ai_Action::Follow_Ai_Action(Ai_Entity *holder) : Ai_Action(holder)
{
}

Follow_Ai_Action::~Follow_Ai_Action()
{
}

bool Follow_Ai_Action::can_do()
{
    return true;    
}

bool Follow_Ai_Action::doing()
{
    return true;
}

Ai_Controller::Ai_Controller(Ai_Entity *holder)
{
    m_holder = holder;
    m_enable_ai = false;
    register_build_func();    
}

Ai_Controller::~Ai_Controller()
{
    delete_action();
}

void Ai_Controller::doing()
{
    if(!enable_ai())
    {
        return;
    }
    
    m_first_action_list.doing();
}

bool Ai_Action_List::doing()
{
    for(std::vector<Ai_Action*>::iterator iter = m_actions.begin(); iter != m_actions.end(); ++iter)
    {
        Ai_Action *action = *iter;

        if(action->can_do())
        {
            if(action != m_current_ai_action)
            {
                if(m_current_ai_action != NULL)
                {
                    m_current_ai_action->on_leave();
                }

                action->on_enter();
            }

            m_current_ai_action = action;
            m_current_ai_action->doing();
            m_current_ai_action->do_sibling();

            return true;            
        }
    }

    return false;    
}

void Ai_Controller::build_ai_action(uint32 ai_id)
{    
    if(ai_id >= 10)
    {
        return;
    }

    (this->*m_build_funcs[ai_id])();
}

void Ai_Controller::register_build_func()
{
    m_build_funcs[0] = &Ai_Controller::build_ai_0;    
    m_build_funcs[1] = &Ai_Controller::build_ai_1;
    m_build_funcs[2] = &Ai_Controller::build_ai_2;
    m_build_funcs[3] = &Ai_Controller::build_ai_3;
}

Ai_Action_List Ai_Controller::build_action_list(Ai_Action **action_arr, uint32 arr_length)
{
    Ai_Action_List action_list;
    
    for(uint32 i = 0; i != arr_length; ++i)
    {
        Ai_Action *action = action_arr[i];
        add_action(action);
        action_list.add_action(action);
    }
    
    return action_list;
}

void Ai_Controller::build_ai_0()
{
    Ai_Action* action_list[] = {
        new Stand_Ai_Action(m_holder),
        new Follow_Ai_Action(m_holder),
    };

    m_first_action_list = build_action_list(action_list, 2);
}

void Ai_Controller::build_ai_1()
{
    Ai_Action* action_list[] = {
        new Stand_Ai_Action(m_holder),
        new Follow_Ai_Action(m_holder),
    };

    m_first_action_list = build_action_list(action_list, 2);
}

void Ai_Controller::build_ai_2()
{
    Ai_Action* action_list[] = {
        new Stand_Ai_Action(m_holder),
        new Follow_Ai_Action(m_holder),
    };

    m_first_action_list = build_action_list(action_list, 2);
}

void Ai_Controller::build_ai_3()
{
    Ai_Action* action_list[] = {
        new Stand_Ai_Action(m_holder),
        new Follow_Ai_Action(m_holder),
    };

    m_first_action_list = build_action_list(action_list, 2);
}

void Ai_Controller::add_action(Ai_Action *action)
{
    m_all_actions.insert(action);
}

void Ai_Controller::delete_action()
{
    for(std::set<Ai_Action*>::iterator iter = m_all_actions.begin(); iter != m_all_actions.end(); ++iter)
    {
        delete *iter;
    }
}

}
}
