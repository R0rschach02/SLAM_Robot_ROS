#include <memory>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/LinearMath/Matrix3x3.h"

using std::placeholders::_1;

class DriveDebugger : public rclcpp::Node
{
public:
  DriveDebugger() : Node("drive_debugger"), 
    // WICHTIG: Initialisierung mit 0, aber dem KORREKTEN Clock-Type des Nodes
    last_cmd_time_(0, 0, RCL_ROS_TIME) 
  {
    // Log-Datei öffnen
    log_file_.open("drive_log.txt", std::ios::out | std::ios::trunc);
    if (log_file_.is_open()) {
        RCLCPP_INFO(this->get_logger(), "Log-Datei 'drive_log.txt' erfolgreich erstellt.");
        log_file_ << "Time_Sec; CMD_Lin_X; CMD_Ang_Z; ODOM_Lin_X; ODOM_Ang_Z; ODOM_Yaw; Comment\n";
    } else {
        RCLCPP_ERROR(this->get_logger(), "Konnte Log-Datei nicht erstellen!");
    }

    // Subscriber
    cmd_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "/cmd_vel", 10, std::bind(&DriveDebugger::cmd_callback, this, _1));

    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "/odom", 10, std::bind(&DriveDebugger::odom_callback, this, _1));

    // Timer
    timer_ = this->create_wall_timer(
      std::chrono::milliseconds(100), std::bind(&DriveDebugger::logging_loop, this));
      
    RCLCPP_INFO(this->get_logger(), "LOGGER LÄUFT. Warte auf Input...");
  }

  ~DriveDebugger() {
      if (log_file_.is_open()) {
          log_file_.close();
      }
  }

private:
  void cmd_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    latest_cmd_ = *msg;
    // Hier ist es sicher, now() zu rufen, da der Callback zur Laufzeit passiert
    last_cmd_time_ = this->now();
    cmd_active_ = true;
  }

  void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    current_linear_vel_ = msg->twist.twist.linear.x;
    current_angular_vel_ = msg->twist.twist.angular.z;

    tf2::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf2::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    current_yaw_ = yaw;
  }

  void logging_loop()
  {
    // FIX: Wir prüfen erst, ob überhaupt schon mal eine Zeit gesetzt wurde
    // und fangen Fehler bei Clock-Mismatch ab
    try {
        if (cmd_active_) {
            rclcpp::Duration diff = this->now() - last_cmd_time_;
            if (diff.seconds() > 0.5) {
                cmd_active_ = false;
                latest_cmd_.linear.x = 0.0;
                latest_cmd_.angular.z = 0.0;
            }
        }
    } catch (const std::runtime_error& e) {
        // Falls kurzzeitig die Uhren noch nicht synchron sind, ignorieren wir diesen Zyklus
        // statt abzustürzen.
        return; 
    }

    if (log_file_.is_open()) {
        double time_now = 0.0;
        try {
            time_now = this->now().seconds();
        } catch (...) {
            time_now = 0.0; 
        }
        
        std::string comment = "OK";
        if (std::abs(latest_cmd_.angular.z) > 0.1 && std::abs(current_angular_vel_) < 0.05) {
            comment = "BLOCKADE_DETECTED";
        }

        log_file_ << std::fixed << std::setprecision(3)
                  << time_now << "; " 
                  << latest_cmd_.linear.x << "; " 
                  << latest_cmd_.angular.z << "; "
                  << current_linear_vel_ << "; " 
                  << current_angular_vel_ << "; "
                  << current_yaw_ << "; "
                  << comment << "\n";
    }
  }

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_sub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp::TimerBase::SharedPtr timer_;
  
  std::ofstream log_file_;
  geometry_msgs::msg::Twist latest_cmd_;
  rclcpp::Time last_cmd_time_;
  
  double current_linear_vel_ = 0.0;
  double current_angular_vel_ = 0.0;
  double current_yaw_ = 0.0;
  bool cmd_active_ = false;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DriveDebugger>());
  rclcpp::shutdown();
  return 0;
}