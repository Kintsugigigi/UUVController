//
// @Time     : 2023/11/12 11:36
// @Author   : Yang
// @Software : CLion
// 智嵌物联CAN模块数据格式

#ifndef USVCONTROLLER_ZQCANModel_HPP
#define USVCONTROLLER_ZQCANModel_HPP

#include "string"
#include <cstdint>

namespace ZQCANModel
{
#pragma pack(1)
    /** CAN模块头部信息 **/
    struct CANHeadType
    {
        uint8_t data_len = 0x08;     //CAN数据段长度
        uint8_t can_frame;    //0:CAN数据帧 1:远程帧
        uint8_t data_control; //0:标准帧 1:扩展帧
        uint16_t can_id;      //CAN ID
    };
#pragma pack()
    /** CAN标准帧信息 **/
    struct CANStandardType
    {
        CANHeadType head_msg; //头部信息
        uint8_t data_1;
        uint8_t data_2;
        uint8_t data_3;
        uint8_t data_4;
        uint8_t data_5;
        uint8_t data_6;
        uint8_t data_7;
        uint8_t data_8;
    };
}

#endif //USVCONTROLLER_ZQCANModel_HPP