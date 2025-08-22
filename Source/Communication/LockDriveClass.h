//
// @Time     : 2023/12/13 17:15
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_LOCKDRIVECLASS_H
#define AUV_LOCKDRIVECLASS_H

#include "../Postman/UDPSocketBaseClass.h"
#include "string"

class LockDriveClass
{
public:
    LockDriveClass();
    ~LockDriveClass();

    // 连接UI的类
    UDPSocketBaseClass  lockSocketClass;

    // 连接动力锁
    void connectLock(const std::string &, int host_port,
                     const std::string &, int target_port);

private:
    // CAN通信发送间隔  毫秒
    int interval_num = 800;
    // 从动力锁获取消息 线程
    std::thread get_msg_thread;
    // 向动力锁发送消息 线程
    std::thread send_msg_thread;
    // 从动力锁获取消息 循环
    static void getMessageLoop(void*);
    // 向动力锁发送消息 循环
    static void sendMessageLoop(void*);
    // 处理 动力锁获取消息
    bool cookGetMessage(unsigned char[], int);
};


#endif //AUV_LOCKDRIVECLASS_H
