#ifndef GABRIEL__BASE__CONNECTION
#define GABRIEL__BASE__CONNECTION

namespace gabriel {
namespace base {

class Connection
{
public:
    Connection();
    virtual ~Connection();
    virtual void on_disconnect() = 0;
    void disconnect();
};

}
}

#endif
