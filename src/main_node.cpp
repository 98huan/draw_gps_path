#include <math.h>
#include <ros/ros.h>
#include <sys/stat.h>
#include <nav_msgs/Path.h>
#include "glog/logging.h"

#include "draw_gps_path/global_defination/global_defination.h"
#include "draw_gps_path/subscriber/gnss_subscriber.hpp"
using namespace draw_gps_path;

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);
    FLAGS_log_dir = WORK_SPACE_PATH + "/Log";
    FLAGS_alsologtostderr = 1;  //让日志同时输出到终端
    if (access(FLAGS_log_dir.data(), F_OK) < 0) {
        LOG_IF(FATAL, (mkdir(FLAGS_log_dir.data(), S_IRWXU) < 0)) << "create folder error: " << FLAGS_log_dir;
    }

    ros::init(argc, argv, "test_frame_node");
    ros::NodeHandle nh;

    std::shared_ptr<GNSSSubscriber> gnss_sub_ptr = std::make_shared<GNSSSubscriber>(nh, "/odometry/gpsz", 1000000); //订阅GPS的话题
    ros::Publisher gps_start_point_pub_ = nh.advertise<sensor_msgs::NavSatFix>("/ref_point_wgs84", 100, "map"); //发送给卫星地图接收
    ros::Publisher gps_path_pub_ = nh.advertise<nav_msgs::Path>("gps_path", 10);
    
    nav_msgs::Path gps_path;
    geometry_msgs::PoseStamped pose;
    std::deque<GNSSData> gnss_data_buff;
    sensor_msgs::NavSatFix gps_start_point; //gps起点

    bool gnss_origin_position_inited = false;

    ros::Rate rate(100);
    while (ros::ok())
    {
        ros::spinOnce();
        gnss_sub_ptr->ParseData(gnss_data_buff);

        while (gnss_data_buff.size() > 0)
        {
            GNSSData gnss_data = gnss_data_buff.front();
            gnss_data_buff.pop_front();

            // gps初始化
            if (!gnss_origin_position_inited)
            {
                gnss_data.InitOriginPosition();

                gps_start_point.header.stamp = ros::Time::now();
                gps_start_point.header.frame_id = "map";
                gps_start_point.latitude = gnss_data.latitude;
                gps_start_point.longitude = gnss_data.longitude;
                gps_start_point.altitude = gnss_data.altitude;
                gps_start_point.status.service = 0;

                gnss_origin_position_inited = true;
            }
            // gps相对位置更新
            gnss_data.UpdateXYZ();

            gps_path.header.frame_id = "map";
            gps_path.header.stamp = ros::Time::now();
            pose.header = gps_path.header;
            pose.pose.position.x = gnss_data.local_E;
            pose.pose.position.y = gnss_data.local_N;
            pose.pose.position.z = gnss_data.local_U;
            gps_path.poses.push_back(pose);
            // LOG(INFO) << gnss_data.local_E << "," << gnss_data.local_N << "," << gnss_data.local_U;
            gps_start_point_pub_.publish(gps_start_point);
            gps_path_pub_.publish(gps_path);
        }
        rate.sleep();
    }
    return 0;
}