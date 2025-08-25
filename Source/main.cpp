//
// @Time     : 2023/5/2 18:46
// @Author   : Yang
// @Software : CLion
// 航道无人艇测试程序

#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <iostream>
#include "ctime"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "Navigation/NavigationMasterClass.h"
#include "Condition/ConditionMasterClass.h"
#include "Control/ControlMasterClass.h"
#include "Action/ActionMasterClass.h"
#include "Communication/CommunicationMasterClass.h"
#include "Tools/GISTools.hpp"
#include "Tools/LogToTextClass.h"
#include "Tools/DataFormatTools.hpp"
// 新增: 串口相关头文件
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <cstring>

// 新增: 打开并配置串口
static int openSerialPort(const char* device, speed_t baud)
{
    int fd = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        spdlog::error("打开串口失败 {} errno={}", device, errno);
        return -1;
    }

    // 清除非阻塞标志，后续用select控制阻塞行为
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        spdlog::error("tcgetattr 失败 errno={}", errno);
        ::close(fd);
        return -1;
    }

    cfmakeraw(&tty);
    // 波特率
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    // 8N1
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // 本地连接，启用接收
    tty.c_cflag |= (CLOCAL | CREAD);

    // 关闭硬件/软件流控
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // 设置读取行为: 立即返回由select控制
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        spdlog::error("tcsetattr 失败 errno={}", errno);
        ::close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    spdlog::info("串口已打开 {} 波特率已设置", device);
    return fd;
}

// 新增: 深度计读取线程函数（原样输出每一行，若为二进制流也会输出十六进制片段）
static void depthReader(const char* device, speed_t baud)
{
    int fd = openSerialPort(device, baud);
    if (fd < 0) {
        spdlog::error("深度计读取线程启动失败，无法打开串口 {}", device);
        return;
    }

    std::string line;
    char buf[256];

    while (true) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        timeval tv{};
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int rv = select(fd + 1, &rfds, nullptr, nullptr, &tv);
        if (rv < 0) {
            spdlog::error("select 出错 errno={}", errno);
            break;
        } else if (rv == 0) {
            // 超时，无数据
            continue;
        }

        ssize_t n = ::read(fd, buf, sizeof(buf));
        if (n < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            spdlog::error("串口读取失败 errno={}", errno);
            break;
        } else if (n == 0) {
            // 串口被关闭
            spdlog::warn("串口无数据或被关闭");
            break;
        } else {
            // 将数据按行输出（适用于以 \n 结尾的ASCII协议）
            for (ssize_t i = 0; i < n; ++i) {
                char c = buf[i];
                if (c == '\r') continue;
                if (c == '\n') {
                    if (!line.empty()) {
                        spdlog::info("Depth RX: {}", line);
                        line.clear();
                    }
                } else {
                    line.push_back(c);
                }
            }

            // 如果不是文本协议，仍然提供十六进制观察数据片段
            std::string hexDump;
            hexDump.reserve(n * 3);
            for (ssize_t i = 0; i < n; ++i) {
                char tmp[4];
                snprintf(tmp, sizeof(tmp), "%02X ", static_cast<unsigned char>(buf[i]));
                hexDump += tmp;
            }
            spdlog::debug("Depth RX HEX: {}", hexDump);
        }
    }

    ::close(fd);
    spdlog::info("深度计读取线程结束，串口已关闭");
}

int main()
{
    sleep(5);

    /** 文件名加时间 **/
    time_t tm;   //time_t是time.h库中定义的数据类型，用以表示时间
    time(&tm);   //time函数用以获取当前的时间，并存储在time_t类型的变量中
    char tmp[64];
    strftime(tmp, sizeof(tmp),"%Y_%m_%d_%H_%M_%S", localtime(&tm));
    std::string time_str(tmp);
    std::string file_name = "/home/auv/Desktop/auv/Tmp/LogText/";
    file_name = file_name + time_str + "log.txt";
    /** logger **/
     auto logger = spdlog::basic_logger_mt("loggerMain",file_name);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);

    /** 运动控制与规划类 **/
    ControlMasterClass controlMasterClass;

    /** 导航相关信息 **/
    NavigationMasterClass navigationMaster;
    navigationMaster.startNavigationDevice();

    /** 执行机构驱动类 **/
    ActionMasterClass actionMasterClass;
    actionMasterClass.robotNowMode = &controlMasterClass.robotNowMode;
    actionMasterClass.limitMenu = &controlMasterClass.limitMenu;
    actionMasterClass.startAction();

    /** 自身状态信息 电池 **/
    //ConditionMasterClass conditionMasterClass;
    //conditionMasterClass.robotNowMode = &controlMasterClass.robotNowMode; //USV模式
    //conditionMasterClass.startConditionListen();

    /** 运动控制与规划类 启动**/
    controlMasterClass.usvNavMenu = &navigationMaster.usvNavMenu;      // USV导航信息
    controlMasterClass.action_queue = &actionMasterClass.action_queue; // 执行机构 队列
    controlMasterClass.startController();

    /** 通信类 **/
    CommunicationMasterClass commMasterClass;
    commMasterClass.robotNowMode = &controlMasterClass.robotNowMode; // 模式
    commMasterClass.limitMenu = &controlMasterClass.limitMenu;       // 限制
    commMasterClass.targetMenu = &controlMasterClass.targetMenu;     // 执行目标
    commMasterClass.remote_queue = controlMasterClass.remote_queue;  // 遥控消息
    commMasterClass.path_queue = &controlMasterClass.path_queue;     // 任务路径点
    commMasterClass.usvNavMenu = &navigationMaster.usvNavMenu;       // USV导航信息
    commMasterClass.usvControlMenu = &actionMasterClass.usvControlMenu; //控制指令 发送给USV主控用
    commMasterClass.usvPropellerStatusMenu = &actionMasterClass.usvPropellerStatusMenu; //推进器状态
    commMasterClass.startConnect();

//    /** Log 文件 **/
//    LogToTextClass logClass;
//    logClass.dvlMenu = &navigationMaster.dvlClass.dvlMenu;
//    logClass.insMenu = &navigationMaster.insClass.insMenu;
//    logClass.depthMenu = &navigationMaster.depthometerClass.depthMenu;
//    logClass.GPSMenu = &navigationMaster.gpsClass.GPSMenu;
//    logClass.soundMenu = &commMasterClass.soundClass.soundMenu;
//    logClass.startLog();

    // 新增: 启动深度计串口读取线程
    // 若你在树莓派上使用的是 UART0，设备通常为 /dev/serial0 或 /dev/ttyAMA0
    std::thread depth_thread([](){
        depthReader("/dev/serial0", B115200);
        // 如需更换设备或波特率，改这里：depthReader("/dev/ttyAMA0", B9600);
    });
    depth_thread.detach();

    while (true)
    {

        if(navigationMaster.usvNavMenu.status_type == 1)
        {
            spdlog::info("经度:({0}) 纬度:({1}) 艏向:({2}) 横摇:({3}) 纵摇:({4}) 转艏速度:({5}) 东速:({6}) 北速:({7})) ",
                         navigationMaster.usvNavMenu.lon, navigationMaster.usvNavMenu.lat,
                         navigationMaster.usvNavMenu.yaw, navigationMaster.usvNavMenu.roll, navigationMaster.usvNavMenu.pitch,
                         navigationMaster.usvNavMenu.yaw_roti,navigationMaster.usvNavMenu.east_speed, navigationMaster.usvNavMenu.north_speed);

            spdlog::info("期望经度:({0}) 期望纬度:({1}) 位置偏差:({2})  期望速度:{4} \r\n 期望艏向:({5}) 期望俯仰:({6})) 期望横滚:({7}) ",
                         controlMasterClass.targetMenu.lon, controlMasterClass.targetMenu.lat,
                         YGISTools::pointToDistance(navigationMaster.usvNavMenu.lon,navigationMaster.usvNavMenu.lat,
                                             controlMasterClass.targetMenu.lon, controlMasterClass.targetMenu.lat),
                         controlMasterClass.targetMenu.yaw, controlMasterClass.targetMenu.speed);
        }
        else
        {
            spdlog::info("USV主控掉线");
        }

        spdlog::info("机器人运动模式:({0:d}) 机器人警告:({1:d})",
                     controlMasterClass.robotNowMode.mission_mode, controlMasterClass.robotNowMode.warning_type);

        sleep(1);
    }
}
