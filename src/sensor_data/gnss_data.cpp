#include "draw_gps_path/sensor_data/gnss_data.hpp"
#include "glog/logging.h"

//静态成员变量必须在类外初始化
bool draw_gps_path::GNSSData::origin_position_inited = false;
GeographicLib::LocalCartesian draw_gps_path::GNSSData::geo_converter;

namespace draw_gps_path {

void GNSSData::InitOriginPosition() {
    geo_converter.Reset(latitude, longitude, altitude);
    origin_position_inited = true;
}

void GNSSData::UpdateXYZ() {
    if (!origin_position_inited) {
        LOG(WARNING) << "GeoConverter has not set origin position";
    }
    geo_converter.Forward(latitude, longitude, altitude, local_E, local_N, local_U);
}
}