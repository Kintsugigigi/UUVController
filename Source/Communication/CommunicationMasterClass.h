//
// @Time     : 2023/12/11 18:11
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_COMMUNICATIONMASTERCLASS_H
#define AUV_COMMUNICATIONMASTERCLASS_H

#include "CommunicationHub.h"
#include "../Action/ActionHub.h"
#include "../Condition/ConditionMasterClass.h"
#include "../Control/ControlHub.h"
#include "../Navigation/NavigationHub.h"
#include "LockDriveClass.h"
#include "NetToUSVMasterClass.h"
#include "NetToPCClass.h"

class CommunicationMasterClass
{
public:
    CommunicationMasterClass();
    ~CommunicationMasterClass();

    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    // 机器人限制
    ControlHub::RobotLimitType * limitMenu;
    // 机器人运动目标
    ControlHub::RobotTargetType * targetMenu;
    // 无人艇状态信息
    NavigationHub::USVBackNavigationType * usvNavMenu;
    // 机器人控制参数
    ActionHub::USVControlType * usvControlMenu;
    // 推进器状态信息
    ActionHub::ThrusterStatusType * usvPropellerStatusMenu;
    // 遥控消息队列
    BlockingQueue<std::map<std::string, float>> * remote_queue;
    // 任务信息 队列
    BlockingQueue<std::map<std::string, double>> * path_queue;


    // 与USV主控连接的类
    NetToUSVMasterClass usvMasterClass;
    // 与上位机电脑连接的类
    NetToPCClass pcClass;

    // 开始机器人通讯
    void startConnect();

private:

    // 看门狗 线程
    std::thread watch_dog_thread;
    // 看门狗循环
    static void watchDogLoop(void*);
};


#endif //AUV_COMMUNICATIONMASTERCLASS_H
