//
// @Time     : 2023/12/10 16:40
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_ACTIONMASTERCLASS_H
#define AUV_ACTIONMASTERCLASS_H

#include "map"
#include "thread"
#include "../Tools/BlockingQueue.h"
#include "../Control/ControlHub.h"
#include "ActionHub.h"


class ActionMasterClass
{
public:
    ActionMasterClass();
    ~ActionMasterClass();

//    // 各执行机构 控制参数
//    ActionHub::ActionControlType actionMenu;

    // 机器人控制参数
    ActionHub::USVControlType usvControlMenu;
    // 推进器状态信息
    ActionHub::ThrusterStatusType usvPropellerStatusMenu;
    // 获取执行机构命令消息队列
    BlockingQueue<std::map<std::string, float>> action_queue;

    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    // 机器人限制
    ControlHub::RobotLimitType * limitMenu;

    // 控制线程
    //std::thread control_thread;
    // 开启连接
    void startAction();

private:
    // 控制间隔 毫秒
    int interval_num = 1000;
    // 消息队列 锁
    std::mutex mutex_action_cout;
    // 控制线程
    std::thread get_control_thread;
    // 控制循环
    static void controlLoop(void*);
    // 获取控制命令循环
    static void getControlLoop(void*);
    // 运动控制命令解析
    void cookCommandForMove(const char[], const int&);
    // 灯控制命令解析
    void cookCommandForLED(const char[], const int&);
    // 停止
    void stopSet();
    // 舵机归零位
    void zeroSet();
    // 看门狗 线程
    std::thread watch_dog_thread;
    // 看门狗循环
    static void watchDogLoop(void*);
};

#endif //AUV_ACTIONMASTERCLASS_H
