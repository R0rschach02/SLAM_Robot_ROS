cmake_minimum_required(VERSION 3.8)
project(slam_robot_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Abhängigkeiten finden
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)   # WICHTIG: Für Odometry Messages
find_package(tf2 REQUIRED)        # WICHTIG: Für Quaternion/Matrix Mathe
find_package(Eigen3 REQUIRED)
find_package(std_msgs REQUIRED)

# Neues Executable für den Pan-Tilt Node
add_executable(pan_tilt_node
  src/pan_tilt_node.cpp
  src/GazeboMotorController.cpp
  src/WorkspaceSafety.cpp
)

# Eigen3 Header einbinden
target_include_directories(pan_tilt_node PUBLIC
  include/
  ${EIGEN3_INCLUDE_DIR}
)

# Abhängigkeiten verknüpfen
ament_target_dependencies(pan_tilt_node
  rclcpp
  std_msgs
  geometry_msgs
)

# Falls du den cmd_vel_listener auch noch brauchst, musst du ihn SEPARAT definieren:
# add_executable(cmd_vel_listener src/cmd_vel_listener.cpp)
# ament_target_dependencies(cmd_vel_listener rclcpp geometry_msgs)

# 4. Installieren (Damit 'ros2 run' es findet)
install(TARGETS
  drive_debugger
  pan_tilt_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()