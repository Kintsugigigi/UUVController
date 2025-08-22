//
// @Time     : 2024/5/29 10:53
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_TIMETOOLS_H
#define AUV5PI_TIMETOOLS_H
#include "chrono"

namespace YTimeTools
{
    /** 获取当前时间 毫秒**/
    inline double getCurrentTime()
    {
        return std::chrono::system_clock::now().time_since_epoch().count() * 1e-6;
    }
}

#endif //AUV5PI_TIMETOOLS_H
