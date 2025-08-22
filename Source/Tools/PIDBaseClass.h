//
// @Time     : 2024/1/6 17:35
// @Author   : Yang
// @Software : CLion
// PID基础类

#ifndef AUV5PI_PIDBASECLASS_H
#define AUV5PI_PIDBASECLASS_H

#include <iostream>

class PIDBaseClass
{

public:
    PIDBaseClass();
    ~PIDBaseClass();

    double kp = 0;                // 比例 参数
    double ki = 0;                // 积分 参数
    double kd = 0;                // 微分 参数
    double sample_time = 1;       // 采样时间
    float  output_max = 100;      // 最大输出值
    double integral_term = 0;     // 积分项
    double differential_term = 0; // 微分项
    double proportion = 0;        // 比例 结果
    double integral = 0;          // 积分 结果
    double differential = 0;      // 微分 结果
    float  last_output_value = 0; // 前一次输出的值
    double last_target_value = 0; // 前一次目标值
    double last_error = 0;        // 前一次的误差
    double last_time = 0;         // 前一次更新数据的时间
    double integral_separate = 0; // 积分分离值
    double integral_limit = 0;    // 积分饱和值
    int integral_num_max = 0;     // 积分最大次数 只保留最近的积分值

    //状态值清空
    void resetValue();
    // 更新参数
    float updateParameter(double, double, double);

private:
    // 清空积分
    void cleanIntegral();
    int integral_num_total = 0;// 积分当前次数
};



#endif //AUV5PI_PIDBASECLASS_H
