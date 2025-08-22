//
// @Time     : 2024/1/6 17:35
// @Author   : Yang
// @Software : CLion
// PID基础类

#include "PIDBaseClass.h"
#include "chrono"


PIDBaseClass::PIDBaseClass()
{
    kp = 0;                // 比例 参数
    ki = 0;                // 积分 参数
    kd = 0;                // 微分 参数
    sample_time = 1;       // 采样时间
    output_max = 100;      // 最大输出值
    integral_term = 0;     // 积分项
    differential_term = 0; // 微分项
    proportion = 0;        // 比例 结果
    integral = 0;          // 积分 结果
    differential = 0;      // 微分 结果
    last_output_value = 0; // 前一次输出的值
    last_target_value = 0; // 前一次目标值
    last_error = 0;        // 前一次的误差
    last_time = 0;         // 前一次更新数据的时间
    integral_separate = 0; // 积分分离值
    integral_limit = 0;    // 积分饱和值
    integral_num_max = 0;     // 积分最大次数 只保留最近的积分值
    integral_num_total = 0;// 积分当前次数
}
PIDBaseClass::~PIDBaseClass()
{

}

float PIDBaseClass::updateParameter(double get_current_value, double get_target_value,
                       double get_current_time=0)
{
    // 目标值改变 清空参数
    if(get_target_value != last_target_value)
    {
        resetValue();
    }
    // 偏差
    double error = get_target_value - get_current_value;
    // 当前时间
    double current_time = get_current_time;
    if (current_time == 0)
    {
        // 获取当前时间 毫秒
        current_time = std::chrono::system_clock::now().time_since_epoch().count() * 1e-6;
    }
    // 时间间隔
    double delta_time = current_time - last_time;
    if (delta_time >= sample_time)
    {
        if(ki == 0)
        {
            // 积分分离
            if(abs(error) > integral_separate)
            { // 清空积分
                integral_term = 0;
            }
            else if(integral_num_total <  integral_num_max)
            {//不到最大积分数量
                // 正常积分
                integral_term += error ;
                integral_num_total ++;
            }
            else
            {//达到最大积分数量
                // 简化计算方式 去掉一个均值
                integral_term = integral_term / integral_num_total * (integral_num_total - 1) + error;
            }

            // 积分限幅
            if (integral_term < -integral_limit)
            {
                integral_term = -integral_limit;
            }
            else if (integral_term > integral_limit)
            {
                integral_term = integral_limit;
            }
        }

        // 整微分
        differential_term = (error - last_error)/(delta_time / sample_time);

        proportion =   kp * error;
        integral =     ki * integral_term;
        differential = kd * differential_term;

        // 计算输出
        double output = proportion + differential;
        if(integral)
        {
            output += integral;
        }

        if(output > output_max)
        {
            output = output_max;
        }
        if(output < -output_max)
        {
            output = -output_max;
        }

        // 记录时间
        last_time = current_time;
        last_target_value = get_target_value;
        // 记录偏差
        last_error = error;
        // 记录输出值
        last_output_value = output;

        return (float)output;
    }


    // 时间不够一个节拍 输出上一次的值
    return last_output_value;
}

/** 清空参数**/
void PIDBaseClass::resetValue()
{
    cleanIntegral(); //清空积分
    last_error = 0.0; //清空误差
    last_output_value = 0.0; //清空上次输出
}

/** 清空参数**/
void PIDBaseClass::cleanIntegral()
{
    integral_term = 0.0; //清空积分
    integral_num_total = 0; //清空积分累计次数
}

