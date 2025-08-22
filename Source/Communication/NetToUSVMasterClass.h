//
// @Time     : 2024/6/5 19:46
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_NETTOUSVMASTERCLASS_H
#define AUV5PI_NETTOUSVMASTERCLASS_H

#include "map"
#include "CommunicationHub.h"
#include "../Postman/TCPSocketClientClass.h"
#include "../Postman/UDPSocketBaseClass.h"
#include "../Control/ControlHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Action/ActionHub.h"

class NetToUSVMasterClass
{
public:
    NetToUSVMasterClass();
    ~NetToUSVMasterClass();

    // 与主控连接状态
    CommunicationHub::USVMasterType usvComStatusMenu;
    // 接收主控消息的结构体
    CommunicationHub::USVMasterGet1Type usvGetMenu;
    // 发送给主控消息的结构体
    CommunicationHub::USVMasterSendType usvSendMenu;

    // 无人艇导航相关信息
    NavigationHub::USVBackNavigationType * usvNavMenu;
    // 推进器状态信息
    ActionHub::ThrusterStatusType * usvPropellerStatusMenu;
    // 机器人控制参数
    ActionHub::USVControlType * usvControlMenu;

    // 连接艇主控程序的类
    TCPSocketClientClass  usvSocketClass;

    // 连接无线电
    void connectUSVMaster(const std::string &, const int &,
                      const std::string &, const int &);

private:
    // 控制指令发送间隔  毫秒
    int interval_manual = 100;

    // 从主控获取消息 线程
    std::thread get_msg_thread;
    // 向主控发送消息 线程
    std::thread send_msg_thread;
    // 从主控获取消息 循环
    static void getMessageLoop(void*);
    // 向主控发送消息 循环
    static void sendMessageLoop(void*);
    // 处理 主控获取消息
    bool cookGetMessage(unsigned char[], int);
    // 处理 向主控发送消息
    void sendMessageNow(unsigned char*, int);

};


#endif //AUV5PI_NETTOUSVMASTERCLASS_H
