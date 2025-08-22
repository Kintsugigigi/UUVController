//
// @Time     : 2024/6/5 19:46
// @Author   : Yang
// @Software : CLion
//

#include "NetToUSVMasterClass.h"
#include "../Tools/GISTools.hpp"


NetToUSVMasterClass::NetToUSVMasterClass()
{
    memset(&usvGetMenu, 0, sizeof(CommunicationHub::USVMasterGet1Type));
    memset(&usvSendMenu, 0, sizeof(CommunicationHub::USVMasterSendType));
}

NetToUSVMasterClass::~NetToUSVMasterClass()
{

}

void NetToUSVMasterClass::connectUSVMaster(const std::string &host_addr, const int &host_port,
                                           const std::string &target_addr, const int &target_port)
{
    // 与USV主控相连
    usvSocketClass.createTCPClient(target_addr, target_port);

    // 读取USV主控 线程
    get_msg_thread = std::thread(NetToUSVMasterClass::getMessageLoop, this);
    // 发送消息给USV主控 线程
    send_msg_thread = std::thread(NetToUSVMasterClass::sendMessageLoop, this);

}

void NetToUSVMasterClass::getMessageLoop(void *__this)
{
    NetToUSVMasterClass * _this =(NetToUSVMasterClass *)__this;
    // 接收数据
    char recvBuf[100];
    while (true)
    {

        memset(recvBuf, 0, sizeof(recvBuf));
        // 接收数据
        int cnt  = recv(_this->usvSocketClass.tcpClientSocket, recvBuf, sizeof(recvBuf), 0);
        if(cnt < 0)
        {
            close(_this->usvSocketClass.tcpClientSocket);
            break;
        }
        else
        {
            printf("Client 接收到的消息 %s\n", recvBuf);
            if(recvBuf[0] == 1)
            {
                // 赋值
                memcpy(&_this->usvGetMenu, &recvBuf, sizeof(_this->usvGetMenu));

                // 看门狗时间清零
                _this->usvComStatusMenu.dog_time = 0;
                _this->usvNavMenu->dog_time = 0;
                //各项位置姿态值
                _this->usvNavMenu->lon = _this->usvGetMenu.lon;
                _this->usvNavMenu->lat = _this->usvGetMenu.lat;
                _this->usvNavMenu->yaw = _this->usvGetMenu.yaw;
                _this->usvNavMenu->pitch = _this->usvGetMenu.pitch;
                _this->usvNavMenu->roll = _this->usvGetMenu.roll;
                _this->usvNavMenu->east_speed = _this->usvGetMenu.east_speed;
                _this->usvNavMenu->north_speed = _this->usvGetMenu.north_speed;
                _this->usvNavMenu->gnss_status = _this->usvGetMenu.gnss_status;
                // 计算纵向与横向速度
                YGISTools::calculateVelocities(_this->usvNavMenu->yaw,
                                               _this->usvNavMenu->east_speed,
                                               _this->usvNavMenu->north_speed,
                                               _this->usvNavMenu->fore_speed,
                                               _this->usvNavMenu->starboard_speed);
                // 推进器状态
                _this->usvPropellerStatusMenu->left_propeller_rpm = _this->usvGetMenu.left_propeller_rpm;
                _this->usvPropellerStatusMenu->left_propeller_status = _this->usvGetMenu.left_propeller_status;
                _this->usvPropellerStatusMenu->right_propeller_rpm = _this->usvGetMenu.right_propeller_rpm;
                _this->usvPropellerStatusMenu->right_propeller_status = _this->usvGetMenu.right_propeller_status;
            }
        }
    }
}

void NetToUSVMasterClass::sendMessageLoop(void *__this)
{
    NetToUSVMasterClass * _this =(NetToUSVMasterClass *)__this;

    while (true)
    {
        // 赋值 推力
        _this->usvSendMenu.thrust_x = _this->usvControlMenu->thrust;
        _this->usvSendMenu.thrust_y = _this->usvControlMenu->thrust;
        // 赋值 转矩
        _this->usvSendMenu.torque = _this->usvControlMenu->torque;

        // 发送之
        int iSend = send(_this->usvSocketClass.tcpClientSocket, &_this->usvSendMenu, sizeof(_this->usvSendMenu), 0);
        if(iSend == -1)
        {
            printf("TCPClient 向USV主控发送数据失败!");
            break;
        }
        else
        {

        }
        // 每组间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_manual));
    }
}

bool NetToUSVMasterClass::cookGetMessage(unsigned char *, int)
{
    return false;
}

void NetToUSVMasterClass::sendMessageNow(unsigned char *, int)
{

}
