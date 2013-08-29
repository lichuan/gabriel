#include "gabriel/base/astar.hpp"

using namespace gabriel::base;

class CB
{
public:
    bool is_block(const Point &pos) const
    {
        if(pos.m_x == 500 && pos.m_y != 0)
        {
            return true;
        }
        
        return false;
    }
};

int main()
{
    CB cb;
    int32 x1, y1, x2, y2;
    std::cin >> x1 >> y1 >> x2 >> y2;
    std::cout << "src: (" << x1 << ", " << y1 << ") dest:(" << x2 << ", " << y2 << ")" << std::endl;
    struct timeval time_begin, time_end;
    gettimeofday(&time_begin, NULL);
    std::cout << "start time: " << time_begin.tv_sec << " " << time_begin.tv_usec << std::endl;    
    std::list<Point> path_list = Astar<500000>::instance()->find_path(cb, Point(x1, y1), Point(x2, y2));
    gettimeofday(&time_end, NULL);
    std::cout << "end time: " << time_end.tv_sec << " " << time_end.tv_usec << std::endl;
    std::cout << "cost time: " << 1000000 * (time_end.tv_sec - time_begin.tv_sec) + time_end.tv_usec - time_begin.tv_usec << " 微秒" << std::endl;    
    
    for(std::list<Point>::const_iterator iter = path_list.begin(); iter != path_list.end(); ++iter)
    {
        std::cout << "(" << iter->m_x << ", " << iter->m_y << ")->";
    }

    std::cout << std::endl;
}
