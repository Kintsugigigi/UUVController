//
// @Time     : 2023/12/8 20:28
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_COMMUNICATIONHUB_H
#define AUV_COMMUNICATIONHUB_H

#include "string"
#include "atomic"

namespace CommunicationHub
{
    /** 艇主控通信状态 **/
    struct USVMasterType
    {
        uint8_t status_type;   //0:无数据 1:正常 2:失联 3:异常
        unsigned long int dog_time; //失联计数
    };

    /** 与上位机通信状态 **/
    struct PCType
    {
        uint8_t status_type;   //0:无数据 1:正常 2:失联 3:异常
        unsigned long int dog_time; //失联计数
    };

    /** 接收的艇主控消息 **/
    struct USVMasterGet1Type
    {
        uint8_t data_type;   //0:无数据 1:正常 2:失联 3:异常
        float UTC;
        double lon;        //经度
        double lat;        //纬度
        float yaw;         //艏向deg
        float pitch;       //纵摇deg
        float roll;        //横摇deg
        float east_speed;  //东向速度
        float north_speed; //北向速度
        float yaw_roti;    //艏向速度  度/分钟
        uint8_t gnss_status; //GPS定位状态 0:失效 1:信号一般 2:信号较好
        uint8_t left_propeller_status; //左推状态
        uint16_t left_propeller_rpm;    //左推转速
        uint16_t left_propeller_angle;  //左推角度  暂时没用
        uint8_t right_propeller_status;//右推状态
        uint16_t right_propeller_rpm;   //右推转速
        uint16_t right_propeller_angle;  //右推角度  暂时没用
    };

    /** 发送给艇主控消息 **/
    struct USVMasterSendType
    {
        float thrust_x;      //横向推力
        float thrust_y;      //纵向推力
        float torque;        //转矩
        int  wp_index;       //正在执行的航迹点
        uint8_t wp_status;   //收到的点状态
        int sk_finished;     //正在执行的航迹点
        uint8_t rx_status;   //收到的点状态
        double cog_lon;      //CoG经度
        double cog_lat;      //CoG纬度
        uint8_t path_status; // 寻迹状态
        uint8_t risk_status; // 避障状态 暂时没用
        uint16_t left_propeller_rpm;    //左推转速 暂时没用
        uint16_t left_propeller_angle;  //左推角度 暂时没用
        uint16_t right_propeller_rpm;   //右推转速 暂时没用
        uint16_t right_propeller_angle; //右推角度 暂时没用
    };
}


#endif //AUV_COMMUNICATIONHUB_H
