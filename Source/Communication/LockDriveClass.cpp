//
// @Time     : 2023/12/13 17:15
// @Author   : Yang
// @Software : CLion
//

#include "LockDriveClass.h"
#include "cstring"

LockDriveClass::LockDriveClass()
{

}

LockDriveClass::~LockDriveClass()
{
    //清理
}

void LockDriveClass::connectLock(const std::string &host_addr, int host_port, const std::string &target_addr, int target_port)
{
    // 启动服务 连接
    lockSocketClass.createUDPSocket(host_addr, host_port, target_addr, target_port);
    // 读取无线电消息 线程
    get_msg_thread = std::thread(LockDriveClass::getMessageLoop, this);
    // 发送消息给无线电 线程
    send_msg_thread = std::thread(LockDriveClass::sendMessageLoop, this);
}


void LockDriveClass::getMessageLoop(void * __this)
{
    // 接收数据
    unsigned char receive_buffer[2048];
    LockDriveClass * _this =(LockDriveClass *)__this;
    while(true)
    {
        // 清空缓存
        memset(receive_buffer, 0, sizeof(receive_buffer));
        // 接收数据
        int msg_len = recv(_this->lockSocketClass.udp_socket, receive_buffer, sizeof(receive_buffer), 0);
        if(msg_len < 0)
        {
            // 更新连接状态
            _this->lockSocketClass.socket_status = 3;
//            // 关闭socket
//            close(_this->uiSocketClass.udp_socket);
            break;
        }
        else
        {
            _this->cookGetMessage(receive_buffer, msg_len);
        }
    }
}

/** 发送消息  循环 **/
void LockDriveClass::sendMessageLoop(void* __this)
{
    LockDriveClass * _this =(LockDriveClass *)__this;

    while (true)
    {
        // 发送 心跳信息
        std::string send_str = "Main Program: Active";
        bool switch_status = _this->lockSocketClass.socketSendMessage(send_str);
        // 每组间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_num));
    }
}


bool LockDriveClass::cookGetMessage(unsigned char get_char[], int len)
{
    return false;
}

