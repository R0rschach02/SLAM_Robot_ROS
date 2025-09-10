#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

class CmdVelListener : public rclcpp::Node {
public:
  CmdVelListener(): Node("cmd_vel_listener") {
    sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel", 10,
      [this](geometry_msgs::msg::Twist::UniquePtr msg) {
        RCLCPP_INFO(this->get_logger(),
                    "CmdVel received: linear x=%.3f angular z=%.3f",
                    msg->linear.x, msg->angular.z);
      });
  }
private:
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CmdVelListener>());
  rclcpp::shutdown();
  return 0;
}
