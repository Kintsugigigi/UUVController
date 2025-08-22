//
// @Time     : 2023/9/9 11:49
// @Author   : Yang
// @Software : CLion
// UDP基础通讯

#ifndef USVCONTROLLER_UDPSOCKETBASECLASS_H
#define USVCONTROLLER_UDPSOCKETBASECLASS_H

#include<iostream>
#include <mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include "thread"
#include "queue"
#include "../Tools/BlockingQueue.h"

class UDPSocketBaseClass 
{
public:

    UDPSocketBaseClass();
    ~UDPSocketBaseClass();

    // 创建UDPSocket
    void createUDPSocket(const std::string&, const int&, const std::string&, const int&);
    // 开启接收线程
    void startGetMessage();
    // 开启发送线程
    void startSendMessage();
    // 发送消息 方法
    bool socketSendMessage(const std::string);

    // 发送消息 方法
    bool socketSendCharMessage(unsigned char *, int);

    // 关闭TCP连接
    void closeUDPSocket();

    // 客户端获取的消息队列
    BlockingQueue<std::string> get_msg_queue;
    std::mutex mutex_get_cout;
    // 客户端发送消息队列
    BlockingQueue<std::string> send_msg_queue;

    /*********************** socket ************************/
    // udp地址地址
    sockaddr_in host_addr;
    sockaddr_in target_addr;

    // UDP
    int udp_socket;
    // DUP连接状态 0:未创建 1:创建成功 2:绑定端口失败 3:关闭
    int socket_status;

private:

    // 客户端发送消息队列 锁
    std::mutex mutex_send_cout;
    /** ********************** 通信线程 ********************** **/
    /* ---- 客户端 ---- */
    std::thread get_msg_thread;
    std::thread send_msg_Thread;

    /** ********************** 通信方法 ********************** **/
    /* ---- 客户端 ---- */
    // 客户端获取消息方法
    static void  getUDPMessageLoop(void*);
    // 客户端发送消息方法
    static void  sendUDPMessageLoop(void*);
};


#endif //USVCONTROLLER_UDPSOCKETBASECLASS_H
