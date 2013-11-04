/*
 author: lichuan
 qq: 308831759
 email: 308831759@qq.com
 homepage: www.lichuan.me
 github: https://github.com/lichuan/gabriel
 date: 2013-08-23
 desc: a星寻路的一种高效实现
*/

#ifndef GABRIEL_BASE_ASTAR
#define GABRIEL_BASE_ASTAR

#include <list>
#include <cstdlib>
#include <map>
#include <iostream>
#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base {
        
struct Point
{
    Point(int32 x, int32 y)
    {
        m_x = x;
        m_y = y;
    }

    Point()
    {
        m_x = 0;
        m_y = 0;
    }

    int32 hash() const
    {
        return m_x * 10000 + m_y;        
    }
    
    bool operator==(const Point &pos) const
    {
        return m_x == pos.m_x && m_y == pos.m_y;
    }    
    
    int32 m_x;
    int32 m_y;
};

//路径节点
struct Node
{
    Node(const Point &pos, int32 g, int32 h, const Node *parent)
    {
        m_pos = pos;
        m_g = g;
        m_h = h;
        m_f = g + h;        
        m_parent = parent;        
    }

    Point m_pos; //节点的坐标
    int32 m_g; //缓存g值
    int32 m_h; //缓存h    
    int32 m_f; //缓存f值
    const Node *m_parent; //该节点的前置节点
};

class Point_CB
{
public:
    virtual bool is_valid(const Point &pos) const = 0;
    
    virtual bool is_reach(const Point &pos, const Point &dest_pos) const
    {
        return pos == dest_pos;
    }    
};

//A星寻路实现
template<int32 MAX_NODE>
class Astar_Impl
{
    template<int32>
    friend class Astar;
    
private:
    Astar_Impl(const Point_CB &point_cb) : m_point_cb(point_cb)
    {
    }

    ~Astar_Impl()
    {
        clear_node();        
    }
    
    bool is_valid(const Point &pos) const
    {
        return m_point_cb.is_valid(pos);
    }

    bool is_reach(const Point &pos, const Point &dest_pos) const
    {
        return m_point_cb.is_reach(pos, dest_pos);
    }
    
    bool exist_in_close(const Point &pos) const
    {
        return m_close_map.find(pos.hash()) != m_close_map.end();
    }

    void clear_node()
    {
        for(std::map<int32, Node*>::const_iterator iter = m_open_map.begin(); iter != m_open_map.end(); ++iter)
        {
            delete iter->second;
        }
        
        for(std::map<int32, Node*>::const_iterator iter = m_close_map.begin(); iter != m_close_map.end(); ++iter)
        {
            delete iter->second;
        }
    }
    
    Node* get_minimum_f_node_from_open() const
    {
        std::multimap<int32, Node*>::const_iterator iter = m_order_by_f_map.begin();

        return iter->second;
    }

    Node* get_node_from_open(const Point &pos) const
    {
        std::map<int32, Node*>::const_iterator iter = m_open_map.find(pos.hash());

        if(iter == m_open_map.end())
        {
            return NULL;
        }

        return iter->second;
    }
    
    int32 h_value(const Point &src_pos, const Point &dest_pos) const
    {
        int32 x_diff = src_pos.m_x - dest_pos.m_x;
        int32 y_diff = src_pos.m_y - dest_pos.m_y;
        x_diff = abs(x_diff);
        y_diff = abs(y_diff);
        int32 v1 = 0;
        int32 v2 = 0;        

        if(x_diff > y_diff)
        {
            v1 = x_diff - y_diff;
            v2 = y_diff;            
        }
        else
        {
            v1 = y_diff - x_diff;
            v2 = x_diff;
        }

        return v1 * 10 + v2 * 14; //横竖方向的权重10，斜方向的权重14
    }

    const Node* add_around_node_to_open(const Node *cur_node, const Point &dest_pos)
    {
        static const int32 around_offset[8][2] = {
            {0, -1}, //top
            {1, -1}, //top-right
            {1, 0},
            {1, 1},
            {0, 1},
            {-1, 1},
            {-1, 0},
            {-1, -1},
        };

        const Point &cur_pos = cur_node->m_pos;
        int32 g_inc = 0;        
        
        for(int32 i = 0; i != 8; ++i)
        {
            const Point pos(cur_pos.m_x + around_offset[i][0], cur_pos.m_y + around_offset[i][1]);

            if(is_reach(pos, dest_pos))
            {
                return cur_node;
            }

            if(!is_valid(pos))
            {
                continue;
            }

            if(exist_in_close(pos))
            {
                continue;
            }

            g_inc = 10;
            
            //判断斜方向的两边是否有阻挡
            if(i % 2 == 1)
            {
                const int32 idx_1 = (i - 1) % 8;
                const int32 idx_2 = (i + 1) % 8;
                const Point pos_1(cur_pos.m_x + around_offset[idx_1][0], cur_pos.m_y + around_offset[idx_1][1]);
                const Point pos_2(cur_pos.m_x + around_offset[idx_2][0], cur_pos.m_y + around_offset[idx_2][1]);

                if(!is_valid(pos_1) || !is_valid(pos_2))
                {
                    continue;
                }

                g_inc = 14;                
            }

            Node *node = get_node_from_open(pos);
            const int32 g = cur_node->m_g + g_inc;

            if(node != NULL)
            {
                if(node->m_g > g)
                {
                    node->m_parent = cur_node;
                    node->m_g = g;
                    node->m_f = g + node->m_h;
                    m_order_by_f_map.erase(m_iter_map[node]);
                    m_iter_map[node] = m_order_by_f_map.insert(m_order_by_f_map.begin(), std::make_pair(node->m_f, node));                    
                }

                continue;
            }

            Node *new_node = new Node(pos, g, h_value(pos, dest_pos), cur_node);
            m_open_map.insert(std::make_pair(pos.hash(), new_node));
            m_iter_map.insert(std::make_pair(new_node, m_order_by_f_map.insert(m_order_by_f_map.begin(), std::make_pair(new_node->m_f, new_node))));            
        }

        return NULL;            
    }
    
    std::list<Point> find_path(const Point &src_pos, const Point &dest_pos)
    {
        std::list<Point> pos_list;
        
        if(!is_valid(src_pos) || !is_valid(dest_pos) || is_reach(src_pos, dest_pos))
        {
            return pos_list;
        }
        
        //反向查找
        const Node *node = new Node(dest_pos, 0, h_value(dest_pos, src_pos), NULL);
        m_open_map.insert(std::make_pair(dest_pos.hash(), node));        
        m_iter_map.insert(std::make_pair(node, m_order_by_f_map.insert(m_order_by_f_map.begin(), std::make_pair(node->m_f, node))));
        int32 num = 0;        

        while(!m_open_map.empty())
        {
            if(++num > MAX_NODE)
            {
                return pos_list;
            }
            
            Node *node = get_minimum_f_node_from_open();                        
            m_open_map.erase(node->m_pos.hash());
            m_order_by_f_map.erase(m_iter_map[node]);
            m_iter_map.erase(node);
            m_close_map.insert(std::make_pair(node->m_pos.hash(), node));                        
            const Node *first_node = add_around_node_to_open(node, src_pos);

            if(first_node != NULL)
            {
                const Node *node = first_node;
                
                while(node != NULL)
                {
                    pos_list.push_back(node->m_pos);
                    node = node->m_parent;                    
                }

                return pos_list;                
            }
        }

        return pos_list;
    }

    std::map<int32, Node*> m_open_map;
    std::map<int32, Node*> m_close_map;
    std::multimap<int32, Node*> m_order_by_f_map;
    std::map<Node*, std::multimap<int32, Node*>::iterator> m_iter_map;    
    const Point_CB &m_point_cb;    
};
    
template<int32 MAX_NODE>
class Astar
{
public:
    static Astar* instance()
    {
        static Astar singleton;

        return &singleton;
    }

    //对每一次查找单独创建一个A星实现类，以支持在多线程环境中进行A星寻路
    std::list<Point> find_path(const Point_CB &point_cb, const Point &src_pos, const Point &dest_pos) const
    {
        Astar_Impl<MAX_NODE> impl(point_cb);
        
        return impl.find_path(src_pos, dest_pos);
    }

private:
    Astar()
    {
    }
};
} //end namespace base
} //end namespace gabriel

#endif
