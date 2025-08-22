//
// @Time     : 2024/1/17 22:37
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_LOGTOTEXTCLASS_H
#define AUV_LOGTOTEXTCLASS_H

#include "thread"
#include <fstream>
#include <ctime>
#include <unistd.h>
#include "../Postman/UDPSocketBaseClass.h"
#include "../Navigation/NavigationHub.h"
//#include "../Communication/SoundClass.h"


class LogToTextClass
{
public:
    LogToTextClass();
    ~LogToTextClass();

//    // GPS 信息汇总
//    NavigationHub::GPSMessageType * GPSMenu{};
//    // INS 信息汇总
//    NavigationHub::INSMessageType * insMenu{};
//    // DVL 信息汇总
//    NavigationHub::DVLMessageType * dvlMenu{};
//    // Depthometer 信息汇总
//    NavigationHub::DepthMessageType * depthMenu{};
//    // 测距 信息汇总
//    CommunicationHub::SoundType * soundMenu{};

    void startLog();

private:
    // 记录间隔  毫秒
    int interval_num = 500;
    // 从动力锁获取消息 线程
    std::thread log_thread;

    // 文件
    std::ofstream logfile;

    // 从动力锁获取消息 循环
    static void logMessageLoop(void*);
};


#endif //AUV_LOGTOTEXTCLASS_H
