//
// @Time     : 2024/5/29 15:19
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_LOGPIDCLASS_H
#define AUV5PI_LOGPIDCLASS_H

#include "thread"
#include <fstream>
#include <ctime>
#include <unistd.h>
#include "../Postman/UDPSocketBaseClass.h"
#include "../Navigation/NavigationHub.h"

class LogPIDClass
{

public:
    LogPIDClass();
    ~LogPIDClass();

    // 文件
    std::ofstream logfile;

    void startLog();

private:


};


#endif //AUV5PI_LOGPIDCLASS_H
