//
// @Time     : 2023/9/9 13:21
// @Author   : Yang
// @Software : CLion
// 电台解析

#ifndef AUV_RadioReadCooker_HPP
#define AUV_RadioReadCooker_HPP

#include "cstring"
#include "string"
#include "map"
#include "../Tools/CRCTools.hpp"
#include "../Postman/UDPSocketBaseClass.h"
#include "../Action/ActionHub.h"
#include "../Condition/ConditionHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Control/ControlHub.h"
#include <utility>
#include<arpa/inet.h>
#include "map"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

namespace MsgReadCooker
{
    // 消息头 CC ascii
    inline unsigned char head_char_ary[2] = {0x43, 0x43};

    // 读取上位机消息
    inline void cookGetPCMessage(const char * get_char,
                                        ControlHub::MissionModeType * robotNowMode,
                                        ControlHub::RobotTargetType * targetMenu,
                                        ControlHub::ContolPIDMenuType *usvPIDMenu)
    {
        // 键值对 容器
        rapidjson::Document get_doc;
        // 解析数据到doc容器里  默认 UTF-8
        get_doc.Parse(get_char);

        // 转化结果
        if (!get_doc.IsObject())
        {
            printf("json 解析失败 \n");
            return;
        }

        // 接收运动模式
        if (get_doc.HasMember("status"))
        {
            uint8_t get_mode = get_doc["status"].GetInt();
            robotNowMode->mission_mode = (ControlHub::RobotModeEnum)(get_mode);
            //printf("运动解算反馈模式:{0:d} - {1:d} -",get_mode, solutionMode);
        }

        // 接收目标艏向
        if (get_doc.HasMember("targetYaw"))
        {
            float get_yaw = get_doc["targetYaw"].GetFloat();
            targetMenu->yaw = get_yaw;
        }

        // 接收目标速度
        if (get_doc.HasMember("targetSpeed"))
        {
            float get_speed = get_doc["targetSpeed"].GetFloat();
            targetMenu->speed = get_speed;
        }

        // 接收目标经度
        if (get_doc.HasMember("targetLon"))
        {
            double get_lon = get_doc["targetLon"].GetDouble();
            targetMenu->lon = get_lon;
        }
        // 接收目标纬度
        if (get_doc.HasMember("targetLat"))
        {
            double get_lat = get_doc["targetLat"].GetDouble();
            targetMenu->lat = get_lat;
        }
        //接收航向PID参数
        if (get_doc.HasMember("yawKp"))
        {
            double kp = get_doc["yawKp"].GetDouble();
            usvPIDMenu->yaw_kp=kp;
        }
        if (get_doc.HasMember("yawKi"))
        {
            double ki = get_doc["yawKi"].GetDouble();
            usvPIDMenu->yaw_ki = ki;
        }
        if (get_doc.HasMember("yawKd"))
        {
            double kd = get_doc["yawKd"].GetDouble();
            usvPIDMenu->yaw_kd = kd;
        }
        //接收航速PID参数
        if (get_doc.HasMember("speedKp"))
        {
            double kp = get_doc["speedKp"].GetDouble();
            usvPIDMenu->speed_kp = kp;
        }
        if (get_doc.HasMember("speedKi"))
        {
            double ki = get_doc["speedKi"].GetDouble();
            usvPIDMenu->speed_ki = ki;
        }
        if (get_doc.HasMember("speedKd"))
        {
            double kd = get_doc["speedKd"].GetDouble();
            usvPIDMenu->speed_kd = kd;
        }


        // 解析路径数组
        const rapidjson::Value& line_ary = get_doc["targetLineAry"];
        if (line_ary.IsObject())
        {
            // 加入其中
            for (rapidjson::SizeType i = 0; i < line_ary.Size(); i++)
            {
                const rapidjson::Value& point_ary = line_ary[i];

                //std::cout << "Target Line Point " << i << ": Lon " << point_ary[0].GetDouble()
                //          << ", Lat " << point_ary[1].GetDouble() << std::endl;
                targetMenu->mission_path_ary[i][0] = point_ary[0].GetDouble();
                targetMenu->mission_path_ary[i][1] = point_ary[1].GetDouble();
                targetMenu->mission_path_num = i;
            }
        }
    }
}



#endif //AUV_RadioReadCooker_HPP