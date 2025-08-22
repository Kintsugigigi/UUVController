//
// @Time     : 2023/12/11 22:13
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV_NAVIGATIONMASTERCLASS_H
#define AUV_NAVIGATIONMASTERCLASS_H

#include "thread"
#include "NavigationHub.h"


class NavigationMasterClass
{
public:
    NavigationMasterClass();
    ~NavigationMasterClass();

    // 无人艇状态信息
    NavigationHub::USVBackNavigationType usvNavMenu;

    // 开启连接
    void startNavigationDevice();

private:
    // 看门狗 线程
    std::thread watch_dog_thread;
    // 看门狗循环
    static void watchDogLoop(void*);
};


#endif //AUV_NAVIGATIONMASTERCLASS_H
