//
// Created by GY on 2021/5/12.
//

#ifndef UNTITLED1_TCPCOMMUNICATIONCLASS_H
#define UNTITLED1_TCPCOMMUNICATIONCLASS_H

#include<iostream>
#include <mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "thread"
#include "queue"
#include "../Tools/BlockingQueue.h"

class CommunicationClass
{
public:

    CommunicationClass();
    ~CommunicationClass();

    // 开启TCP服务
    void startTcpServer(std::string, int);
    // 客户端去连接TCP
    void startTcpClient(std::string, int);

    // 关闭tcp 服务
    void closeTCPServer();

    // 客户端获取的消息队列
    BlockingQueue<std::string> getClientQueue;
    std::mutex mutex_get_client_cout;
    // 客户端发送的消息队列
    BlockingQueue<std::string> sendClientQueue;
    // 服务端获取的消息队列
    BlockingQueue<std::string> getServerQueue;
    std::mutex mutex_get_server_cout;
    // 服务端发送的消息队列
    BlockingQueue<std::string> sendServerQueue;

private:

    /************************ socket ************************/
    /* ---- 服务端 ---- */
    // 本机Tcp服务地址
    sockaddr_in serverAddr;
    int tcpServerSocket;

    // 服务端响应请求的连接
    sockaddr_in serverConnAddr;
    int serverSockConn;
    // 服务端获取消息队列 锁
    std::mutex mutex_send_server_cout;


    /* ---- 客户端 ---- */
    // 客户端去连接的地址
    sockaddr_in clientAddr;
    int tcpClientSocket;
    // 客户端获取消息队列 锁
    std::mutex mutex_send_client_cout;


    /************************ 通信线程 ************************/
    /* ---- 服务端 ---- */
    //服务端监听 线程
    std::thread serverThread;
    // 服务端获取消息 线程
    std::thread getServerThread;
    // 服务端发送消息 线程
    std::thread sendServerThread;

    /* ---- 客户端 ---- */
    std::thread getClientThread;
    std::thread sendClientThread;


    // ************************ 通信方法 ************************
    // ---- 服务端 ----
    // Tcp 服务监听线程开启
    static void * holdServer_pth(void*);
    // 服务端获取消息方法
    static void * getServerMessage_pth(void*);
    // 服务端发送消息方法
    static void * sendServerMessage_pth(void*);

    /* ---- 客户端 ---- */
    // 客户端获取消息方法
    static void * getClientMessage_pth(void*);
    // 客户端发送消息方法
    static void * sendClientMessage_pth(void*);


    // ************************ 备用 ************************
//    // 获取TCP消息
//    void getTcpMessage(SOCKADDR_IN addrClient);
//    // 发送TCP消息
//    void sendTcpMessage();

};


#endif //UNTITLED1_TCPCOMMUNICATIONCLASS_H
