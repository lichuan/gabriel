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
 *   @date: 2013-11-29 09:00:15                                        *
 *                                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef GABRIEL__BASE__DEFINE
#define GABRIEL__BASE__DEFINE

#include "ace/Basic_Types.h"

typedef ACE_INT8 int8;
typedef ACE_UINT8 uint8;
typedef ACE_INT16 int16;
typedef ACE_UINT16 uint16;
typedef ACE_INT32 int32;
typedef ACE_UINT32 uint32;
typedef ACE_INT64 int64;
typedef ACE_UINT64 uint64;

#define MIN_INT8 ACE_CHAR_MIN
#define MAX_INT8 ACE_CHAR_MAX
#define MAX_UINT8 ACE_OCTET_MAX

#define MIN_INT16 ACE_INT16_MIN
#define MAX_INT16 ACE_INT16_MAX
#define MAX_UINT16 ACE_UINT16_MAX

#define MIN_INT32 ACE_INT32_MIN
#define MAX_INT32 ACE_INT32_MAX
#define MAX_UINT32 ACE_UINT32_MAX

#define MIN_INT64 ACE_INT64_MIN
#define MAX_INT64 ACE_INT64_MAX
#define MAX_UINT64 ACE_UINT64_MAX

#define MIN_FLOAT ACE_FLT_MIN
#define MAX_FLOAT ACE_FLT_MAX

#define MIN_DOUBLE ACE_DBL_MIN
#define MAX_DOUBLE ACE_DBL_MAX

namespace gabriel {
namespace base {

const uint32 RECV_REQUEST_SIZE = 4096;
const uint32 MSG_QUEUE_HWM = 64 * 1024;
const uint32 MSG_QUEUE_LWM = MSG_QUEUE_HWM;
        
enum SERVER_TYPE
{
    INVALID_SERVER = 0,
    SUPERCENTER_SERVER = 1,    
    CENTER_SERVER = 2,
    RECORD_SERVER = 3,
    LOGIN_SERVER = 4,
    GAME_SERVER = 5,
    GATEWAY_SERVER = 6,
};

enum CLIENT_TYPE
{
    INVALID_CLIENT = 0,
    CENTER_CLIENT = CENTER_SERVER,
    RECORD_CLIENT = RECORD_SERVER,
    LOGIN_CLIENT = LOGIN_SERVER,
    GAME_CLIENT = GAME_SERVER,
    GATEWAY_CLIENT = GATEWAY_SERVER,
    USER_CLIENT = GATEWAY_CLIENT + 1,
};

enum CONNECTION_STATE
{
    INVALID_STATE = 0,
    CONNECTED_STATE = 1,
    CLOSING_STATE = 2,
    CLOSED_STATE = 3,
};

}
}

#endif
