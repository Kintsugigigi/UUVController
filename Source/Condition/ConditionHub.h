//
// @Time     : 2023/10/10 10:32
// @Author   : Yang
// @Software : CLion
// 电源板状态

#ifndef USVCONTROLLER_POWERHUB_H
#define USVCONTROLLER_POWERHUB_H

//#pragma once

#include <cstdint>
#include "mutex"
#include "ctime"
#include "atomic"
#include "../Tools/ZQCANModel.hpp"
#include "cstring"

namespace ConditionHub
{
    /** 自身状态命令 模板 **/
    struct ConditionCommandType
    {
        // 接收的命令
        unsigned char command_char_ary[10];
        // 命令接收状态
        std::atomic_uint8_t read_time;
    };

    /** 电池信息 **/
    struct BatteryType
    {
        uint8_t status_type;   //0:无数据 1:正常 2:设备失联 3:设备异常
        unsigned int dog_time; //失联计数

        int16_t voltage; //电压
        int16_t current; //电流
        int16_t capacity; //电量
    };

    /** 开关控制信息 来源**/
    enum SwitchSourceEnum
    {
        SourceCondition = 0,   //自身
        SourceWarning = 1,     //警告
        SourceRadio = 2,       //电台
        SourceSound = 3,       //水声通讯
        SourceNet = 4,         //网络
        SourceOther = 5        //其他未知
    };
}
//#pragma pack()

#endif //USVCONTROLLER_POWERHUB_H
