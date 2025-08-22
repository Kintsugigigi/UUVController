//
// @Time     : 2023/9/9 13:21
// @Author   : Yang
// @Software : CLion
// 电台解析

#ifndef AUV_RadioWriterCooker_HPP
#define AUV_RadioWriterCooker_HPP

#include "cstring"
#include "../Tools/CRCTools.hpp"
#include "../Postman/UDPSocketBaseClass.h"
#include "../Action/ActionHub.h"
#include "../Condition/ConditionHub.h"
#include "../Navigation/NavigationHub.h"
#include "../Control/ControlHub.h"
#include "spdlog/spdlog.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

namespace MsgWriterCooker
{
    //消息头 CC ascii
    inline unsigned char head_char_ary[2] = {0x43, 0x43};

    // 无人艇状态信息
    inline std::string cookSendPCStatusMessage(ControlHub::MissionModeType * robotNowMode,
                                         NavigationHub::USVBackNavigationType * usvNavMenu,
                                         ControlHub::RobotTargetType * targetMenu,
                                         ActionHub::ThrusterStatusType * usvPropellerStatusMenu,
                                         ControlHub::ContolPIDMenuType* usvPIDMenu)
    {

        /** 发送的数据 **/
        // string buffer对象
        rapidjson::StringBuffer send_buffer;
        // 编辑内容对象
        rapidjson::Writer<rapidjson::StringBuffer,rapidjson::Document::EncodingType,rapidjson::ASCII<>> send_writer(send_buffer);
        // 加入括号
        send_writer.StartObject();

        // 机器人模式
        send_writer.Key("status");
        send_writer.Int(robotNowMode->mission_mode);
        // 机器人警告
        send_writer.Key("waring");
        send_writer.Int(robotNowMode->warning_type);
        // 机器人当前经度
        send_writer.Key("lon");
        send_writer.Double(usvNavMenu->lon);
        // 机器人当前纬度
        send_writer.Key("lat");
        send_writer.Double(usvNavMenu->lat);
        // 当前艏向
        send_writer.Key("yaw");
        send_writer.Double(usvNavMenu->yaw);
        // 当前俯仰
        send_writer.Key("pitch");
        send_writer.Double(usvNavMenu->pitch);
        // 当前横滚
        send_writer.Key("roll");
        send_writer.Double(usvNavMenu->roll);
        // 纵向速度
        send_writer.Key("fSpeed");
        send_writer.Double(usvNavMenu->fore_speed);
        // 横向速度
        send_writer.Key("hSpeed");
        send_writer.Double(usvNavMenu->starboard_speed);
        // 左推转速
        send_writer.Key("leftRPM");
        send_writer.Int(usvPropellerStatusMenu->left_propeller_rpm);
        // 右推转速
        send_writer.Key("rightRPM");
        send_writer.Int(usvPropellerStatusMenu->right_propeller_rpm);

        /** 目标信息 **/
        // 目标艏向
        send_writer.Key("targetYaw");
        send_writer.Double(targetMenu->yaw);
        // 目标速度
        send_writer.Key("targetFSpeed");
        send_writer.Double(targetMenu->speed);
        // 目标经度
        send_writer.Key("targetLon");
        send_writer.Double(targetMenu->lon);
        // 目标纬度
        send_writer.Key("targetLat");
        send_writer.Double(targetMenu->lat);
        // 视线与路径焦点经度
        send_writer.Key("losLon");
        send_writer.Double(targetMenu->los_lon);
        // 视线与路径焦点纬度
        send_writer.Key("losLat");
        send_writer.Double(targetMenu->los_lat);
        //发送当前艇航向参数
        send_writer.Key("yawKp");
        send_writer.Double(usvPIDMenu->yaw_kp);

        send_writer.Key("yawKi");
        send_writer.Double(usvPIDMenu->yaw_ki);

        send_writer.Key("yawKd");
        send_writer.Double(usvPIDMenu->yaw_kd);
        //发送当前艇航速参数
        send_writer.Key("speedKp");
        send_writer.Double(usvPIDMenu->speed_kd);
        
        send_writer.Key("speedKi");
        send_writer.Double(usvPIDMenu->speed_ki);

        send_writer.Key("speedKd");
        send_writer.Double(usvPIDMenu->speed_kd);


        //加入括回
        send_writer.EndObject();
        // 发送的字符串
        std::string send_str = send_buffer.GetString();

        return send_str;
    }

    // 无人艇路径信息
    inline std::string cookSendPCPathMessage(ControlHub::RobotTargetType * targetMenu)
    {

        /** 发送的数据 **/
        // 创建一个Document对象
        rapidjson::Document doc;
        doc.SetObject();
        // 获取一个分配器用于添加元素
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
        // 添加复杂的数组 targetLineAry
        rapidjson::Value lineArray(rapidjson::kArrayType);

        for (const auto& pair : targetMenu->mission_path_ary)
        {
            rapidjson::Value point(rapidjson::kArrayType);
            point.PushBack(pair[0], allocator);
            point.PushBack(pair[1], allocator);
            lineArray.PushBack(point, allocator);
        }

        doc.AddMember("targetLineAry", lineArray, allocator);

        // 将Document转换为字符串
        rapidjson::StringBuffer send_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(send_buffer);
        doc.Accept(writer);

        // 发送的字符串
        std::string send_str = send_buffer.GetString();

        return send_str;
    }
}



#endif //AUV_RadioWriterCooker_HPP