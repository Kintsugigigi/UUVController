
#ifndef USVCONTROLLER_NAVIGATIONHUBTYPE_H
#define USVCONTROLLER_NAVIGATIONHUBTYPE_H

#include "string"
#include "atomic"

namespace NavigationHub
{

    struct USVBackNavigationType
    {
        uint8_t status_type;   //0:无数据 1:设备正常 2:设备失联 3:设备异常
        unsigned long int dog_time; //失联计数

        double lon;        //经度
        double lat;        //纬度
        float yaw;         //艏向deg
        float roll;        //横摇deg
        float pitch;       //纵摇deg
        float yaw_roti;    //艏向速度  度/分钟
        float east_speed;  //东向速度
        float north_speed; //北向速度
        float fore_speed;  //纵向速度
        float starboard_speed; //横向速度
        uint8_t gnss_status; //GPS定位状态 0:失效 1:信号一般 2:信号较好
    };
}


#endif //USVCONTROLLER_NAVIGATIONHUBTYPE_H