//
// @Time     : 2023/11/18 09:12
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_ACTIONTYPE_H
#define AUV_ACTIONTYPE_H

//include "../Tools/ZQCANModel.hpp"
//include "ctime"
#include "atomic"

namespace ActionHub
{
    /** 执行机构的值 **/
    struct ThrusterStatusType
    {
        uint8_t left_propeller_status; //左推状态
        uint8_t left_propeller_rpm;    //左推转速
        uint8_t right_propeller_status;//右推状态
        uint8_t right_propeller_rpm;   //右推转速
        std::atomic<int> read_time;    //距离时长
    };

    struct USVControlType
    {
        float thrust; //推力 [-278 ~ 646]
        float torque;   //扭矩 [-152 ~ 152]
    };

    /** 运动控制信息  扩展项暂时不用**/
    enum ActionSourceEnum
    {
        SourceAction = 0,      //ActionMaster 自身
        SourceWarning = 1,     //警告
        SourceRadioRemote = 2, // 电台遥控 纯手动
        SourceRadioRemoteHold = 3, // 电台遥控 自动航向
        SourceRadioSet = 4,    // 电台设置固定值
        SourceMotionSolution= 5, //运动解算模块
        SourceNet = 6,         //网络
        SourceOther = 7        //其他未知
    };
}

#endif //AUV_ACTIONTYPE_H
