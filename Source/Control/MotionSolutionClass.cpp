//
// @Time     : 2023/11/1 18:52
// @Author   : Yang
// @Software : CLion
// 运动解算模块

#include "MotionSolutionClass.h"
#include "rapidjson/reader.h"
#include <utility>
#include<arpa/inet.h>
#include "map"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "../Tools/ConfigTools.hpp"
#include "../Tools/TimeTools.hpp"
#include "iomanip"

MotionSolutionClass::MotionSolutionClass()
{
    usvPIDMenu.yaw_scale = 0; //转矩油门
    usvPIDMenu.drive_f_scale = 0; //前进油门
}

MotionSolutionClass::~MotionSolutionClass()
{
    //清理
}

/** 启动运动解算**/
void MotionSolutionClass::startMotionSolution()
{
    /** PID参数配置 **/
    std::map<std::string, std::map<std::string, std::string>> config_map = YConfigTools::parseIniFile("../Conf/PIDConfig.ini");
    std::map<std::string, std::string> pid_map = config_map["PID"];
    // 艏向
    //std::string yaw_p = pid_map["yawP"];
    //std::string yaw_i = pid_map["yawI"];
    //std::string yaw_d = pid_map["yawD"];
    yawPIDClass.kp = usvPIDMenu.yaw_kp;
    yawPIDClass.ki = usvPIDMenu.yaw_ki;
    yawPIDClass.kd = usvPIDMenu.yaw_kd;
    //yawPIDClass.kp = stof(yaw_p);
    //yawPIDClass.ki = stof(yaw_i);
    //yawPIDClass.kd = stof(yaw_d);
    //printf("+++ 艏向参数 {0} - {1} - {2}", "yawP", "yawI", "yawD");
    yawPIDClass.sample_time = interval_outer; //外环采样周期
    yawPIDClass.output_max = 10;

//    // 艏向变化率
//    std::string yaw_variable_p = pid_map["yawVariableP"];
//    std::string yaw_variable_i = pid_map["yawVariableI"];
//    std::string yaw_variable_d = pid_map["yawVariableD"];
//    yawVariablePIDClass.kp = stof(yaw_variable_p);
//    yawVariablePIDClass.ki = stof(yaw_variable_i);
//    yawVariablePIDClass.kd = stof(yaw_variable_d);
//    printf("+++ 艏向变化参数 {0} - {1} - {2}", yaw_variable_p, yaw_variable_i, yaw_variable_d);
//    yawVariablePIDClass.sample_time = interval_inner; //内环采样周期 毫秒
//    yawVariablePIDClass.output_max = 100;
//    yawVariablePIDClass.integral_num_max = 20; //累计20个节拍
//    yawVariablePIDClass.integral_limit = 50;   //积分限幅到50%油门
//    yawVariablePIDClass.integral_separate = 10; //10度 积分分离

    // 速度
    //std::string speed_p = pid_map["speedP"];
    //std::string speed_i = pid_map["speedI"];
    //std::string speed_d = pid_map["speedD"];
    //printf("+++ 速度参数 {0} - {1} - {2}", speed_p, speed_i, speed_d);
    speedPIDClass.kp = usvPIDMenu.speed_kp;
    speedPIDClass.ki = usvPIDMenu.speed_ki;
    speedPIDClass.kd = usvPIDMenu.speed_kd;
    speedPIDClass.sample_time = interval_outer; //外环采样周期
    speedPIDClass.output_max = 50;
//    // 速度变化率
//    std::string speed_variable_p = pid_map["speedVariableP"];
//    std::string speed_variable_i = pid_map["speedVariableI"];
//    std::string speed_variable_d = pid_map["speedVariableD"];
//    printf("+++ 速度变化参数 {0} - {1} - {2}", speed_variable_p, speed_variable_i, speed_variable_d);
//    speedPIDClass.kp = stof(speed_variable_p);
//    speedPIDClass.ki = stof(speed_variable_i);
//    speedPIDClass.kd = stof(speed_variable_d);
//    speedPIDClass.sample_time = interval_outer; //外环采样周期
//    speedPIDClass.output_max = 50;

    // 运动解算 线程
    //comput_thread = std::thread(MotionSolutionClass::computLoop, this);
//    // 内环 线程
//    inner_thread = std::thread(MotionSolutionClass::innerRingLoop, this);
    // 外环 线程
    outer_thread = std::thread(MotionSolutionClass::outerRingLoop, this);
}


void MotionSolutionClass::outerRingLoop(void * __this)
{
    MotionSolutionClass * _this =(MotionSolutionClass *)__this;

    while (true)
    {
        // 获取当前时间
        double current_time = YTimeTools::getCurrentTime();

        // 艏向偏差
        float deviation_yaw = _this->usvNavMenu->yaw - _this->targetMenu->yaw;
        // 转化为 +180 ~ -180
        if(deviation_yaw > 180)
        {
            deviation_yaw = deviation_yaw - 360;
        }
        if(deviation_yaw < -180)
        {
            deviation_yaw = deviation_yaw + 360;
        }

        // 艏向 映射 左右转动油门
        _this->usvPIDMenu.yaw_scale = _this->yawPIDClass.updateParameter(_this->usvNavMenu->yaw, _this->targetMenu->yaw, current_time);
        printf("当前艏向:{0} 目标艏向:{1} 艏向pid目标:{2} 艏向偏差:{3} 艏向输出:{4} 艏向p项:{5} 艏向i项:{6} 艏向d项:{7} 艏向p值:{8} 艏向i值:{9} 艏向d值:{10}",
                     _this->usvNavMenu->yaw, _this->targetMenu->yaw,_this->yawPIDClass.last_target_value,
                     _this->yawPIDClass.last_error, _this->yawPIDClass.last_output_value,
                     _this->yawPIDClass.last_error, _this->yawPIDClass.integral_term,
                     _this->yawPIDClass.differential_term, _this->yawPIDClass.proportion,
                     _this->yawPIDClass.integral, _this->yawPIDClass.differential);

        // 速度 映射 前进油门
        _this->usvPIDMenu.drive_f_scale = _this->speedPIDClass.updateParameter(_this->usvNavMenu->fore_speed, _this->targetMenu->speed, current_time);
        printf("当前速度:{0} 目标速度:{1} 速度pid目标:{2} 速度偏差:{3} 速度输出:{4} 速度p项:{5} 速度i项:{6} 速度d项:{7} 速度p值:{8} 速度i值:{9} 速度d值:{10}",
                     _this->usvNavMenu->fore_speed, _this->targetMenu->speed,_this->speedPIDClass.last_target_value,
                     _this->speedPIDClass.last_error, _this->speedPIDClass.last_output_value,
                     _this->speedPIDClass.last_error, _this->speedPIDClass.integral_term,
                     _this->speedPIDClass.differential_term, _this->speedPIDClass.proportion,
                     _this->speedPIDClass.integral, _this->speedPIDClass.differential);

        // 分配推进器
        _this->motionSolving();

        // 节拍间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_outer));
    }
}

void MotionSolutionClass::innerRingLoop(void * __this)
{
    MotionSolutionClass * _this =(MotionSolutionClass *)__this;

    while (true)
    {
        // 遥控自动姿态
        if(_this->robotNowMode->mission_mode == ControlHub::ModeRemoteAuto)
        {
            // 获取当前时间
            double current_time = YTimeTools::getCurrentTime();

            // 艏向 角速度控制
            _this->usvPIDMenu.yaw_scale = -_this->yawVariablePIDClass.updateParameter(_this->usvNavMenu->yaw_roti, _this->usvPIDMenu.yaw_speed, current_time);
            printf("艏向比例:{0}",_this->usvPIDMenu.yaw_scale);



            printf("艏向变化当前:{0} 目标:{1} pid目标:{2} 偏差:{3} 输出:{4} p项:{5} i项:{6} d项:{7} p值:{8} i值:{9} d值:{10}",
                         _this->usvNavMenu->yaw_roti, _this->usvPIDMenu.yaw_speed,_this->yawVariablePIDClass.last_target_value,
                         _this->yawVariablePIDClass.last_error, _this->yawVariablePIDClass.last_output_value,
                         _this->yawVariablePIDClass.last_error, _this->yawVariablePIDClass.integral_term,
                         _this->yawVariablePIDClass.differential_term, _this->yawVariablePIDClass.proportion,
                         _this->yawVariablePIDClass.integral, _this->yawVariablePIDClass.differential);


            /** 输出PID参数到Log文件 **/
            if (_this->pidLogClass.logfile.is_open())
            {
                // 获取当前时间
                time_t now = time(nullptr);
                char * timeString = ctime(&now);
                // 去掉换行符
                timeString[strcspn(timeString, "\n")] = '\0';
                // 写入日志信息
                _this->pidLogClass.logfile<< std::fixed << std::setprecision(8) <<
                                           timeString  <<  "," <<
                                          _this->usvNavMenu->yaw << "," <<
                                          _this->targetMenu->yaw<< "," <<
                                          _this->yawPIDClass.last_target_value << "," <<
                                          _this->yawPIDClass.last_output_value << "," <<
                                          _this->yawPIDClass.proportion << "," <<
                                          _this->yawPIDClass.integral_term << "," <<
                                          _this->yawPIDClass.differential_term << "," <<
                                          _this->yawPIDClass.proportion << "," <<
                                          _this->yawPIDClass.integral << "," <<
                                          _this->yawPIDClass.differential << "," <<
                                          std::endl;
            }
            else
            {
                printf("打开记录文件失败");
            }
        }

        // 纵向速度 加速度

        // 横向速度 加速度

        // 节拍间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_inner));
    }
}

/** 推力分配 **/
void MotionSolutionClass::motionSolving()
{
    // 判断是否需要运动解算
    if(robotNowMode->mission_mode != ControlHub::ModeRemoteAuto and
       robotNowMode->mission_mode != ControlHub::ModeCruiseSpeed and
       robotNowMode->mission_mode != ControlHub::ModePathPoint and
       robotNowMode->mission_mode != ControlHub::ModePathLoop)
    {
        return;
    }

    // 转速目前控制不了
    int left_speed = 0; // 左推转速
    int right_speed = 0; //右推转速


    float torque;   //扭矩 [-152 ~ 152]
    torque = usvPIDMenu.yaw_scale / 100 * 152;

    float thrust; //推力 [-278 ~ 646]
    if(usvPIDMenu.drive_f_scale > 0)
    {
        thrust = usvPIDMenu.drive_f_scale / 100 * 646;
    }
    else
    {
        thrust = usvPIDMenu.drive_f_scale / 100 * 278;
    }

    // 发送给执行机构
    std::map<std::string, float> action_map = {};
    int source = ActionHub::SourceMotionSolution;
    action_map.insert(std::make_pair("source", (float)source));
    action_map.insert(std::make_pair("thrust", thrust));
    action_map.insert(std::make_pair("torque", torque));
    action_map.insert(std::make_pair("left_speed", left_speed));
    action_map.insert(std::make_pair("right_speed", right_speed));

    // 发送到执行器
    solution_queue->push(action_map);
}