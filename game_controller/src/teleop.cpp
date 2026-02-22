#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/joy.hpp"

class TeleopNode : public rclcpp::Node {
public:
    TeleopNode() : Node("teleop_node") {
        // Publisher für Roboter-Geschwindigkeit
        cmd_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        // Subscriber für Controller Input
        joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
            "joy", 10, std::bind(&TeleopNode::joy_callback, this, std::placeholders::_1));

        // Skalen für Geschwindigkeit
        linear_scale_ = 1.0;  // m/s
        angular_scale_ = 1.0; // rad/s
    }

private:
    void joy_callback(const sensor_msgs::msg::Joy::SharedPtr joy_msg) {
        auto twist = geometry_msgs::msg::Twist();
        // Beispiel: links Stick vertikal -> linear x, links Stick horizontal -> linear y
        twist.linear.x = linear_scale_ * joy_msg->axes[1];  // Forward/backward
        twist.linear.y = linear_scale_ * joy_msg->axes[0];  // Strafing (falls Differentialdrive evtl. 0)
        twist.angular.z = angular_scale_ * joy_msg->axes[3]; // Right Stick horizontal -> Drehung

        cmd_pub_->publish(twist);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
    double linear_scale_;
    double angular_scale_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TeleopNode>());
    rclcpp::shutdown();
    return 0;
}
