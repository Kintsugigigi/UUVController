//
// @Time     : 2024/6/1 13:03
// @Author   : Yang
// @Software : CLion
// 手动遥控模式

#ifndef AUV5PI_REMOTECONTROLLERCLASS_H
#define AUV5PI_REMOTECONTROLLERCLASS_H

#include "map"
#include "ControlHub.h"
#include "../Postman/UDPSocketBaseClass.h"
#include "../Action/ActionHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Tools/BlockingQueue.h"
#include "../Tools/PIDBaseClass.h"
#include "LogPIDClass.h"


class RemoteControllerClass
{

public:
    RemoteControllerClass();
    ~RemoteControllerClass();

    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    // 机器人运动目标
    ControlHub::RobotTargetType * targetMenu;
    // 机器人限制
    ControlHub::RobotLimitType * limitMenu;

    // 执行机构消息队列
    BlockingQueue<std::map<std::string, float>> * action_queue;
    // 遥控消息队列
    BlockingQueue<std::map<std::string, float>> remote_queue;
    // 运动解算消息队列
    BlockingQueue<std::map<std::string, float>> solution_queue;

    // 遥控循环
    void startRemoteController();

private:
    // 发送间隔  毫秒
    int interval_time = 100;

    int left_propeller_speed = 0;  //左推转速
    int right_propeller_speed = 0; //右推转速
    float thrust; //推力 [-278 ~ 646]
    int torque;   //扭矩 [-152 ~ 152]

    // 遥控线程
    std::thread remote_thread;
    // 遥控执行循环
    static void remoteLoop(void*);

    // 运动解算线程
    std::thread solution_thread;
    // 运动解算循环
    static void solutionLoop(void*);
};


#endif //AUV5PI_REMOTECONTROLLERCLASS_H
