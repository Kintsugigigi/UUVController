//
// @Time     : 2023/5/2 18:46
// @Author   : Yang
// @Software : CLion
// 航道无人艇测试程序

#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include "ctime"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "Navigation/NavigationMasterClass.h"
#include "Condition/ConditionMasterClass.h"
#include "Control/ControlMasterClass.h"
#include "Action/ActionMasterClass.h"
#include "Communication/CommunicationMasterClass.h"
#include "Tools/GISTools.hpp"
#include "Tools/LogToTextClass.h"
#include "Tools/DataFormatTools.hpp"

int main()
{
    sleep(5);

    /** 文件名加时间 **/
    time_t tm;   //time_t是time.h库中定义的数据类型，用以表示时间
    time(&tm);   //time函数用以获取当前的时间，并存储在time_t类型的变量中
    char tmp[64];
    strftime(tmp, sizeof(tmp),"%Y_%m_%d_%H_%M_%S", localtime(&tm));
    std::string time_str(tmp);
    std::string file_name = "/home/auv/Desktop/auv/Tmp/LogText/";
    file_name = file_name + time_str + "log.txt";
    /** logger **/
     auto logger = spdlog::basic_logger_mt("loggerMain",file_name);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);

    /** 运动控制与规划类 **/
    ControlMasterClass controlMasterClass;

    /** 导航相关信息 **/
    NavigationMasterClass navigationMaster;
    navigationMaster.startNavigationDevice();

    /** 执行机构驱动类 **/
    ActionMasterClass actionMasterClass;
    actionMasterClass.robotNowMode = &controlMasterClass.robotNowMode;
    actionMasterClass.limitMenu = &controlMasterClass.limitMenu;
    actionMasterClass.startAction();

    /** 自身状态信息 电池 **/
    //ConditionMasterClass conditionMasterClass;
    //conditionMasterClass.robotNowMode = &controlMasterClass.robotNowMode; //USV模式
    //conditionMasterClass.startConditionListen();

    /** 运动控制与规划类 启动**/
    controlMasterClass.usvNavMenu = &navigationMaster.usvNavMenu;      // USV导航信息
    controlMasterClass.action_queue = &actionMasterClass.action_queue; // 执行机构 队列
    controlMasterClass.startController();

    /** 通信类 **/
    CommunicationMasterClass commMasterClass;
    commMasterClass.robotNowMode = &controlMasterClass.robotNowMode; // 模式
    commMasterClass.limitMenu = &controlMasterClass.limitMenu;       // 限制
    commMasterClass.targetMenu = &controlMasterClass.targetMenu;     // 执行目标
    commMasterClass.remote_queue = controlMasterClass.remote_queue;  // 遥控消息
    commMasterClass.path_queue = &controlMasterClass.path_queue;     // 任务路径点
    commMasterClass.usvNavMenu = &navigationMaster.usvNavMenu;       // USV导航信息
    commMasterClass.usvControlMenu = &actionMasterClass.usvControlMenu; //控制指令 发送给USV主控用
    commMasterClass.usvPropellerStatusMenu = &actionMasterClass.usvPropellerStatusMenu; //推进器状态
    commMasterClass.startConnect();

//    /** Log 文件 **/
//    LogToTextClass logClass;
//    logClass.dvlMenu = &navigationMaster.dvlClass.dvlMenu;
//    logClass.insMenu = &navigationMaster.insClass.insMenu;
//    logClass.depthMenu = &navigationMaster.depthometerClass.depthMenu;
//    logClass.GPSMenu = &navigationMaster.gpsClass.GPSMenu;
//    logClass.soundMenu = &commMasterClass.soundClass.soundMenu;
//    logClass.startLog();

    while (true)
    {

        if(navigationMaster.usvNavMenu.status_type == 1)
        {
            spdlog::info("经度:({0}) 纬度:({1}) 艏向:({2}) 横摇:({3}) 纵摇:({4}) 转艏速度:({5}) 东速:({6}) 北速:({7})) ",
                         navigationMaster.usvNavMenu.lon, navigationMaster.usvNavMenu.lat,
                         navigationMaster.usvNavMenu.yaw, navigationMaster.usvNavMenu.roll, navigationMaster.usvNavMenu.pitch,
                         navigationMaster.usvNavMenu.yaw_roti,navigationMaster.usvNavMenu.east_speed, navigationMaster.usvNavMenu.north_speed);

            spdlog::info("期望经度:({0}) 期望纬度:({1}) 位置偏差:({2})  期望速度:{4} \r\n 期望艏向:({5}) 期望俯仰:({6})) 期望横滚:({7}) ",
                         controlMasterClass.targetMenu.lon, controlMasterClass.targetMenu.lat,
                         YGISTools::pointToDistance(navigationMaster.usvNavMenu.lon,navigationMaster.usvNavMenu.lat,
                                             controlMasterClass.targetMenu.lon, controlMasterClass.targetMenu.lat),
                         controlMasterClass.targetMenu.yaw, controlMasterClass.targetMenu.speed);
        }
        else
        {
            spdlog::info("USV主控掉线");
        }



        spdlog::info("机器人运动模式:({0:d}) 机器人警告:({1:d})",
                     controlMasterClass.robotNowMode.mission_mode, controlMasterClass.robotNowMode.warning_type);

        sleep(1);
    }
}
