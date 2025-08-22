//
// @Time     : 2024/6/1 13:03
// @Author   : Yang
// @Software : CLion
//

#include "RemoteControllerClass.h"


RemoteControllerClass::RemoteControllerClass()
{

}

RemoteControllerClass::~RemoteControllerClass()
{

}

void RemoteControllerClass::startRemoteController()
{
    // 遥控 线程
    remote_thread = std::thread(RemoteControllerClass::remoteLoop, this);

    solution_thread = std::thread(RemoteControllerClass::solutionLoop, this);
}

void RemoteControllerClass::remoteLoop(void * __this)
{
    RemoteControllerClass * _this =(RemoteControllerClass *)__this;

    while (true)
    {
        std::map get_map = _this->remote_queue.take();
        //spdlog::warn("获取了遥控消息");
        if(_this->robotNowMode->mission_mode == ControlHub::ModeCruiseSpeed or
           _this->robotNowMode->mission_mode == ControlHub::ModeRemoteAuto)
        {
            //直接给执行器
            _this->action_queue->push(get_map);
        }
        else if (_this->robotNowMode->mission_mode == ControlHub::ModeRemoteAuto)
        {


        }
        else
        {

        }
    }
}

void RemoteControllerClass::solutionLoop(void * __this)
{
    RemoteControllerClass *_this = (RemoteControllerClass *) __this;

    while (true)
    {
        std::map get_map = _this->solution_queue.take();


    }
}