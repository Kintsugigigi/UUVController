//
// @Time     : 2023/12/10 16:40
// @Author   : Yang
// @Software : CLion
//

#include "ActionMasterClass.h"
#include "../Control/ModeTypeTools.hpp"
#include <unistd.h>


ActionMasterClass::ActionMasterClass()
{

}

ActionMasterClass::~ActionMasterClass()
{

}

void ActionMasterClass::startAction()
{
    // 控制执行器线程
    //control_thread = std::thread(ActionMasterClass::controlLoop, this);
    // 控制执行器线程
    //get_control_thread = std::thread(ActionMasterClass::getControlLoop, this);
    // 看门狗 线程
    watch_dog_thread = std::thread(ActionMasterClass::watchDogLoop, this);
}


void ActionMasterClass::cookCommandForMove(const char get_char [], const int & len)
{

}

void ActionMasterClass::cookCommandForLED(const char get_char [], const int & len)
{

}

void ActionMasterClass::getControlLoop(void * __this)
{
    ActionMasterClass * _this =(ActionMasterClass *)__this;
    while (true)
    {
        std::map get_map = _this->action_queue.take();

        //std::lock_guard<std::mutex> lock(_this->mutex_action_cout);
        //机器人警告超时
        if(_this->robotNowMode->warning_type == ControlHub::WarningLonely)
        {
            printf("Action 机器人通信超时");
            continue;
        }

        //机器人越界
        if(_this->robotNowMode->warning_type == ControlHub::WarningLimit)
        {
            if(_this->robotNowMode->mission_mode != ControlHub::ModeTest)
            {
                printf("Action 机器人越界");
                continue;
            }
        }

        //消息源
        int source = (int)get_map["source"];
        if (source == ActionHub::SourceMotionSolution and
                ModeTypeTools::needSolution(_this->robotNowMode->mission_mode))
        {
            //运动解算
            float thrust = get_map["thrust"];
            float torque = get_map["torque"];
            int left_speed = (int)get_map["left_speed"];
            int right_speed = (int)get_map["right_speed"];

            printf("-- action 收到运动解算数据: 推力:{0:}  转矩:{1:} 左推转速:{2:} 右推转速:{3:} --",
                         thrust, torque, left_speed, right_speed);

            // 赋值给通讯模块
            _this->usvControlMenu.thrust = thrust;
            _this->usvControlMenu.torque = torque;
        }
    }
}


void ActionMasterClass::watchDogLoop(void* __this)
{
    ActionMasterClass * _this =(ActionMasterClass *)__this;
    while (true)
    {
        //看门狗计时
        _this->robotNowMode->dog_time += 1;
        if (_this->robotNowMode->dog_time >=2)//遥控超时
        {
            if(_this->robotNowMode->mission_mode == ControlHub::ModeRemoteBase or
               _this->robotNowMode->mission_mode == ControlHub::ModeRemoteAuto)
            {
                // 停止推进器
                _this->stopSet();
                printf("**机器人遥控超时**停止推进器**");
            }
        }

        //机器人停止模式
        if(_this->robotNowMode->mission_mode == ControlHub::ModeNoIdea or
            _this->robotNowMode->mission_mode == ControlHub::ModeStop)
        {
            // 停止推进器
            _this->stopSet();
            printf("**机器人停止模式**停止推进器**");
        }

        //机器人警告超时
        if(_this->robotNowMode->warning_type == ControlHub::WarningLonely)
        {
            // 停止推进器
            _this->stopSet();
            printf("**机器人超时**停止推进器**");
            // 超时进入停止模式
            if (_this->robotNowMode->mission_mode != ControlHub::ModeStop and
                _this->robotNowMode->mission_mode != ControlHub::ModeNoIdea)
            {
                _this->robotNowMode->mission_mode = ControlHub::ModeStop;
            }
        }

        //机器人越界
        if(_this->robotNowMode->warning_type == ControlHub::WarningLimit)
        {
            if(_this->robotNowMode->mission_mode != ControlHub::ModeRemoteBase and
               _this->robotNowMode->mission_mode != ControlHub::ModeTest)
            {
                // 停止推进器
                _this->stopSet();
                printf("**机器人越界** 停止推进器**");
                if (_this->robotNowMode->mission_mode != ControlHub::ModeStop and
                    _this->robotNowMode->mission_mode != ControlHub::ModeNoIdea)
                {
                    _this->robotNowMode->mission_mode = ControlHub::ModeStop;
                }
            }
        }
        sleep(1);
    }
}

/** 停止所有推进器 **/
void ActionMasterClass::stopSet()
{
    // 归零
    usvControlMenu.thrust = 0;
    usvControlMenu.torque = 0;
}





