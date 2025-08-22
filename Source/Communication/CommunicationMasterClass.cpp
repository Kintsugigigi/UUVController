//
// @Time     : 2023/12/11 18:11
// @Author   : Yang
// @Software : CLion
//

#include "CommunicationMasterClass.h"
#include "cstring"

CommunicationMasterClass::CommunicationMasterClass()
{

}

CommunicationMasterClass::~CommunicationMasterClass()
{

}

void CommunicationMasterClass::startConnect()
{
    // 本机IP 地址
    std::string host_ip_str = "192.168.3.109";
    /** USV 主控 **/
    int usv_master_port_my = 2501;
    int usv_master_port = 2500;


    // UI 上位机 地址
    std::string ui_ip_str = "192.168.3.200";
    /** 上位机 **/
    int pc_port = 8000;
    int pc_port_my = 8000;

    // 与USV主控通信的类
    usvMasterClass.usvNavMenu = usvNavMenu;
    usvMasterClass.usvPropellerStatusMenu = usvPropellerStatusMenu;
    usvMasterClass.usvControlMenu = usvControlMenu;
    usvMasterClass.connectUSVMaster(host_ip_str,usv_master_port_my, host_ip_str, usv_master_port);

    // 与上位机通信的类
    pcClass.robotNowMode = robotNowMode;
    pcClass.usvNavMenu = usvNavMenu;
    pcClass.targetMenu = targetMenu;
    pcClass.usvPropellerStatusMenu = usvPropellerStatusMenu;
    pcClass.usvControlMenu = usvControlMenu;
    pcClass.connectPC(host_ip_str,pc_port_my, ui_ip_str, pc_port);

    // 看门狗 线程
    watch_dog_thread = std::thread(CommunicationMasterClass::watchDogLoop, this);
}

void CommunicationMasterClass::watchDogLoop(void* __this)
{
    CommunicationMasterClass * _this =(CommunicationMasterClass *)__this;
    while (true)
    {
        // 失联状态
        int lonely_status = 0;
        // 主控通信状态
        _this->usvMasterClass.usvComStatusMenu.dog_time += 1;
        if (_this->usvMasterClass.usvComStatusMenu.dog_time >= _this->limitMenu->lossTime)
        {
            printf("主控失联");
            //标识为设备失联
            _this->usvMasterClass.usvComStatusMenu.status_type = 2;
            // 失联
            _this->robotNowMode->warning_type = ControlHub::WarningDivce;
            // 失联
            lonely_status = 1;
        }
        else
        {
            // 在线
            lonely_status = 0;
        }

        if (_this->pcClass.pcComStatusMenu.dog_time >= _this->limitMenu->lossTime)
        {
            printf("上位机失联");
            //标识为设备失联
            _this->pcClass.pcComStatusMenu.status_type = 2;
            // 失联
            _this->robotNowMode->warning_type = ControlHub::WarningLonely;
            // 失联
            lonely_status = 1;
        }
        else
        {
            // 在线
            lonely_status = 0;
        }

        // 失联标识
        if(lonely_status > 0)
        {
            // 失联
            _this->robotNowMode->warning_type = ControlHub::WarningLonely;
        }
        else
        {
            //超时归零
            _this->robotNowMode->lonely_time = 0;
            // 解除失联状态
            if(_this->robotNowMode->warning_type == ControlHub::WarningLonely)
            {
                _this->robotNowMode->warning_type = ControlHub::WarningNormal;
            }
        }
        sleep(1);
    }
}


