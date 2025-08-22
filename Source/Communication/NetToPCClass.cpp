//
// @Time     : 2024/6/5 19:46
// @Author   : Yang
// @Software : CLion
//

#include "NetToPCClass.h"
#include "MsgWriterCooker.hpp"
#include "MsgReadCooker.hpp"


NetToPCClass::NetToPCClass()
{

}

NetToPCClass::~NetToPCClass()
{

}

void NetToPCClass::connectPC(const std::string &host_addr, const int &host_port,
                             const std::string &target_addr, const int &target_port)
{
    // 与上位机连接
    pcSocketClass.createUDPSocket(host_addr, host_port, target_addr, target_port);

    // 读取上位机 线程
    get_msg_thread = std::thread(NetToPCClass::getMessageLoop, this);
    // 发送给上位机 线程
    send_msg_thread = std::thread(NetToPCClass::sendMessageLoop, this);
}

void NetToPCClass::getMessageLoop(void * __this)
{
    // 接收数据
    unsigned char receive_buffer[2048];
    NetToPCClass * _this =(NetToPCClass *)__this;
    while(true)
    {
        // 清空缓存
        memset(receive_buffer, 0, sizeof(receive_buffer));
        // 接收数据
        int msg_len = recv(_this->pcSocketClass.udp_socket, receive_buffer, sizeof(receive_buffer), 0);
        if(msg_len < 0)
        {
            // 更新连接状态
            _this->pcSocketClass.socket_status = 3;
//            // 关闭socket
//            close(_this->uiSocketClass.udp_socket);
            break;
        }
        else
        {
            // 看门狗时间清空
            _this->pcComStatusMenu.dog_time = 0;
            // 处理接收到的消息
            _this->cookGetMessage(receive_buffer, msg_len);
        }
    }
}


/** 发送消息给PC的循环 **/
void NetToPCClass::sendMessageLoop(void * __this)
{
    // 接收数据
    unsigned char receive_buffer[2048];
    NetToPCClass * _this =(NetToPCClass *)__this;

    int send_index = 0;
    while(true)
    {
        // 姿态信息
        std::string send_status_str = MsgWriterCooker::cookSendPCStatusMessage(_this->robotNowMode,_this->usvNavMenu,
                                                 _this->targetMenu,_this->usvPropellerStatusMenu);
        _this->pcSocketClass.socketSendMessage(send_status_str); //发送给上位机

        send_index ++;
        if(send_index > 10)
        {
            send_index = 0;
            // 路径信息
            std::string send_path_str = MsgWriterCooker::cookSendPCPathMessage(_this->targetMenu);
            _this->pcSocketClass.socketSendMessage(send_status_str); //发送给上位机
        }
        // 每组间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_manual));
    }
}


/** 处理从PC接收的数据 **/
bool NetToPCClass::cookGetMessage(unsigned char get_char[], int len)
{
    // 处理收到的数据
    MsgReadCooker::cookGetPCMessage((char*)get_char, robotNowMode, targetMenu);

    return false;
}

void NetToPCClass::sendMessageNow(unsigned char get_char[], int len)
{


}
