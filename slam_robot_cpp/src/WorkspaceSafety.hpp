#pragma once
#include <Eigen/Dense>

class WorkspaceSafety {
public:
    void setChassisBox(Eigen::Vector3d min, Eigen::Vector3d max);
    Eigen::Vector3d getLinkPosition(double pan, double tilt);
    bool isWorkspaceSafe(double pan, double tilt);

private:
    // Gelenk-Offsets aus URDF (in Metern)
    const Eigen::Vector3d PAN_ORIGIN  {0.0101, -0.01,    0.0126};
    const Eigen::Vector3d TILT_ORIGIN {-0.0195, -0.00575, -0.00576};
    const Eigen::Vector3d SENSOR_TIP  {0.019,  -0.05,   -0.000072};
    const Eigen::Vector3d chassis_min{-0.077, -0.055, 0.0};
    const Eigen::Vector3d chassis_max{ 0.077,  0.055, 0.054};
    

};

    safety.setChassisBox(
    Eigen::Vector3d(-0.077, -0.055, 0.0),   // min
    Eigen::Vector3d( 0.077,  0.055, 0.054)  // max
    );