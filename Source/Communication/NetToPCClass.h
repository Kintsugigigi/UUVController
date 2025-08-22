//
// @Time     : 2024/6/5 19:46
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_NETTOPCCLASS_H
#define AUV5PI_NETTOPCCLASS_H

#include "map"
#include "CommunicationHub.h"
#include "../Postman/UDPSocketBaseClass.h"
#include "../Control/ControlHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Action/ActionHub.h"

class NetToPCClass
{

public:
    NetToPCClass();
    ~NetToPCClass();

    // 与上位机连接状态
    CommunicationHub::PCType pcComStatusMenu;
    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    // 机器人运动目标
    ControlHub::RobotTargetType * targetMenu;
    // 无人艇导航相关信息
    NavigationHub::USVBackNavigationType * usvNavMenu;
    // 推进器状态信息
    ActionHub::ThrusterStatusType * usvPropellerStatusMenu;
    // 机器人控制参数
    ActionHub::USVControlType * usvControlMenu;
    // 连接艇上位机程序的类
    UDPSocketBaseClass  pcSocketClass;

    // 连接无线电
    void connectPC(const std::string &, const int &,
                          const std::string &, const int &);

private:
    // 控制指令发送间隔  毫秒
    int interval_manual = 100000;

    // 从上位机获取消息 线程
    std::thread get_msg_thread;
    // 向上位机发送消息 线程
    std::thread send_msg_thread;
    // 从上位机获取消息 循环
    static void getMessageLoop(void*);
    // 向上位机发送消息 循环
    static void sendMessageLoop(void*);
    // 处理 上位机获取消息
    bool cookGetMessage(unsigned char[], int);
    // 处理 向上位机发送消息
    void sendMessageNow(unsigned char*, int);

};


#endif //AUV5PI_NETTOPCCLASS_H
