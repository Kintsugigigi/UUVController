//
// @Time     : 2024/6/6 17:39
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_LOSCLASS_H
#define AUV5PI_LOSCLASS_H

#include "map"
#include "../Tools/BlockingQueue.h"
#include "../Tools/PIDBaseClass.h"
#include "ControlHub.h"
#include "../Action/ActionHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Postman/UDPSocketBaseClass.h"
#include "LogPIDClass.h"

class LOSClass
{

public:
    LOSClass();
    ~LOSClass();

    // 无人艇导航信息 从无人艇返回的信息
    NavigationHub::USVBackNavigationType * usvNavMenu;
    // 机器人运动目标
    ControlHub::RobotTargetType * targetMenu;
    // 运动解算当前目标
    ControlHub::RobotTargetType * controlMenu;

    // 判断到达点的半径
    float point_radius;
    // 载体转弯半径
    float turning_radius;

    // 开始计算循环
    void startLosCompute(float, float, double, double, int);

    //输出目标航向
    float target_yaw;

    //目标经纬度
    double target_lon;
    double target_lat;
    //前一次目标经纬度
    double last_lon;
    double last_lat;
    //前视点
    double los_lon;
    double los_lat;

    // 跟踪点序号
    int point_index;

private:

    // 计算一次
    void computeOnce(float, float, double, double, double, double, double, double);

};


#endif //AUV5PI_LOSCLASS_H
