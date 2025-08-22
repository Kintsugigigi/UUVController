//
// @Time     : 2023/12/11 17:26
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_CONDITIONMASTERCLASS_H
#define AUV_CONDITIONMASTERCLASS_H

#include "map"
#include "thread"
#include "ConditionHub.h"
#include "../Control/ControlHub.h"
#include "../Tools/BlockingQueue.h"


class ConditionMasterClass
{
public:
    ConditionMasterClass();
    ~ConditionMasterClass();

    // 机器人当前模式
    ControlHub::MissionModeType * robotNowMode;
    //电池状态
    ConditionHub::BatteryType batteryMenu;

    // 开启连接
    void startConditionListen();

private:
    // 看门狗 线程
    std::thread watch_dog_thread;
    // 看门狗循环
    static void watchDogLoop(void*);
};


#endif //AUV_CONDITIONMASTERCLASS_H
