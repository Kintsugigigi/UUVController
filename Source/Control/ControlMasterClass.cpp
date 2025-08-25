//
// @Time     : 2024/6/5 19:31
// @Author   : Yang
// @Software : CLion
//

#include "ControlMasterClass.h"
#include<cstring>
#include <cmath>


ControlMasterClass::ControlMasterClass() {
    memset(&robotNowMode, 0, sizeof(ControlHub::MissionModeType));
    memset(&targetMenu, 0, sizeof(ControlHub::RobotTargetType));
    memset(&limitMenu, 0, sizeof(ControlHub::RobotLimitType));

    // 限制
    limitMenu.roll = 30;
    limitMenu.pitch = 30;
    limitMenu.altitude = 0;
    limitMenu.left_thruster = 100;
    limitMenu.right_thruster = 100;
    limitMenu.lossTime = 2;

    //目标初始值
    targetMenu.lon = 0;
    targetMenu.lat = 0;
    targetMenu.yaw = 0;
    targetMenu.speed = 0.00;
}

ControlMasterClass::~ControlMasterClass() {
}

void ControlMasterClass::startController() {
    /** 遥控 **/
    remote_queue = &remoteControllerClass.remote_queue;
    remoteControllerClass.action_queue = action_queue;
    remoteControllerClass.robotNowMode = &robotNowMode;
    remoteControllerClass.limitMenu = &limitMenu;
    remoteControllerClass.targetMenu = &targetMenu;
    remoteControllerClass.startRemoteController();

    /** 运动解算 **/
    motionSolutionClass.robotNowMode = &robotNowMode;
    motionSolutionClass.solution_queue = &remoteControllerClass.solution_queue;
    motionSolutionClass.targetMenu = &targetMenu;
    motionSolutionClass.action_queue = action_queue;
    motionSolutionClass.startMotionSolution();

    /** 获取路径 线程 **/
    get_mission_thread = std::thread(ControlMasterClass::getMissionLoop, this);

    /** 路径 线程 **/
    path_thread = std::thread(ControlMasterClass::pathLoop, this);

    /** 看门狗 线程 **/
    watch_dog_thread = std::thread(ControlMasterClass::watchDogLoop, this);
}


/** 路径循环 **/
void ControlMasterClass::pathLoop(void *__this) {
    ControlMasterClass *_this = (ControlMasterClass *) __this;
    // 路径循环次数
    int loop_num = 0;
    //当前目标点
    int index = 0;
    int max_num = sizeof(_this->targetMenu.mission_path_ary) / sizeof(_this->targetMenu.
                      mission_path_ary[0]);
    while (true) {
        //如果模式发生变化，则清空子状态
        if (_this->robotLastMode.mission_mode != _this->robotNowMode.mission_mode) {
            //子状态清空
            _this->robotNowMode.mode_level = 0;
            index = 0;
            printf("模式{0}-切换成-模式{1}", _this->robotLastMode.mission_mode,
                   _this->robotNowMode.mission_mode);
        }

        if (_this->robotNowMode.mission_mode == ControlHub::ModePathPoint or
            _this->robotNowMode.mission_mode == ControlHub::ModePathLoop) {
            //            /** 切点 **/
            //            // 艏向误差
            //            float dis_er = abs(_this->insMenu->yaw - _this->targetMenu.yaw);
            //            printf("艏向误差 {0} ", dis_er);
            //            if (YGISTools::distance(_this->targetMenu.lon ,_this->targetMenu.lat,
            //                                    _this->insMenu->lon,_this->insMenu->lat) < 2)
            //            {
            //                printf("到达点 {0}", index);
            //                index += 1;
            //                if(index >= max_num)
            //                {
            //                    index = 0;
            //                }
            //            }
            //            // 判断点
            //            double  target_lon_now = _this->targetMenu.mission_path_ary[index * 2];
            //            double  target_lat_now = _this->targetMenu.mission_path_ary[index * 2 + 1];
            //            if(target_lon_now ==0 or target_lat_now == 0)
            //            {   //如果经纬度0 则选第一个点
            //                index = 0;
            //                target_lon_now = _this->targetMenu.mission_path_ary[index * 2];
            //                target_lat_now = _this->targetMenu.mission_path_ary[index * 2 + 1];
            //                printf("跑点艏向 {0}", _this->targetMenu.yaw);
            //            }
            //            // 设定目标点
            //            _this->targetMenu.lon = target_lon_now;
            //            _this->targetMenu.lat = target_lat_now;
            //
            //            // 设定艏向
            //            if((_this->robotNowMode.mission_mode == ControlHub::ModePathLoopSlow or
            //                _this->robotNowMode.mission_mode == ControlHub::ModePathLoopFast))
            //            {
            //                if(YGISTools::distance(_this->targetMenu.lon,_this->targetMenu.lat,
            //                                       _this->insMenu->lon,_this->insMenu->lat) < 2)
            //                {
            //                    printf("切换艏向");
            //                    //距离点近 改为定点模式
            //                    _this->robotNowMode.mode_level = 0;
            //
            //                    double target_bearing = YGISTools::calculateBearing( _this->insMenu->lon,_this->insMenu->lat,
            //                                                                         _this->targetMenu.lon,_this->targetMenu.lat);
            //                    //目标艏向
            //                    _this->targetMenu.yaw = (float)target_bearing;
            //                }
            //                else
            //                {
            //                    printf("慢速路径 2");
            //                    //距离目标点远 则修改艏向指向目标点
            //                    _this->robotNowMode.mode_level = 0;
            //                    double target_bearing = YGISTools::calculateBearing( _this->insMenu->lon,_this->insMenu->lat,
            //                                                                         _this->targetMenu.lon,_this->targetMenu.lat);
            //                    //目标艏向
            //                    _this->targetMenu.yaw = (float)target_bearing;
            //                }
            //            }
            //            printf("跑点艏向 {0}", _this->targetMenu.yaw);
        }
        //记录模式状态
        _this->robotLastMode.mission_mode = _this->robotNowMode.mission_mode;
        sleep(1);
    }
}

/** 任务循环函数 **/
void ControlMasterClass::getMissionLoop(void *__this) {
    ControlMasterClass *_this = (ControlMasterClass *) __this;
    int max_num = sizeof(_this->targetMenu.mission_path_ary) / sizeof(_this->targetMenu.
                      mission_path_ary[0]);
    while (true) {
        std::map get_map = _this->path_queue.take();

        double lon = 0; //经度
        double lat = 0; //纬度
        int index = 0; //点的位置
        int source = 0; //数据源 1:任务路径 2:归航路径 3:寻光路径

        if (get_map.count("lon") == 1) {
            lon = get_map["lon"];
        }
        if (get_map.count("lat") == 1) {
            lat = get_map["lat"];
        }
        if (get_map.count("index") == 1) {
            index = (int) get_map["index"];
            //printf(get_map["index"]);
        }
        if (get_map.count("source") == 1) {
            source = (int) get_map["source"];
        }
        //printf("获得的目标点----{0}-{1}-{2}-{3}-{4}-", source, lon, lat, index, max_num);

        if (index == -1) {
            if (_this->robotNowMode.mission_mode == ControlHub::ModeNoIdea or
                _this->robotNowMode.mission_mode == ControlHub::ModeStop) {
                _this->targetMenu.lon = lon;
                _this->targetMenu.lat = lat;
                //printf("设置了原点");
            }
        } else if (index == -2) {
            if (source == 1) {
                //任务路径 清空所有点
                memset(&_this->targetMenu.mission_path_ary, 0, sizeof(targetMenu.mission_path_ary));
                _this->targetMenu.mission_path_num = 0;
            } else if (source == 2) {
                //归航任务 清空所有点
                memset(&_this->targetMenu.back_path_ary, 0, sizeof(targetMenu.back_path_ary));
                _this->targetMenu.back_path_num = 0;
            } else if (source == 3) {
                //其他模式待定 清空所有点
            }
        } else {
            if (index >= 0 and index < max_num and lon > 0 and lat > 0) {
                if (source == 1) {
                    // 任务路径
                    if (_this->targetMenu.mission_path_num == index or
                        _this->targetMenu.mission_path_num + 1 == index) {
                        _this->targetMenu.mission_path_ary[index][0] = lon;
                        _this->targetMenu.mission_path_ary[index][1] = lat;
                        _this->targetMenu.mission_path_num = index;
                        //printf("设置了任务目标点{0}-", index);
                    }
                } else if (source == 2) {
                    // 返航路径
                    if (_this->targetMenu.back_path_num == index or
                        _this->targetMenu.back_path_num + 1 == index) {
                        _this->targetMenu.back_path_ary[index][0] = lon;
                        _this->targetMenu.back_path_ary[index][1] = lat;
                        _this->targetMenu.back_path_num = index;
                        //printf("设置了回程目标点{0}-", index);
                    }
                } else if (source == 3) {
                    // 其他模式待定
                }
                //printf("+++修改了路径{0}+++", index);
            }
        }
    }
}


void ControlMasterClass::watchDogLoop(void *__this) {
    ControlMasterClass *_this = (ControlMasterClass *) __this;
    bool limit_type = false; //限制标识
    float pitch_error = 0; //误差
    float roll_error = 0;
    float speed_error = 0;
    while (true) {
        //失联时间
        _this->robotNowMode.lonely_time++;
        // 超限判断
        limit_type = false;
        // 俯仰限制
        pitch_error = fabs(_this->usvNavMenu->pitch);
        if (pitch_error > _this->limitMenu.pitch) {
            limit_type = true;
            printf("俯仰超限{0}", _this->usvNavMenu->pitch);
        }
        //滚动限制
        roll_error = fabs(_this->usvNavMenu->roll);
        if (roll_error > _this->limitMenu.roll) {
            limit_type = true;
            printf("滚动超限{0}", _this->usvNavMenu->roll);
        }
        //速度限制
        speed_error = _this->usvNavMenu->fore_speed;
        if (speed_error > _this->limitMenu.speed) {
            limit_type = true;
            printf("速度超限{0}", _this->usvNavMenu->fore_speed);
        }

        //高度限制
        //if (_this->dvlMenu->distance_range < _this->limitMenu.altitude)
        //{
        //    limit_type = true;
        //    printf("高度超限{0}", _this->depthMenu->depth);
        //}
        // 越界警告
        if (limit_type) {
            _this->robotNowMode.warning_type = ControlHub::WarningLimit;
        } else {
            if (_this->robotNowMode.warning_type == ControlHub::WarningLimit) {
                _this->robotNowMode.warning_type = ControlHub::WarningNormal;
            }
        }
        // 增加计数
        sleep(1);
    }
}
