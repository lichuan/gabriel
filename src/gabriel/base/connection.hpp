#ifndef GABRIEL__BASE__CONNECTION
#define GABRIEL__BASE__CONNECTION

#include "gabriel/base/common.hpp"

namespace gabriel {
namespace base {

class Connection
{
public:
    Connection();
    virtual ~Connection();    
    bool connected() const;
    void connected(bool b);
    void close();
    
private:
    bool m_connected;
};

}
}

#endif
