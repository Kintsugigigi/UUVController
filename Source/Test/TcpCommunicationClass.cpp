//
// Created by GY on 2021/5/12.
//

#include "TcpCommunicationClass.h"
//#include<tchar.h>



using namespace std;

CommunicationClass::CommunicationClass()
{

}

CommunicationClass::~CommunicationClass()
{
    //清理
    close(tcpServerSocket);

}


/**********
 * 服务端 *
 * *********/

/* 服务端 初始化 */
void CommunicationClass::startTcpServer(std::string ipAddr, int myPort)
{
    //创建socket
    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
    tcpServerSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (tcpServerSocket == SOCKET_ERROR) {
        cout << "创建socket server失败 = " << WSAGetLastError() << endl;
        return ;
    }

    // 设置 ip
    static const char * ipChar = ipAddr.data();
    //设置 端口号
    static int nPort = myPort;
    // socket 设置信息结构体
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ipChar);
    serverAddr.sin_port = htons(nPort);
    //    printf("准备连接");
    cout << "准备Tcp server"  << endl;

    if (bind(tcpServerSocket, (SOCKADDR *)&serverAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        cout << "Tcp server 套接字绑定失败！" << endl;
        closesocket(tcpServerSocket);
        return;
    }

    //设置套接字为监听状态
    if (listen(tcpServerSocket, SOMAXCONN) < 0)
    {
        cout << "Tcp server 设置监听状态失败！" << endl;
        closesocket(tcpServerSocket);
        return;
    }
    // 开启服务线程
    serverThread = thread(CommunicationClass::holdServer_pth, this);
}

/* 服务端 监听连接 */
void * CommunicationClass::holdServer_pth(void* __this)
{
    cout << "服务端正在监听连接，请稍候...." << endl;
    CommunicationClass * _this =(CommunicationClass *)__this;

    while(1)
    {
        int len = sizeof(SOCKADDR);
        //等待客户请求到来
        _this ->serverSockConn = accept(_this->tcpServerSocket, (SOCKADDR *) &_this -> serverConnAddr, &len);
        if(_this ->serverSockConn == SOCKET_ERROR)
        {
            cout << "Tcp Server 等待请求失败" <<  WSAGetLastError() << endl;
            break;
        }
        else
        {
            cout << "Tcp server 有客户端连接" << endl;
            // 服务端接收消息 线程
            _this->getServerThread = thread(CommunicationClass::getServerMessage_pth, _this);
            // 服务端发送消息 线程
            _this->sendServerThread = thread(CommunicationClass::sendServerMessage_pth, _this);
        }
        cout << " Tcp server 连接的客户端的IP是:" <<  inet_ntoa(_this -> serverConnAddr.sin_addr) << endl;
        cout << "Tcp server 连接的客户端的端口是:" <<_this ->   serverConnAddr.sin_port << endl;

    }
    closesocket(_this->tcpServerSocket);
    return 0;
}

/* 服务端 接收消息 */
void * CommunicationClass::getServerMessage_pth(void* __this)
{
    CommunicationClass * _this =(CommunicationClass *)__this;
    // 接收数据
    char recvBuf[100];

    while(true){
        memset(recvBuf, 0, sizeof(recvBuf));
        //      //接收数据
        int cnt  = recv(_this->serverSockConn, recvBuf, sizeof(recvBuf), 0);
        if(cnt < 0)
        {
            closesocket(_this->serverSockConn);
            break;
        }
        // 加入客户端接收消息的队列
        _this->getServerQueue.push(recvBuf);
        printf("Tcp server 接收到的消息 %s\n", recvBuf);
    }

    cout << "Tcp server 与客户端连接断开："  << inet_ntoa(_this->serverConnAddr.sin_addr) << endl;
    return 0;
}

/* 服务端 发送消息 */
void * CommunicationClass::sendServerMessage_pth(void* __this)
{
    CommunicationClass * _this =(CommunicationClass *)__this;

    while(true)
    {
        // 从客户端发送队列中获取消息
        string sendStr = _this->sendServerQueue.take();
        // 转换消息格式
        char sendbuf[1024] ;
        strcpy(sendbuf, sendStr.c_str());
        int iSend = send(_this->serverSockConn, sendbuf, sizeof(sendbuf) , 0);
        if(iSend == SOCKET_ERROR)
        {
            cout << "Tcp server 向客户端发送数据失败："  << iSend << endl;
            break;
        }
        else
        {

        }
        std::lock_guard<mutex> lock(_this->mutex_send_server_cout);
    }
    return 0;
}

/* 关闭服务端 */
void CommunicationClass::closeTCPServer()
{
    //关闭socket连接
    closesocket(serverSockConn);
    //清理
    WSACleanup();
}


/**********
 * 客户端 *
 * *********/
/* 客户端 建立连接 */
void CommunicationClass::startTcpClient(std::string ipAddr, int myPort)
{
    //创建客户端socket
    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
    tcpClientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if (tcpClientSocket == SOCKET_ERROR) {
        cout << "创建socket client 失败 = " << WSAGetLastError() << endl;
        return ;
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

    if (connect(tcpClientSocket, (SOCKADDR *)&clientAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        cout << "服务器连接失败！" << endl;
        closesocket(tcpClientSocket);
    }
    else
    {
        cout << "服务器连接成功！" << endl;
    }

    // 服务端接收消息 线程
    getClientThread = thread(CommunicationClass::getClientMessage_pth, this);
    // 服务端发送消息 线程
    sendClientThread = thread(CommunicationClass::sendClientMessage_pth, this);
}

/* 客户端 接收消息 */
void * CommunicationClass::getClientMessage_pth(void* __this)
{
    CommunicationClass * _this =(CommunicationClass *)__this;
    // 接收数据
    char recvBuf[100];
    while(true)
    {
        memset(recvBuf, 0, sizeof(recvBuf));
        // 接收数据
        int cnt  = recv(_this->tcpClientSocket, recvBuf, sizeof(recvBuf), 0);
        if(cnt < 0)
        {
            closesocket(_this->tcpClientSocket);
            break;
        }
        // 加入客户端接收消息的队列
        _this->getClientQueue.push(recvBuf);
        printf("Client 接收到的消息 %s\n", recvBuf);
    }

    cout << "Tcp server 与客户端连接断开："  << inet_ntoa(_this->serverConnAddr.sin_addr) << endl;
    return 0;
}

/* 客户端 发送消息 */
void * CommunicationClass::sendClientMessage_pth(void* __this)
{
    CommunicationClass * _this =(CommunicationClass *)__this;

    while(true)
    {
        // 从客户端发送队列中获取消息
        string sendStr = _this->sendClientQueue.take();
        // 转换消息格式
        char sendbuf[1024] ;
        strcpy(sendbuf, sendStr.c_str());
        cout << "Tcp client 发送的消息："  << sendbuf << endl;
        int iSend = send(_this->tcpClientSocket, sendbuf, sizeof(sendbuf), 0);
        if(iSend == SOCKET_ERROR)
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







/* 单线程扩展 备用*/
//void CommunicationClass::getTcpMessage(SOCKADDR_IN addrClient)
//{
//    // 接收数据
//    char recvBuf[100];
//    while(true)
//    {
//        memset(recvBuf, 0, sizeof(recvBuf));
//        //      //接收数据
//        int cnt  = recv(serverSockConn, recvBuf, sizeof(recvBuf), 0);
//        if(cnt < 0)
//        {
//            closesocket(serverSockConn);
//            break;
//        }
//        printf("接收到的消息 %s\n", recvBuf);
//    }
//
//    cout << "Tcp server 与客户端连接断开："  << inet_ntoa(addrClient.sin_addr) << endl;
//}
//
//
//void CommunicationClass::sendTcpMessage()
//{
//    //发送数据
//    char sendbuf[] = "服务端已连接";
//    int iSend = send(serverSockConn, sendbuf, sizeof(sendbuf) , 0);
//    if(iSend == SOCKET_ERROR)
//    {
//        printf(" Tcp server 向客户端发送数据失败");
//    }
//}
/****************************************************** UDP 备用 **********************************************************************************/
//void CommunicationClass::startUDPCommunication(std::string ipAddr, int myPort)
//{
//    static WSAData wsd;           //初始化信息
////    SOCKET soSend;         //发送SOCKET
//
//    //启动Winsock
//    if (WSAStartup(MAKEWORD(2,2),&wsd) != 0) {/*进行WinSocket的初始化,
//        windows 初始化socket网络库，申请2，2的版本，windows socket编程必须先初始化。*/
//        cout << "WSAStartup Error = " << WSAGetLastError() << endl;
//        return ;
//    }
//
//    //创建socket
//    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
//    //SOCK_DGRAM --  UDP类型，不保证数据接收的顺序，非可靠连接；
//    sendSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
//    if (sendSocket == SOCKET_ERROR) {
//        cout << "socket Error = " << WSAGetLastError() << endl;
//        return ;
//    }
//
//    //设置端口号
//    static int nPort = myPort;
//    // ip
//    static const char* ipChar = ipAddr.data();
//
//    addrSrv.sin_family = AF_INET;
//    addrSrv.sin_port = htons(nPort);
//    addrSrv.sin_addr.s_addr = inet_addr(ipChar);
//
////    printf("准备连接");
//    cout << "准备UDP连接"  << endl;
//
//}
//
//
//
//void CommunicationClass::closeCommunication()
//{
//    //    //关闭socket连接
//    closesocket(sendSocket);
//    //清理
//    WSACleanup();
//}
//
//void CommunicationClass::sendMessage(std::string oneMessageStr)
//{
//    static int nRet = 0;
//    static const char* messageChar = oneMessageStr.data();
//    //发送数据到指定的IP地址和端口
//    nRet = sendto(sendSocket,messageChar,strlen(messageChar),0,(SOCKADDR*)&siLocal,sizeof(SOCKADDR));
//    if (nRet == SOCKET_ERROR) {
//        cout << "发送失败" << WSAGetLastError() << endl;
//        return;
//    }
//}

//void CommunicationClass::getQueueMessage(std::string ipAddr, int myPort)
//{
//
//
//}

//int _tmain(int argc, _TCHAR* argv[])
//{
//
////    thread testThread;
////
////    WSAData wsd;           //初始化信息
////    SOCKET soSend;         //发送SOCKET
////    int nRet = 0;
////    //int i = 0;
////    int dwSendSize = 0;
////    SOCKADDR_IN siLocal;    //远程发送机地址和本机接收机地址
////
////    //启动Winsock
////    if (WSAStartup(MAKEWORD(2,2),&wsd) != 0) {/*进行WinSocket的初始化,
////        windows 初始化socket网络库，申请2，2的版本，windows socket编程必须先初始化。*/
////        cout << "WSAStartup Error = " << WSAGetLastError() << endl;
////        return 0;
////    }
////
////    //创建socket
////
////    //AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
////    //SOCK_DGRAM --  UDP类型，不保证数据接收的顺序，非可靠连接；
////    soSend = socket(AF_INET,SOCK_STREAM,IPPROTO_UDP);
////    if (soSend == SOCKET_ERROR) {
////        cout << "socket Error = " << WSAGetLastError() << endl;
////        return 1;
////    }
////
////    //设置端口号
////    int nPort = 8000;
////    siLocal.sin_family = AF_INET;
////    siLocal.sin_port = htons(nPort);
////    siLocal.sin_addr.s_addr = inet_addr("127.0.0.0");
////    printf("sfsdfk");
////
////
////    for (int i = 0; i < 30; i ++){
////        //开始发送数据
////        //发送数据到指定的IP地址和端口
////        nRet = sendto(soSend,"123 mutouren",strlen("123 mutouren"),0,(SOCKADDR*)&siLocal,sizeof(SOCKADDR));
////        if (nRet == SOCKET_ERROR) {
////            cout << "sendto Error " << WSAGetLastError() << endl;
////            break;
////        }
////    }
////
//////    //关闭socket连接
//////    closesocket(soSend);
//////    //清理
//////    WSACleanup();
//
//
//
//    return 0;
//}



