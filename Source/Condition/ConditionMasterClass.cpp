//
// @Time     : 2023/12/11 17:26
// @Author   : Yang
// @Software : CLion
//

#include "ConditionMasterClass.h"
#include <unistd.h>


ConditionMasterClass::ConditionMasterClass()
{

}

ConditionMasterClass::~ConditionMasterClass()
{

}

void ConditionMasterClass::startConditionListen()
{
    // 看门狗 线程
    watch_dog_thread = std::thread(ConditionMasterClass::watchDogLoop, this);
}

void ConditionMasterClass::watchDogLoop(void* __this)
{
    ConditionMasterClass * _this =(ConditionMasterClass *)__this;
    while (true)
    {
        /** 电池状态监控 **/
        if(_this->batteryMenu.voltage < 45) //电压
        {
            _this->robotNowMode->battery_warning = true;
        }
        if(_this->batteryMenu.current > 20) //电流
        {
            _this->robotNowMode->battery_warning = true;
        }
        if(_this->batteryMenu.capacity < 20) //电量
        {
            _this->robotNowMode->battery_warning = true;
        }
        // 增加计数
        sleep(1);
    }
}
