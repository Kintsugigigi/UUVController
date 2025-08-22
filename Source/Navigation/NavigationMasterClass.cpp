//
// @Time     : 2023/12/11 22:13
// @Author   : Yang
// @Software : CLion
//

#include "NavigationMasterClass.h"
#include <string>
#include "unistd.h"

NavigationMasterClass::NavigationMasterClass()
{

}

NavigationMasterClass::~NavigationMasterClass()
{

}

void NavigationMasterClass::startNavigationDevice()
{
    // 看门狗 线程
    watch_dog_thread = std::thread(NavigationMasterClass::watchDogLoop, this);
}

void NavigationMasterClass::watchDogLoop(void * __this)
{
    NavigationMasterClass * _this =(NavigationMasterClass *)__this;
    while (true)
    {
        /** 惯导 状态更新 **/
        //_this->insClass.insMenu.dog_time += 1;
        //if (_this->insClass.insMenu.dog_time >=2)
        //{   //标识为设备失联
        //    _this->insClass.insMenu.status_type = 2;
        //}
        _this->usvNavMenu.dog_time += 1;
        if (_this->usvNavMenu.dog_time >=2)
        {   //标识为设备失联
            _this->usvNavMenu.status_type = 2;
        }

        sleep(1);
    }
}
