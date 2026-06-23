#pragma once

#include <optional>

namespace slam_robot {

struct DistanceReading {
    double x; 
    double y;
    double z;
    double distance_m;
    double timestamp_sec;
    bool valid; 
}; 


class IDistanceSensor {

public: 

virtual std::optional<DistanceReading> getLatestReading() const = 0;


virtual ~IDistanceSensor() = default;

};



}