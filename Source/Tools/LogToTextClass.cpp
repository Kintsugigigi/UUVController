//
// @Time     : 2024/1/17 22:37
// @Author   : Yang
// @Software : CLion
//

#include "LogToTextClass.h"
#include "spdlog/spdlog.h"
#include "GISTools.hpp"
#include "iomanip"


LogToTextClass::LogToTextClass()
{

}

LogToTextClass::~LogToTextClass()
{
    logfile.close();
}

void LogToTextClass::startLog()
{
    /** 文件名加时间 **/
    time_t tm;
    time(&tm);
    char tmp[64];
    strftime(tmp, sizeof(tmp),"%Y_%m_%d_%H_%M_%S", localtime(&tm));
    std::string time_str(tmp);
    std::string file_name = "/home/auv/Public/auv/Tmp/LogData/";
    file_name = file_name + time_str + "data.txt";
    logfile.open(file_name, std::ios::app);
    //文件记录线程
    log_thread = std::thread(LogToTextClass::logMessageLoop, this);
}

void LogToTextClass::logMessageLoop(void * __this)
{
    LogToTextClass * _this =(LogToTextClass *)__this;
    if (_this->logfile.is_open())
    {
        std::string log_head = "时间, GPS经度, GPS纬度, 惯导经度, 惯导纬度, "
                               "原始GPS经度, 原始GPS纬度, 原始惯导经度, 原始惯导纬度,"
                               "惯导纵向速度, 惯导横向速度, DVL高度"
                               "深度, 艏向, 俯仰, 滚动 ,测距\r\n";
        _this->logfile << log_head  << std::endl;
    }
    while (true)
    {
        if (_this->logfile.is_open())
        {
//            // 获取当前时间
//            time_t now = time(nullptr);
//            char* timeString = ctime(&now);
//            // 去掉换行符
//            timeString[strcspn(timeString, "\n")] = '\0';
//            // 写入日志信息
//            _this->logfile<< std::fixed << std::setprecision(8) <<
//            timeString  <<  "," <<
//            YGISTools::insToAngle(_this->GPSMenu->lon) << "," <<
//            YGISTools::insToAngle(_this->GPSMenu->lat) << "," <<
//            YGISTools::insToAngle(_this->insMenu->lon) << "," <<
//            YGISTools::insToAngle(_this->insMenu->lat) << "," <<
//            _this->GPSMenu->lon << "," <<
//            _this->GPSMenu->lat << "," <<
//            _this->insMenu->lon << "," <<
//            _this->insMenu->lat << "," <<
//            _this->insMenu->longitudinal_speed << "," <<
//            _this->insMenu->transverse_speed << "," <<
//            _this->dvlMenu->speed_Longitudinal << "," <<
//            _this->dvlMenu->speed_Transverse << "," <<
//            _this->dvlMenu->distance_range << "," <<
//            _this->depthMenu->depth << "," <<
//            _this->insMenu->yaw << "," <<
//            _this->insMenu->pitch << "," <<
//            _this->insMenu->roll << "," <<
//            (float)_this->soundMenu->distance / 10 << "," <<
//            std::endl;
        }
        else
        {
            printf("打开记录文件失败");
        }
        // 记录间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(_this->interval_num));
    }
}
