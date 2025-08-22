//
// @Time     : 2023/11/1 18:52
// @Author   : Yang
// @Software : CLion
// 运动解算模块

#ifndef USVCONTROLLER_MOTIONSOLUTIONCLASS_H
#define USVCONTROLLER_MOTIONSOLUTIONCLASS_H

#include "map"
#include "../Tools/BlockingQueue.h"
#include "../Tools/PIDBaseClass.h"
#include "ControlHub.h"
#include "../Action/ActionHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Postman/UDPSocketBaseClass.h"
#include "LogPIDClass.h"

class MotionSolutionClass
{
public:
    MotionSolutionClass();
    ~MotionSolutionClass();

//    // 当前模式
//    ControlHub::MotionSolutionModeEnum solutionMode;
//    // 运动模块收到的运动目标
//    ControlHub::RobotTargetType solutionTargetMenu;
    // 全驱动PID
    ControlHub::ContolPIDMenuType usvPIDMenu;
    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    // 机器人运动目标
    ControlHub::RobotTargetType * targetMenu;
    // 无人艇导航信息 从无人艇返回的信息
    NavigationHub::USVBackNavigationType * usvNavMenu;

    // 执行机构消息队列
    BlockingQueue<std::map<std::string, float>> * action_queue;
    // 遥控自动姿态机构消息队列
    BlockingQueue<std::map<std::string, float>> * solution_queue;

    // 开始计算循环
    void startMotionSolution();


private:
    // CAN通信发送间隔  毫秒
    int interval_can = 500;
    // 从运动解算 线程
    std::thread comput_thread;
    //mode 改变标识
    int mode_change = 0;

    PIDBaseClass yawPIDClass; //艏向
    PIDBaseClass yawVariablePIDClass; //艏向变化率

    PIDBaseClass speedPIDClass; //速度
    PIDBaseClass speedVariablePIDClass; //速度变化率

    // 推力分配
    void motionSolving();
    // 外环
    int interval_outer = 500; //外环控制周期 毫秒
    // 外环线程
    std::thread outer_thread;
    static void outerRingLoop(void*);
    // 内环
    int interval_inner = 200; //内环控制周期 毫秒
    // 内环线程
    std::thread inner_thread;
    static void innerRingLoop(void*);

    // 处理 向运动解算发送消息
    std::string cookSendMessage();

    // pid输出
    LogPIDClass pidLogClass;
};


#endif //USVCONTROLLER_MOTIONSOLUTIONCLASS_H
