#pragma once

#include "IMotorController.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp" 

class GazeboMotorController : public IMotorController {
public:
    explicit GazeboMotorController(rclcpp::Node::SharedPtr node);
    ~GazeboMotorController() override = default;

    void setAngles(double pan, double tilt) override;
    JointAngles getCurrentAngles() override;

private:
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
    JointAngles current_angles_;
};

