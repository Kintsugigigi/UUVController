//
// @Time     : 2024/5/29 15:19
// @Author   : Yang
// @Software : CLion
//

#include "LogPIDClass.h"
#include "../Tools/GISTools.hpp"
#include "iomanip"

LogPIDClass::LogPIDClass()
{

}

LogPIDClass::~LogPIDClass()
{
    logfile.close();
}

void LogPIDClass::startLog()
{
    /** 文件名加时间 **/
    time_t tm;
    time(&tm);
    char tmp[64];
    strftime(tmp, sizeof(tmp),"%Y_%m_%d_%H_%M_%S", localtime(&tm));
    std::string time_str(tmp);
    std::string file_name = "/home/auv/Public/auv/Tmp/LogData/";
    file_name = file_name + time_str + "PID.txt";
    logfile.open(file_name, std::ios::app);

    if (logfile.is_open())
    {
        std::string log_head = "时间, 当前值, 目标值, PID目标值,,输出值, 比例项, "
                               "积分项, 微分项, 比例值, 积分值,"
                               "微分值 \r\n";
        logfile << log_head  << std::endl;
    }
}


