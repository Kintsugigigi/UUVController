//
// @Time     : 2023/9/9 11:49
// @Author   : Yang
// @Software : CLion
// UDP基础通讯

#include <errno.h>
//#include <cstdio>
#include<arpa/inet.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstring>
#include "UDPSocketBaseClass.h"


using namespace std;

UDPSocketBaseClass::UDPSocketBaseClass()
{

}

UDPSocketBaseClass::~UDPSocketBaseClass()
{
    //清理
    close(udp_socket);
}

/**********
 * 客户端 *
 * *********/
/* 客户端 建立连接 */
void UDPSocketBaseClass::createUDPSocket(const std::string&  host_ip_str, const int&  host_port,
                                         const std::string& target_ip_str, const int& target_port)
{
    //创建客户端socket
    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
    udp_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (udp_socket == -1 )
    {
        cout << "创建socket client 失败"  << endl;
        fprintf(stderr, "创建socket client 失败 , reason: %s\n", strerror(errno));
        exit(-1);
        return;
    }
    else
    {
//        setsockopt(udp_socket,SOL_SOCKET,SO_REUSEPORT, &);
        //socket 创建成功
        socket_status = 1;
    }
    // 本机 ip
    const char * my_ip_char = host_ip_str.data();

    // 本机 端口号
    int my_port = host_port;
    // socket 设置信息结构体
    host_addr.sin_family = AF_INET;
    host_addr.sin_addr.s_addr = inet_addr(my_ip_char);
//    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_port = htons(my_port);

    // 目标 ip
    const char * target_ip_char = target_ip_str.data();
    // 目标 端口号
    int target_p = target_port;
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(target_ip_char);
    target_addr.sin_port = htons(target_p);
    printf("准备UDP 连接 {}:{:d}", target_ip_char, target_p);
    if (::bind(udp_socket, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0)
    {

        perror("bind");
        printf("UDP绑定失败！！%s:%d \r\n", host_ip_str.c_str(), host_port);

        socket_status = 2; // 绑定端口失败
        close(udp_socket);
    }
    else
    {
        socket_status = 1; // 更新连接状态
        printf("UDP已启动-> {0:s}:{1:d}",inet_ntoa(host_addr.sin_addr),ntohs(host_addr.sin_port));
    }
}

void UDPSocketBaseClass::startGetMessage()
{
    // 服务端接收消息 线程
    get_msg_thread = thread(UDPSocketBaseClass::getUDPMessageLoop, this);
}

void UDPSocketBaseClass::startSendMessage()
{
    // 服务端发送消息 线程
    send_msg_Thread = thread(UDPSocketBaseClass::sendUDPMessageLoop, this);
}

/* 客户端 接收消息 */
void  UDPSocketBaseClass::getUDPMessageLoop(void* __this)
{
    UDPSocketBaseClass * _this =(UDPSocketBaseClass *)__this;

    while(true)
    {
        // 接收数据
        char receive_buffer[1024];
        memset(receive_buffer, 0, sizeof(receive_buffer));
        // 接收数据
        int msg_len  = recv(_this->udp_socket, receive_buffer, sizeof(receive_buffer), 0);
        if(msg_len < 0)
        {
            close(_this->udp_socket);
            printf("UDP已关闭{}:{0:d}", inet_ntoa(_this->host_addr.sin_addr),
                         ntohs(_this->host_addr.sin_port));
            // socket 状态
            _this->socket_status = 3;
            break;
        }
        // 加入客户端接收消息的队列
        string get_str;
        get_str = receive_buffer;
        _this->get_msg_queue.push(get_str);

//        string show_str;
//        for(int i=0; i<msg_len; i++)
//        {
////            char * hexChar;
////            std::format("{}", recv_buf[i]);
//            char hexChar[20] = {};
//            snprintf(hexChar, sizeof(hexChar)-1, "%#.2x ", (unsigned char)recv_buf[i]);
//
//            show_str += hexChar;
////            get_str += recv_buf[i];
//        }
//        printf("Client 接收到的消息 %s \n", show_str.data());
    }
    printf("UDP 监听停止-> {}:{0:d}",inet_ntoa(_this->host_addr.sin_addr),ntohs(_this->host_addr.sin_port));
}

/* UDP 发送消息 */
void  UDPSocketBaseClass::sendUDPMessageLoop(void* __this)
{
    UDPSocketBaseClass * _this =(UDPSocketBaseClass *)__this;

    while(true)
    {
        // 从客户端发送队列中获取消息
        std::string send_byte = _this->send_msg_queue.take();
        // 发送结果
        bool send_status = _this->socketSendMessage(send_byte);
        if (!send_status)
        {
            printf("UDP发送信息失败-> {}:{0:d}",inet_ntoa(_this->host_addr.sin_addr),ntohs(_this->host_addr.sin_port));
        }
        std::lock_guard<mutex> lock(_this->mutex_send_cout);
    }
}

bool UDPSocketBaseClass::socketSendMessage(const std::string get_str)
{
    // string 转 char[]
    char send_char[get_str.length()];
    strncpy(send_char,get_str.c_str(), get_str.length());
    // 发送
    int isend = sendto(udp_socket,
                       (char *)send_char, get_str.length(), 0,
                       (struct sockaddr *)&target_addr,
                       sizeof(target_addr));
    if(isend == -1)
    {
        socket_status = 4;
//        perror("sendto");
//        spdlog::warn("UDP向电源板发送数据失败！！ {}:{0:d}", inet_ntoa(host_addr.sin_addr),
//                     ntohs(host_addr.sin_port));
        return false;
    }
    else
    {
//            cout << "udp socket 向目标发送数据成功："  << isend << endl;
        return true;
    }
    return false;
}

bool UDPSocketBaseClass::socketSendCharMessage(unsigned char * send_char, int len)
{
    // 发送
    int isend = sendto(udp_socket, send_char, len, 0,
                       (struct sockaddr *)&target_addr,
                       sizeof(target_addr));
    if(isend == -1)
    {
        socket_status = 4;
        return false;
    }
    else
    {
        return true;
    }
    return false;
}



