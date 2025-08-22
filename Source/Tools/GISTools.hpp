//
// @Time     : 2023/11/11 10:28
// @Author   : Yang
// @Software : CLion
// 地理坐标计算

#ifndef AUV_GISTOOLS_HPP
#define AUV_GISTOOLS_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include "assert.h"
#include "math.h"
#include <cmath>
#include<cstring>


namespace YGISTools
{
    /** 地球半径 **/
    inline float earth_radius = 6371939;

    /** 角度转弧度 **/
    inline double angleToRadian(double angle)
    {
        return angle * M_PI / 180;
    }

    /** 弧度转角度 **/
    inline double radianToAngle(double radian)
    {
        return radian / M_PI * 180;
    }

    /** 经纬度惯导格式转角度格式 **/
    inline double insToAngle(double one_ins_value)
    {
        //转成正常格式
        one_ins_value = one_ins_value / 100;
        // 度
        double one_angle =  (int)one_ins_value;
        // 分
        double one_min = one_ins_value - one_angle;
        // 纯角度
        double back_angle = one_angle + one_min / 60 * 100;

        return back_angle;
    }

//    /** 两经纬度计算距离 **/
//    inline float pointToDistance(double lon_start, double lat_start, double lon_end, double lat_end)
//    {
//        // 起始点
//        //double rad_lon_start = angleToRadian(insToAngle(lon_start));
//        //double rad_lat_start = angleToRadian(insToAngle(lat_start));
//        double rad_lon_start = angleToRadian(lon_start);
//        double rad_lat_start = angleToRadian(lat_start);
//        // 终点
//        //double rad_lon_end = angleToRadian(insToAngle(lon_end));
//        //double rad_lat_end = angleToRadian(insToAngle(lat_end));
//        double rad_lon_end = angleToRadian(lon_end);
//        double rad_lat_end = angleToRadian(lat_end);
//
//        float distance_m = acos(sin(rad_lat_start) * sin(rad_lat_end) +
//                cos(rad_lat_start)* cos(rad_lat_end)* cos(rad_lon_end - rad_lon_start)) * earth_radius;
//
//        return distance_m;
//    }

    /** 两点经纬度计算距离 粗估 受偏心影响**/
    inline double pointToDistance(double lonA, double latA, double lonB, double latB)
    {
        //double latARad = angleToRadian(insToAngle(latA));
        //double lonARad = angleToRadian(insToAngle(lonA));
        //double latBRad = angleToRadian(insToAngle(latB));
        //double lonBRad = angleToRadian(insToAngle(lonB));
        double latARad = angleToRadian(latA);
        double lonARad = angleToRadian(lonA);
        double latBRad = angleToRadian(latB);
        double lonBRad = angleToRadian(lonB);
        //纬度偏差
        double deltaLat = latBRad - latARad;
        //经度偏差
        double deltaLon = lonBRad - lonARad;

        double a = sin(deltaLat / 2.0) * sin(deltaLat / 2.0) +
                   cos(latARad) * cos(latBRad) *
                   sin(deltaLon / 2.0) * sin(deltaLon / 2.0);
        double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

        return earth_radius * c;
    }

    /** 简单判断差距 **/
    inline bool simpleDistanceCheck(double lonA, double latA, double lonB, double latB, float distance)
    {
        if (fabs(lonA-lonB) < distance and fabs(latA -latB)< distance)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /** (根据位置方向距离)计算目标点经纬度 **/
    inline void caltitude(double lon_start, double lat_start, double * lon_end, double * lat_end,
                          float angle, float distance)
    {
        // 起始点
        //double rad_lon_start = angleToRadian(insToAngle(lon_start));
        //double rad_lat_start = angleToRadian(insToAngle(lat_start));
        double rad_lon_start = angleToRadian(lon_start);
        double rad_lat_start = angleToRadian(lat_start);
        auto deg = (float)angleToRadian((double )angle);
        double res_lon =  distance * sin(deg)/(earth_radius*cos(rad_lat_start)*2*M_PI/360);
        double res_lat =  distance * cos(deg)/(earth_radius*2*M_PI/360);
        res_lon = lon_start + res_lon * 60; //度转分  需要优化
        res_lat = lat_start + res_lat * 60; //度转分  需要优化
        memcpy(lon_end, &res_lon, 8);
        memcpy(lat_end, &res_lat, 8);
    }

    /** 根据经纬度 计算目标点方向  方向值为真北**/
    inline double calculateBearing(double lonA, double latA, double lonB, double latB)
    {
        // 惯导
        //double latAAng = insToAngle(latA);
        //double lonAAng = insToAngle(lonA);
        //double latBAng = insToAngle(latB);
        //double lonBAng = insToAngle(lonB);
        // 弧度
        double latARad = angleToRadian(latA);
        double lonARad = angleToRadian(lonA);
        double latBRad = angleToRadian(latB);
        double lonBRad = angleToRadian(lonB);
        // 经度偏差
        double dLon = lonBRad - lonARad;

        double y = sin(dLon) * cos(latBRad);
        double x = cos(latARad) * sin(latBRad) - sin(latARad) * cos(latBRad) * cos(dLon);

        double bearing = atan2(y, x);
        bearing = fmod((bearing + 2 * M_PI), (2 * M_PI));

        return radianToAngle(bearing);
    }

    // 计算纵向和横向速度
    inline void calculateVelocities(float yaw, float east_speed, float north_speed, float &fore_speed, float &starboard_speed)
    {
        // 将艏向角度转换为弧度
        float theta = (float)(angleToRadian(yaw));
        // 纵向速度
        fore_speed = east_speed * cos(theta) + north_speed * sin(theta);
        // 横向速度
        starboard_speed = -east_speed * sin(theta) + north_speed * cos(theta);
    }

    /** 计算AB两点的斜率和截距 **/
    inline void lineEquation(double lonA, double latA, double lonB, double latB, double& slope, double& intercept)
    {
        // 计算斜率
        slope = (latB - latA) / (lonB - lonA);
        // 计算截距
        intercept = latA - slope * lonA;
    }

    // 计算点C到直线AB的距离
    inline double distanceToLineSegment(double lonC, double latC,
                                 double lonA, double latA,
                                 double lonB, double latB)
    {
        // 计算 A 到 C 和 B 到 C 的大圆弧距离
        double distAB = pointToDistance(lonA, latA, lonB, latB);
        double distAC = pointToDistance(lonA, latA, lonC, latC);
        double distBC = pointToDistance(lonB, latB, lonC, latC);
        // 计算 C 到 AB 线段的最短距离
        double s = (distAB + distAC + distBC) / 2;
        double area = sqrt(s * (s - distAB) * (s - distAC) * (s - distBC));
        return 2 * area / distAB;
    }


//    // 计算点C到直线AB的距离
//    double distanceToLine(double lonC, double latC,
//                          double lonA, double latA,
//                          double lonB, double latB)
//    {
//        // 斜率和截距
//        double slope_AB, intercept_AB;
//        // 计算直线AB的斜率和截距
//        lineEquation(lonA, latA, lonB, latB, slope_AB, intercept_AB);
//
//        // 计算点C到直线AB的距离
//        double distance = fabs(slope_AB * lonC - latC + intercept_AB) / sqrt(slope_AB * slope_AB + 1);
//
//        return distance;
//    }

    // 计算C点在AB直线上的 垂足经纬度
   inline void perpendicularFoot(double lonA, double latA,
                           double lonB, double latB,
                           double lonC, double latC,
                           double &lonF, double &latF)
    {
        // 转换为弧度
        latA = angleToRadian(latA);
        lonA = angleToRadian(lonA);
        latB = angleToRadian(latB);
        lonB = angleToRadian(lonB);
        latC = angleToRadian(latC);
        lonC = angleToRadian(lonC);

        // 使用球面余弦定理计算方向
        double dBAC = acos(sin(latC) * sin(latA) + cos(latC) * cos(latA) * cos(lonC - lonA));
        double dBAB = acos(sin(latB) * sin(latA) + cos(latB) * cos(latA) * cos(lonB - lonA));
        double x = acos((cos(dBAC) - cos(dBAB) * cos(dBAC)) / (sin(dBAB) * sin(dBAC)));
        // C点到垂足的距离
        double dAF = dBAB * cos(x);

        // 计算垂足坐标
        double latF_rad = asin(sin(latA) * cos(dAF) + cos(latA) * sin(dAF) * cos(lonB - lonA));
        double lonF_rad = lonA + atan2(sin(lonB - lonA) * sin(dAF) * cos(latA), cos(dAF) - sin(latA) * sin(latF_rad));

        //垂足经纬度
        latF = radianToAngle(latF_rad);
        lonF = radianToAngle(lonF_rad);
    }

    // 求解圆与直线的交点
   inline bool circleLineIntersection(double lonC, double latC, double radius,
                                double lonA, double latA,
                                double lonB, double latB,
                                double *lon1, double *lat1,
                                double *lon2, double *lat2)
    {

        // 圆心C的经纬度lonC latC

        // 计算直线AB的斜率和截距
        double slope_AB, intercept_AB;
        lineEquation(lonA, latA, lonB, latB, slope_AB, intercept_AB);

        // 如果直线 AB 为竖直线的特殊情况
        if (std::isinf(slope_AB))
        {
            // 计算与竖直线相切的两个点的经度
            *lon1 = lonC + radius;
            *lon2 = lonC - radius;
            // 计算对应的纬度
            *lat1 = asin(sin(latC) + (radius / earth_radius));
            *lat2 = asin(sin(latC) - (radius / earth_radius));
        }
        else
        {
            // 计算 A, B, C 三点形成的直线方程
            double a = 1 + pow(slope_AB, 2);
            double b = 2 * (slope_AB * intercept_AB - slope_AB * latC - lonC);
            double c = pow(latC, 2) + pow(lonC, 2) - pow(radius, 2) + pow(intercept_AB, 2) - 2 * intercept_AB * latC;

            // 计算解的判别式
            double delta = pow(b, 2) - 4 * a * c;

            // 计算交点的经纬度
            *lon1 = (-b + sqrt(delta)) / (2 * a);
            *lon2 = (-b - sqrt(delta)) / (2 * a);
            *lat1 = slope_AB * *lon1 + intercept_AB;
            *lat2 = slope_AB * *lon2 + intercept_AB;
        }

        // 将弧度转换为度
        *lon1 = radianToAngle(*lon1);
        *lat1 = radianToAngle(*lat1);
        *lon2 = radianToAngle(*lon2);
        *lat2 = radianToAngle(*lat2);

        return true;
    }

//    // 点C与直线AB的 焦点经纬度
//    void intersectionPoint(double lonC,double  latC, double radius,
//                           double slope_AB, double intercept_AB)
//    {
//        // 圆心经纬度 lonC latC
//        // 直线的斜率 slope_AB  截距 intercept_AB
//        // 解二次方程，求交点
//        // 设交点为(x, y)，代入圆的方程和直线的方程，得到：
//        // (x - lonC)^2 + (y - latC)^2 = R^2
//        // y = slope_AB * x + intercept_AB
//        // 代入得：
//        // (1 + slope_AB^2) * x^2 + 2 * (intercept_AB - latC - slope_AB * lonC) * x + (lonC^2 + (intercept_AB - latC)^2 - R^2) = 0
//
//        double a = 1 + slope_AB * slope_AB;
//        double b = 2 * (intercept_AB - latC - slope_AB * lonC);
//        double c = lonC * lonC + (intercept_AB - latC) * (intercept_AB - latC) - R * R;
//
//        // 求解二次方程
//        double discriminant = b * b - 4 * a * c;
//        if (discriminant >= 0)
//        {
//            double x1 = (-b + sqrt(discriminant)) / (2 * a);
//            double y1 = slope_AB * x1 + intercept_AB;
//            intersection.longitude = x1;
//            intersection.latitude = y1;
//        }
//        else
//        {
//            std::cerr << "No intersection found!" << std::endl;
//        }
//
//        return intersection;
//    }

}

#endif //AUV_GISTOOLS_HPP