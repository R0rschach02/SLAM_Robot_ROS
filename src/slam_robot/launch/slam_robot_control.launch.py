from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # Gazebo Launch
    slam_robot_pkg = get_package_share_directory('slam_robot')
    gazebo_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(slam_robot_pkg, 'launch', 'gazebo.launch.py')
        )
    )

    # Joy Node (Controller Input)
    joy_node = Node(
        package='joy',
        executable='joy_node',
        name='joy_node',
        output='screen',
        parameters=[{'dev': '/dev/input/js0'}]
    )

    # Teleop Node (übersetzt Joy → cmd_vel)
    teleop_node = Node(
        package='game_controller',
        executable='teleop_node',  # Prüfen, dass es so in CMakeLists.txt installiert wurde
        name='teleop_node',
        output='screen'
    )

    return LaunchDescription([
        gazebo_launch,
        joy_node,
        teleop_node
    ])

