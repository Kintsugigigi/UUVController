// DepthTestMain.cpp - 独立的串口读取测试，不依赖项目其它模块
#include <iostream>
#include <thread>
#include <string>
#ifdef __linux__
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <cstring>

static int openSerialPort(const char* device, speed_t baud)
{
    int fd = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "打开串口失败 " << device << " errno=" << errno << "\n";
        return -1;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

    termios tty{};
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "tcgetattr 失败 errno=" << errno << "\n";
        ::close(fd);
        return -1;
    }

    cfmakeraw(&tty);
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "tcsetattr 失败 errno=" << errno << "\n";
        ::close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    std::cout << "串口已打开 " << device << " 波特率已设置\n";
    return fd;
}

static void depthReader(const char* device, speed_t baud)
{
    int fd = openSerialPort(device, baud);
    if (fd < 0) return;

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
            std::cerr << "select 出错 errno=" << errno << "\n";
            break;
        } else if (rv == 0) {
            continue;
        }

        ssize_t n = ::read(fd, buf, sizeof(buf));
        if (n < 0) {
            if (errno == EAGAIN || errno == EINTR) continue;
            std::cerr << "串口读取失败 errno=" << errno << "\n";
            break;
        } else if (n == 0) {
            std::cerr << "串口被关闭\n";
            break;
        } else {
            for (ssize_t i = 0; i < n; ++i) {
                char c = buf[i];
                if (c == '\r') continue;
                if (c == '\n') {
                    if (!line.empty()) {
                        std::cout << "Depth RX: " << line << std::endl;
                        line.clear();
                    }
                } else {
                    line.push_back(c);
                }
            }
        }
    }

    ::close(fd);
    std::cout << "串口已关闭\n";
}
#endif

int main()
{
#ifndef __linux__
    std::cerr << "仅支持在 Linux 上运行（树莓派 Ubuntu）\n";
    return 1;
#else
    // 根据你的接线实际修改设备与波特率
    const char* dev = "/dev/serial0"; // 或 "/dev/ttyAMA0"
    speed_t baud = B115200;           // 根据深度计手册设置
    std::thread t([&](){ depthReader(dev, baud); });
    t.join(); // 简单起见，主线程等待读取线程结束
    return 0;
#endif
}