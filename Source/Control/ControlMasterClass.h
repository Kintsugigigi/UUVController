//
// @Time     : 2024/6/5 19:31
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_CONTROLMASTERCLASS_H
#define AUV5PI_CONTROLMASTERCLASS_H

#include "vector"
#include "ControlHub.h"
#include "../Navigation/NavigationHub.h"
#include "MotionSolutionClass.h"
#include "RemoteControllerClass.h"

class ControlMasterClass
{public:
    ControlMasterClass();
    ~ControlMasterClass();
    // 机器人当前模式
    ControlHub::MissionModeType robotNowMode;
    // 机器人前一时刻模式
    ControlHub::MissionModeType robotLastMode;
    // 机器人限制
    ControlHub::RobotLimitType limitMenu;
    // 机器人运动目标
    ControlHub::RobotTargetType targetMenu;

    // 位置信息 队列
    BlockingQueue<std::map<std::string, double>>  path_queue;

    // 无人艇姿态位置 从无人艇返回的信息
    NavigationHub::USVBackNavigationType * usvNavMenu;
    // 遥控消息队列
    BlockingQueue<std::map<std::string, float>> * remote_queue;
    // 执行机构消息队列
    BlockingQueue<std::map<std::string, float>> * action_queue;

    // 遥控类
    RemoteControllerClass remoteControllerClass;
    // 运动解算
    MotionSolutionClass motionSolutionClass;

    // 开启连接
    void startController();

private:
    // 发送间隔  毫秒
    int interval_num = 100;
    // 看门狗 线程
    std::thread watch_dog_thread;
    // 看门狗循环
    static void watchDogLoop(void*);

    // 路径 线程
    std::thread path_thread;
    // 路径 跟踪循环
    static void pathLoop(void*);

    // 获取路径 线程
    std::thread get_mission_thread;
    // 获取路径循环
    static void getMissionLoop(void*);
};


#endif //AUV5PI_CONTROLMASTERCLASS_H
