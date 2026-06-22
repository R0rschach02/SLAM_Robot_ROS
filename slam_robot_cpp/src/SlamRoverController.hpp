#pragma once

#include "rclcpp/rclcpp.hpp"
#include "WorkspaceSafety.hpp"

class SlamRoverController {
public:
    SlamRoverController(rclcpp::Node::SharedPtr node)
        : kinematics_(node),    // ← wird VOR dem Body gebaut, mit Parameter
          workspace_safety_()   // ← wird VOR dem Body gebaut, ohne Parameter
    {}

private:
    PanTiltKinematics kinematics_;   // ← lebt auf dem Stack der Hauptklasse
    WorkspaceSafety workspace_safety_;
};