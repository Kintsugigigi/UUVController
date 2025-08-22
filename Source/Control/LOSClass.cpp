//
// @Time     : 2024/6/6 17:39
// @Author   : Yang
// @Software : CLion
//

#include "LOSClass.h"
#include <cmath>
#include "../Tools/GISTools.hpp"


LOSClass::LOSClass()
{
    //目标经纬度
    target_lon = 0;
    target_lat = 0;
    //前一次目标经纬度
    last_lon = 0;
    last_lat = 0;
    //前视点
    los_lon = 0;
    los_lat = 0;

    // 跟踪点序号
    point_index = 0;
}

LOSClass::~LOSClass()
{

}

void LOSClass::startLosCompute(float yaw, float speed, double lon, double lat, int index)
{
    point_index = index;
    // 获取目标点
    target_lon = targetMenu->mission_path_ary[point_index][0];
    target_lat = targetMenu->mission_path_ary[point_index][1];

    if(index == 0 or last_lon == 0)
    {
        last_lon = usvNavMenu->lon;
        last_lat = usvNavMenu->lat;
    }

    // 到达目标点判断
    double distance = YGISTools::pointToDistance(usvNavMenu->lon,usvNavMenu->lat,
                                                 target_lon, target_lat);
    if(distance <= point_radius)
    {
        point_index = index + 1;

        last_lon = target_lon;
        last_lat = target_lat;
        // 获取目标点
        target_lon = targetMenu->mission_path_ary[point_index][0];
        target_lat = targetMenu->mission_path_ary[point_index][1];
    }

    double lonF, latF;
    YGISTools::perpendicularFoot(usvNavMenu->lon,usvNavMenu->lat,
                      last_lon, last_lat,
                      target_lon, target_lon,
                      lonF, latF);

    // 横向距离
    double dis_h = YGISTools::pointToDistance(usvNavMenu->lon,usvNavMenu->lat,
                                              lonF, latF);
    // 转弯半径
    if(dis_h >= turning_radius)
    {
        // 大于载体转弯半径 直接去向垂足
        targetMenu->los_lon = lonF;
        targetMenu->los_lat = latF;
    }
    else
    {
        // 小于于载体转弯半径 计算圆与路径交点
        double lon1 = 0;
        double lat1 = 0;
        double lon2 = 0;
        double lat2 = 0;
        // 计算 C为圆心turning_radius为半径的圆，与A B的直线的交点
        bool compute_rase = YGISTools::circleLineIntersection(usvNavMenu->lon,usvNavMenu->lat,turning_radius,
                                                              last_lon, last_lat,
                                                              target_lon, target_lat,
                                                              &lon1, &lat1, &lon2, &lat2);
        // 判断哪个点距离更近
        double distanceA = YGISTools::pointToDistance(lon1, lat1, target_lon, target_lat);
        double distanceB = YGISTools::pointToDistance(lon2, lat2, target_lon, target_lat);
        if(distanceA <= distanceB)
        {
            targetMenu->los_lon = lon1;
            targetMenu->los_lat = lat1;
        }
        else
        {
            targetMenu->los_lon = lon1;
            targetMenu->los_lat = lat1;
        }
    }

    if(los_lon != 0)
    {
        // 计算航向角
        double los_yaw = YGISTools::calculateBearing(usvNavMenu->lon,usvNavMenu->lat,
                                                     lonF, latF);
        targetMenu->control_yaw = (float)los_yaw;
    }
    else
    {
        printf("+++++未计算出前视角++++");
    }
    //target_yaw = computeOnce(usvNavMenu->yaw, usvNavMenu->fore_speed, usvNavMenu->lon, usvNavMenu->lat, last_lon, last_lat, target_lon, target_lat);
}

void LOSClass::computeOnce(float yaw, float speed, double robot_lon, double robot_lat, double last_lon, double last_lat, double target_lon, double target_lat)
{
    double foot_lon = 0;
    double foot_lat = 0;


    YGISTools::perpendicularFoot(robot_lon, robot_lat, last_lon, last_lat, target_lon, target_lat, foot_lon, foot_lat);


    double point_lon = 0;
    double point_lat = 0;
//    YGISTools::caltitude(lon, lat,point_lon,point_lat,)
}


