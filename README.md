# draw_gps_path
## 配置
1、安装rviz_satellite
https://github.com/nobleo/rviz_satellite

## 编译
catkin_make -DCATKIN_WHITELIST_PACKAGES="draw_gps_path" -j1
## 运行
roslaunch draw_gps_path gps_path.launch

## 注意
可能是gps精度或者地图的精度导致gps轨迹会穿过花坛，跟实际不符，因此在rviz中给path的offset设成了X=2， Y=-0.5，是为了让显示的轨迹的与实际更贴近。