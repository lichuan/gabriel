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
 *   @date: 2014-01-09 12:26:17                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__GAME__AI_ACTION
#define GABRIEL__GAME__AI_ACTION

#include <vector>
#include <set>
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace game {

class Ai_Action;
class Ai_Entity;
class Ai_Controller;
    
class Ai_Action_List
{
public:
    Ai_Action_List()
    {
        m_current_ai_action = NULL;
    }

    void add_action(Ai_Action *action)
    {
        m_actions.push_back(action);
    }

    bool doing();
    
private:
    Ai_Action *m_current_ai_action;
    std::vector<Ai_Action*> m_actions;
};

class Ai_Action
{
public:
    Ai_Action(Ai_Entity *holder)
    {
        m_holder = holder;
    }
    
    virtual ~Ai_Action();
    virtual bool can_do() = 0;    
    virtual bool doing() = 0;
    virtual bool on_enter();
    virtual bool on_leave();
    void do_sibling();
    
    void add_sibling_action(Ai_Action_List &action_list)
    {
        m_sibling_actions.push_back(action_list);
    }
    
protected:
    Ai_Controller* ai_controller() const;
    Ai_Entity *m_holder;
    
private:
    std::vector<Ai_Action_List> m_sibling_actions;    
};

class Stand_Ai_Action : public Ai_Action
{
public:
    Stand_Ai_Action(Ai_Entity *holder);    
    virtual ~Stand_Ai_Action();    
    virtual bool can_do();
    virtual bool doing();
    virtual bool on_enter();
    virtual bool on_leave();
};

class Follow_Ai_Action : public Ai_Action
{
public:
    Follow_Ai_Action(Ai_Entity *holder);
    virtual ~Follow_Ai_Action();
    virtual bool can_do();
    virtual bool doing();
    
private:
    Ai_Entity *m_follow_entity;
};

class Ai_Controller
{
public:
    Ai_Controller(Ai_Entity *holder);
    ~Ai_Controller();
    void doing();
    
    void build_ai_action(uint32 ai_id);
    
    void build_ai_0();    
    void build_ai_1();
    void build_ai_2();
    void build_ai_3();
    
    void delete_action();
    void add_action(Ai_Action *action);
    
    bool enable_ai() const
    {
        return m_enable_ai;
    }

    void enable_ai(bool enable)
    {
        m_enable_ai = enable;
    }
    
private:
    void register_build_func();
    Ai_Entity *m_holder;
    Ai_Action_List m_first_action_list;
    std::set<Ai_Action*> m_all_actions;
    void (Ai_Controller::*m_build_funcs[10])();
    Ai_Action_List build_action_list(Ai_Action **action_arr, uint32 arr_length);
    bool m_enable_ai;
};

class Ai_Entity
{
public:
    Ai_Entity() : m_ai_controller(this)
    {
    }
    
    Ai_Controller m_ai_controller;
};

}
}

#endif
