#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <string>
#include <map>

typedef unsigned int uint32;

class Entity
{
protected:
    Entity()
    {
        m_id = 0;
    }

public:
    virtual ~Entity()
    {
    }

    void id(uint32 _id)
    {
        m_id = _id;
    }

    uint32 id() const
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
    uint32 m_id;
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

template<typename Key>
class Entity_Map
{
protected:
    Entity_Map()
    {
    }

    ~Entity_Map()
    {
    }

    bool add_entity(Key key, Entity *entity)
    {
        if(m_entity_map.find(key) != m_entity_map.end())
        {
            return false;
        }
        
        m_entity_map[key] = entity;

        return true;        
    }

    Entity* get_entity(Key key) const
    {
        typename std::map<Key, Entity*>::const_iterator iter = m_entity_map.find(key);

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

    void clear() const
    {
        m_entity_map.clear();
    }

    template<typename Concrete_Entity>
    void exec(Entity_Exec<Concrete_Entity> &cb)
    {
        for(typename std::map<Key, Entity*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
        {
            cb.exec(static_cast<Concrete_Entity*>(iter->second));            
        }
    }

    template<typename Concrete_Entity>
    bool exec_until(Entity_Exec<Concrete_Entity> &cb)
    {
        for(typename std::map<Key, Entity*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
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
        
        for(typename std::map<Key, Entity*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
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
        m_entity_map.erase(key);
    }
    
    template<typename Concrete_Entity>
    bool delete_if(Entity_Exec<Concrete_Entity> &cb, std::vector<Entity*> &del_vec)
    {
        bool ret = false;
        
        for(typename std::map<Key, Entity*>::iterator iter = m_entity_map.begin(); iter != m_entity_map.end(); ++iter)
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
    std::map<Key, Entity*> m_entity_map;
};

class KEY_UINT32 : protected Entity_Map<uint32>
{
protected:
    typedef Entity_Map<uint32> Super;
    
    KEY_UINT32()
    {
    }

    bool add_entity(Entity *entity)
    {
        return Super::add_entity(entity->id(), entity);
    }

    void delete_entity(Entity *entity)
    {
        Super::delete_entity(entity->id());
    }
};

class KEY_STRING : protected Entity_Map<std::string>
{
protected:
    typedef Entity_Map<std::string> Super;
    
    KEY_STRING()
    {
    }

    bool add_entity(Entity *entity)
    {
        return Super::add_entity(entity->name(), entity);
    }

    void delete_entity(Entity *entity)
    {
        Super::delete_entity(entity->name());
    }    
};

//占位
template<int>
class KEY_NONE
{
protected:
    KEY_NONE()
    {
    }

    void clear()
    {
    }

    bool add_entity(Entity *entity)
    {
        return true;        
    }
    
    void delete_entity(Entity *entity)
    {
    }    
};

//提取基类的trait
template<typename T>
struct Get_Super
{
};

template<>
struct Get_Super<uint32>
{
    typedef KEY_UINT32 Super;
};

template<>
struct Get_Super<std::string>
{
    typedef KEY_STRING Super;
};

template<typename Concrete_Entity_Type, typename Super1, typename Super2 = KEY_NONE<1> >
class Entity_Manager : private Super1, private Super2
{
protected:
    Entity_Manager()
    {
    }
    
    bool add_entity(Entity *entity)
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

    template<typename Concrete_Entity>
    bool delete_if(Entity_Exec<Concrete_Entity> &cb)
    {
        std::vector<Entity*> del_vec;

        if(!Super1::delete_if(cb, del_vec))
        {
            return false;
        }

        for(std::vector<Entity*>::iterator iter = del_vec.begin(); iter != del_vec.end(); ++iter)
        {
            Concrete_Entity *concrete_entity = static_cast<Concrete_Entity*>(concrete_entity);
            delete_entity(concrete_entity);
            cb.exec(concrete_entity);
        }

        return true;        
    }


    template<typename Key>
    Concrete_Entity_Type* get_entity(Key key) const
    {
        return static_cast<Concrete_Entity_Type*>(Get_Super<Key>::Super::get_entity(key));        
    }

    void delete_entity(Entity *entity)
    {
        Super1::delete_entity(entity);
        Super2::delete_entity(entity);
    }    

public:
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

    template<typename Concrete_Entity>
    bool exec_until(Entity_Exec<Concrete_Entity> &cb)
    {
        return Super1::exec_until(cb);
    }
    
    template<typename Concrete_Entity>
    bool exec_if(Entity_Exec<Concrete_Entity> &cb)
    {
        return Super1::exec_if(cb);
    }
    
    template<typename Concrete_Entity>
    void exec(Entity_Exec<Concrete_Entity> &cb)
    {
        Super1::exec(cb);
    }
};

#endif

