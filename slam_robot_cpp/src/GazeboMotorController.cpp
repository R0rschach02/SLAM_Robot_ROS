#include "GazeboMotorController.hpp"


GazeboMotorController::GazeboMotorController(rclcpp::Node::SharedPtr node) {
    publisher_ = node->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/pan_tilt_controller/commands", 10
    );
    current_angles_ = {0.0, 0.0};
}


void GazeboMotorController::setAngles(double pan, double tilt) {
    auto msg = std_msgs::msg::Float64MultiArray();
    msg.data = {pan, tilt};
    publisher_->publish(msg);
    current_angles_ = {pan, tilt};
}         

JointAngles GazeboMotorController::getCurrentAngles()
{
    return current_angles_;
}