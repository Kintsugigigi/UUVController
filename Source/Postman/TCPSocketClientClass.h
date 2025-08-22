//
// @Time     : 2023/9/9 11:49
// @Author   : Yang
// @Software : CLion
// Linux

#ifndef USVCONTROLLER_TCPSOCKETCLIENTCLASS_H
#define USVCONTROLLER_TCPSOCKETCLIENTCLASS_H

#include<iostream>
#include <mutex>
//#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "thread"
#include "queue"
#include<cstring>
//#include<string.h>
#include "../Tools/BlockingQueue.h"


class TCPSocketClientClass
{
public:

    TCPSocketClientClass();
    ~TCPSocketClientClass();

    // 连接实例
    int tcpClientSocket;

    // 连接服务器
    void createTCPClient(std::string, int);
    // 开启接收线程
    void startGetMessage();
    // 开启发送线程
    void startSendMessage();

    // 关闭TCP连接
    void closeTCPClient();

    // 客户端获取的消息队列
    BlockingQueue<std::string> getClientQueue;
    std::mutex mutex_get_client_cout;
    // 客户端发送消息队列
    BlockingQueue<std::string> sendClientQueue;


private:

    /************************ socket ************************/
    // 客户端自身地址
    sockaddr_in clientAddr;
    // 服务器地址
    sockaddr_in serverAddr;


    // 客户端发送消息队列 锁
    std::mutex mutex_send_client_cout;

    /************************ 通信线程 ************************/
    /* ---- 客户端 ---- */
    std::thread getClientThread;
    std::thread sendClientThread;

    // ************************ 通信方法 ************************
    /* ---- 客户端 ---- */
    // 客户端获取消息方法
    static void * getClientMessage_pth(void*);
    // 客户端发送消息方法
    static void * sendClientMessage_pth(void*);
};


#endif //USVCONTROLLER_TCPSOCKETCLIENTCLASS_H
