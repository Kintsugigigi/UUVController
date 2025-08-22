//
// @Time     : 2023/11/11 10:28
// @Author   : Yang
// @Software : CLion
// 机器人模式判断

#ifndef AUVCONTROLLER_MODETYPETOOLS_HPP
#define AUVCONTROLLER_MODETYPETOOLS_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include "assert.h"
#include "../Control/ControlHub.h"

namespace ModeTypeTools
{
    // 是否需要运动解算
    inline bool needSolution(ControlHub::RobotModeEnum mission_mode)
    {
        if(
           mission_mode == ControlHub::ModeBackHome or
           mission_mode == ControlHub::ModeRemoteBase or
           mission_mode == ControlHub::ModeRemoteAuto or
           mission_mode == ControlHub::ModeCruiseSpeed or
           mission_mode == ControlHub::ModePathPoint or
           mission_mode == ControlHub::ModePathLoop )
        {
            // 需要运动解算
            return true;
        }
        else
        {   // 不需要运动解算的模式
            return false;
        }
    }
}

#endif //AUVCONTROLLER_MODETYPETOOLS_HPP