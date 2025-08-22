//
// @Time     : 2023/9/9 11:49
// @Author   : Yang
// @Software : CLion
//

#include <errno.h>
//#include <cstdio>
#include<arpa/inet.h>
#include "TCPSocketClientClass.h"

using namespace std;

TCPSocketClientClass::TCPSocketClientClass()
{

}

TCPSocketClientClass::~TCPSocketClientClass()
{
    //清理
    close(tcpClientSocket);
}

/**********
 * 客户端 *
 * *********/
/* 客户端 建立连接 */
void TCPSocketClientClass::startTCPClient(std::string ipAddr, int myPort)
{
    //创建客户端socket
    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
    tcpClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (tcpClientSocket == -1)
    {
        cout << "创建socket client 失败"  << endl;
        fprintf(stderr, "创建socket client 失败 , reason: %s\n", strerror(errno));
        exit(-1);
        return;
    }

    // 设置 ip
    static const char * ipChar = ipAddr.data();
    //设置 端口号
    static int nPort = myPort;
    // socket 设置信息结构体
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = inet_addr(ipChar);
    clientAddr.sin_port = htons(nPort);
    //    printf("准备连接");
    cout << "准备Tcp client 连接"  << endl;

    if (connect(tcpClientSocket, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == -1)
    {
        cout << "服务器连接失败！" << endl;
        close(tcpClientSocket);
    }
    else
    {
        cout << "服务器连接成功！" << endl;
    }

    // 服务端接收消息 线程
    getClientThread = thread(TCPSocketClientClass::getClientMessage_pth, this);
    // 服务端发送消息 线程
    sendClientThread = thread(TCPSocketClientClass::sendClientMessage_pth, this);
}

/* 客户端 接收消息 */
void * TCPSocketClientClass::getClientMessage_pth(void* __this)
{
    TCPSocketClientClass * _this =(TCPSocketClientClass *)__this;
    // 接收数据
    char recvBuf[100];
    while(true)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        // 接收数据
        int cnt  = recv(_this->tcpClientSocket, recvBuf, sizeof(recvBuf), 0);
        if(cnt < 0)
        {
            close(_this->tcpClientSocket);
            break;
        }
        // 加入客户端接收消息的队列
        _this->getClientQueue.push(recvBuf);
        printf("Client 接收到的消息 %s\n", recvBuf);
    }

    cout << "Tcp server 与客户端连接断开："  << inet_ntoa(_this->serverAddr.sin_addr) << endl;
    return 0;
}

/* 客户端 发送消息 */
void * TCPSocketClientClass::sendClientMessage_pth(void* __this)
{
    TCPSocketClientClass * _this =(TCPSocketClientClass *)__this;

    while(true)
    {
        // 从客户端发送队列中获取消息
        string sendStr = _this->sendClientQueue.take();
        // 转换消息格式
        char sendbuf[1024] ;
        strcpy(sendbuf, sendStr.c_str());
        cout << "Tcp client 发送的消息："  << sendbuf << endl;
        int iSend = send(_this->tcpClientSocket, sendbuf, sizeof(sendbuf), 0);
        if(iSend == -1)
        {
            cout << "Client 向服务端发送数据失败："  << iSend << endl;
            break;
        }
        else
        {

        }
        std::lock_guard<mutex> lock(_this->mutex_send_client_cout);
    }
    return 0;
}



