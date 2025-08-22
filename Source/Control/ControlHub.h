//
// @Time     : 2023/12/10 13:33
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_CONTROLHUB_H
#define AUV_CONTROLHUB_H

#include "string"
#include "atomic"

namespace ControlHub
{
    /** 机器人状态类型 **/
    enum RobotModeEnum:uint8_t
    {
        ModeNoIdea      = 0, //等待
        ModeStop        = 1, //停止
        ModeBackHome    = 2, //回程
        ModeRemoteBase  = 3, //遥控
        ModeRemoteAuto  = 4, //遥控 航向航速
        ModeCruiseSpeed = 5, //定速巡航 航向航速
        ModePathPoint   = 6, //路径 不循环
        ModePathLoop    = 7, //路径 循环
        ModeTest        = 8, //测试模式
    };

    /** 机器人状态类型 **/
    enum RobotWarningEnum:uint8_t
    {
        WarningNormal     = 0x00, //正常
        WarningDivce      = 0x01, //重要设备异常
        WarningProgram    = 0x02, //程序模块异常 停止任务
        WarningLimit      = 0x03, //越界 俯仰横滚位置
        WarningPower      = 0x04, //电量低
        WarningLonely     = 0x05, //通信超时
    };

    /** 任务模式 **/
    struct MissionModeType
    {
        RobotModeEnum mission_mode;    //机器人当前模式
        RobotWarningEnum warning_type; //机器人警告
        uint8_t mode_level;            //模式进度
        bool limit_warning;            //越界警告
        bool battery_warning;          //电量警告
        bool speed_warning;            //超速警告
        bool posture_warning;          //姿态警告
        std::atomic<unsigned int> lonely_time; //失联时间
        std::atomic<int> dog_time;     //状态看门狗
    };

    /** 机器人目标参数 **/
    struct RobotTargetType
    {
        double lon;     //期望经度
        double lat;     //期望纬度
        float yaw;      //期望艏向
        float speed;    //期望速度 m/s
        unsigned int lossTime;      //任务时间 秒
        unsigned int missionTime;   //已经执行的时间
        float solution_dis_l; //偏差
        float solution_dis_h; //横向偏差
        double los_lon;       //视线经度
        double los_lat;       //视线纬度

        float control_yaw;    //控制目标艏向
        float control_speed;  //控制目标速度
        double control_lon;    //控制目标经度
        double control_lat;    //控制目标经度

        //路径经纬度点
        double path_lon;
        double path_lat;
        int path_num;   //当前路径点的总数
        int path_index; //当前执行点
        int solution_path_finish; //路径结束
        int solution_point_index; //路径当前跟踪点

        // 任务路径数组
        double mission_path_ary[600][2];
        unsigned int mission_path_num; //任务路径收到的目标点个数
        unsigned int mission_target_num; //任务路径 当前执行点
        // 返航的路径数组
        double back_path_ary[100][2];
        unsigned int back_path_num; //返航路径收到的目标点个数
        unsigned int back_target_num; //返航路径 当前执行点
    };



    /** 机器人限制 **/
    struct RobotLimitType
    {
        float yaw;      //艏向限制
        float pitch;    //俯仰限制 °
        float roll;     //滚动限制 °
        float speed;    //速度限制 m/s
        float altitude; //高度限制 m
        float right_thruster;  //左推限制 油门
        float left_thruster;   //右推限制 油门
        std::atomic<unsigned int>  lossTime; //超时时间 秒
    };

    /** PID控制参数 **/
    struct ContolPIDMenuType
    {
        // 艏向控制
        float yaw_speed;  //转艏速度
        float yaw_scale;  //推进器油门偏差

        // 纵向速度控制
        float speed_f_acceleration;  //纵向加速度
        float drive_f_scale;         //推进器总油门

        int level_time; //当前模式时长
        unsigned int dog_time;

        double yaw_kp;
        double yaw_ki;
        double yaw_kd;//航速PID参数

        double speed_kp;
        double speed_ki;
        double speed_kd;//航向PID参数


    };
}

#endif //AUV_CONTROLHUB_H

