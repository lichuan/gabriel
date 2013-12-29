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
 *   @date: 2013-11-29 09:00:21                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__ENTITY
#define GABRIEL__BASE__ENTITY

#include <string>
#include <map>
#include <vector>
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base {

template<typename ID_Type = uint32>
class Entity
{
public:
    typedef ID_Type Entity_ID_Type;    
    typedef std::string Entity_Name_Type;
    
    Entity()
    {
        m_id = 0;
    }
    
    virtual ~Entity()
    {
    }
    
    void id(ID_Type _id)
    {
        m_id = _id;
    }

    ID_Type id() const
    {
        return m_id;
    }

    void name(std::string _name)
    {
        m_name = _name;
    }

    std::string name() const
    {
        return m_name;
    }
    
private:
    ID_Type m_id;
    std::string m_name;
};

//对所有子元素进行回调
template<typename Concrete_Entity>
class Entity_Exec
{
public:
    Entity_Exec()
    {
    }

    virtual ~Entity_Exec()
    {
    }

    virtual bool exec(Concrete_Entity *entity) = 0;
    
    virtual bool can_delete(Concrete_Entity *entity) const
    {
        return true;        
    }    
};

template<typename Key, typename Entity_ID_Type>
class Entity_Map
{
protected:
    Entity_Map()
    {
        m_in_iteration = false;        
    }

    ~Entity_Map()
    {
    }

    //是否正在迭代中, 用于禁止在迭代map时进行删除迭代器等操作
    bool in_iteration() const
    {
        return m_in_iteration;
    }

    bool add_entity(Key key, Entity<Entity_ID_Type> *entity)
    {
        if(in_iteration())
        {
            return false;
        }
        
        if(m_entity_map.find(key) != m_entity_map.end())
        {
            return false;
        }
        
        m_entity_map[key] = entity;

        return true;        
    }

    Entity<Entity_ID_Type>* get_entity(Key key) const
    {
        typename std::map<Key, Entity<Entity_ID_Type>*>::const_iterator iter = m_entity_map.find(key);

        if(iter == m_entity_map.end())
        {
            return NULL;
        }

        return iter->second;
    }

    uint32 size() const
    {
        return m_entity_map.size();
    }

    bool empty() const
    {
        return m_entity_map.empty();
    }

    void clear()
    {
        if(!in_iteration())
        {
            m_entity_map.clear();
        }        
    }

    template<typename Concrete_Entity>
    void exec_all(Entity_Exec<Concrete_Entity> &cb)
    {
        Guard_Scope_Value<bool> scope_value(m_in_iteration, true, false);
        
        for(typename std::map<Key, Entity<Entity_ID_Type>*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
        {
            cb.exec(static_cast<Concrete_Entity*>(iter->second));
        }
    }

    template<typename Concrete_Entity>
    bool exec_until(Entity_Exec<Concrete_Entity> &cb)
    {
        Guard_Scope_Value<bool> scope_value(m_in_iteration, true, false);
        
        for(typename std::map<Key, Entity<Entity_ID_Type>*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
        {
            if(cb.exec(static_cast<Concrete_Entity*>(iter->second)))
            {
                return true;                
            }            
        }

        return false;        
    }
    
    template<typename Concrete_Entity>
    bool exec_if(Entity_Exec<Concrete_Entity> &cb)
    {
        bool ret = false;
        Guard_Scope_Value<bool> scope_value(m_in_iteration, true, false);
        
        for(typename std::map<Key, Entity<Entity_ID_Type>*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
        {
            Concrete_Entity *concrete_entity = static_cast<Concrete_Entity*>(iter->second);

            if(cb.exec(concrete_entity))
            {
                ret = true;
            }
        }
        
        return ret;        
    }
    
    void delete_entity(Key key)
    {
        if(!in_iteration())
        {
            m_entity_map.erase(key);
        }        
    }
    
    template<typename Concrete_Entity>
    bool delete_if(Entity_Exec<Concrete_Entity> &cb, std::vector<Concrete_Entity*> &del_vec)
    {
        if(in_iteration())
        {
            return false;
        }
        
        Guard_Scope_Value<bool> scope_value(m_in_iteration, true, false);
        bool ret = false;

        for(typename std::map<Key, Entity<Entity_ID_Type>*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
        {
            Concrete_Entity *concrete_entity = static_cast<Concrete_Entity*>(iter->second);
            
            if(cb.can_delete(concrete_entity))
            {
                ret = true;                
                del_vec.push_back(concrete_entity);
            }
        }
        
        return ret;        
    }
    
private:
    std::map<Key, Entity<Entity_ID_Type>*> m_entity_map;
    bool m_in_iteration;    
};

template<typename Concrete_Entity>
class KEY_ID : protected Entity_Map<typename Concrete_Entity::Entity_ID_Type, typename Concrete_Entity::Entity_ID_Type>
{
protected:
    typedef Entity_Map<typename Concrete_Entity::Entity_ID_Type, typename Concrete_Entity::Entity_ID_Type> Super;
    
    KEY_ID()
    {
    }

    bool add_entity(Concrete_Entity *entity)
    {
        return Super::add_entity(entity->id(), entity);
    }

    void delete_entity(Concrete_Entity *entity)
    {
        Super::delete_entity(entity->id());
    }
};

template<typename Concrete_Entity>
class KEY_NAME : protected Entity_Map<typename Concrete_Entity::Entity_Name_Type, typename Concrete_Entity::Entity_ID_Type>
{
protected:
    typedef Entity_Map<typename Concrete_Entity::Entity_Name_Type, typename Concrete_Entity::Entity_ID_Type> Super;
    
    KEY_NAME()
    {
    }

    bool add_entity(Concrete_Entity *entity)
    {
        return Super::add_entity(entity->name(), entity);
    }

    void delete_entity(Concrete_Entity *entity)
    {
        Super::delete_entity(entity->name());
    }    
};

//占位基类
template<typename Concrete_Entity>
class KEY_NONE
{
protected:
    KEY_NONE()
    {
    }

    void clear()
    {
    }

    bool add_entity(Concrete_Entity *entity)
    {
        return true;        
    }
    
    void delete_entity(Concrete_Entity *entity)
    {
    }    
};

template<typename Concrete_Entity>
class KEY_NONE_1 : public KEY_NONE<Concrete_Entity>
{
};

template<typename Concrete_Entity>
class KEY_NONE_2 : public KEY_NONE<Concrete_Entity>
{
};

//提取基类的trait
template<typename Concrete_Entity, typename T>
struct Get_Super
{
};

template<typename Concrete_Entity>
struct Get_Super<Concrete_Entity, typename Concrete_Entity::Entity_ID_Type>
{
    typedef KEY_ID<Concrete_Entity> Super;    
};

template<typename Concrete_Entity>
struct Get_Super<Concrete_Entity, typename Concrete_Entity::Entity_Name_Type>
{
    typedef KEY_NAME<Concrete_Entity> Super;    
};

template<typename Concrete_Entity, template<typename> class Super_T_1, template<typename> class Super_T_2 = KEY_NONE>
class Entity_Manager : private Super_T_1<Concrete_Entity>, private Super_T_2<Concrete_Entity>
{
public:
    typedef Super_T_1<Concrete_Entity> Super1;
    typedef Super_T_2<Concrete_Entity> Super2;
    
    Entity_Manager()
    {
    }
    
    bool add_entity(Concrete_Entity *entity)
    {
        if(!Super1::add_entity(entity))
        {
            return false;
        }

        if(!Super2::add_entity(entity))
        {
            Super1::delete_entity(entity);

            return false;            
        }

        return true;
    }
    
    bool delete_if(Entity_Exec<Concrete_Entity> &cb)
    {
        std::vector<Concrete_Entity*> del_vec;

        if(!Super1::delete_if(cb, del_vec))
        {
            return false;
        }

        for(typename std::vector<Concrete_Entity*>::iterator iter = del_vec.begin(); iter != del_vec.end(); ++iter)
        {
            Concrete_Entity *concrete_entity = *iter;
            delete_entity(concrete_entity);
            cb.exec(concrete_entity);
        }

        return true;
    }

    void delete_all(Entity_Exec<Concrete_Entity> &cb)
    {
        exec_all(cb);
        clear();
    }
    
    Concrete_Entity* get_entity(typename Concrete_Entity::Entity_ID_Type key) const
    {
        return static_cast<Concrete_Entity*>(Get_Super<Concrete_Entity, typename Concrete_Entity::Entity_ID_Type>::Super::get_entity(key));
    }

    Concrete_Entity* get_entity(typename Concrete_Entity::Entity_Name_Type key) const
    {
        return static_cast<Concrete_Entity*>(Get_Super<Concrete_Entity, typename Concrete_Entity::Entity_Name_Type>::Super::get_entity(key));
    }
    
    void delete_entity(Entity<typename Concrete_Entity::Entity_ID_Type> *entity)
    {
        Super1::delete_entity(entity);
        Super2::delete_entity(entity);
    }
    
    uint32 size() const
    {
        return Super1::size();
    }

    bool empty() const
    {
        return Super1::empty();
    }

    void clear()
    {
        Super1::clear();
        Super2::clear();
    }

    bool exec_until(Entity_Exec<Concrete_Entity> &cb)
    {
        return Super1::exec_until(cb);
    }
    
    bool exec_if(Entity_Exec<Concrete_Entity> &cb)
    {
        return Super1::exec_if(cb);
    }
    
    void exec_all(Entity_Exec<Concrete_Entity> &cb)
    {
        Super1::exec_all(cb);
    }
};

}
}

#endif

