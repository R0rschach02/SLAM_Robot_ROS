import re
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, RegisterEventHandler
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch.event_handlers import OnProcessExit
import os
import xacro
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    share_dir = get_package_share_directory('slam_robot')

    # URDF verarbeiten
    xacro_file = os.path.join(share_dir, 'model', 'slam_robot.xacro')
    robot_description_config = xacro.process_file(xacro_file)
    robot_urdf = robot_description_config.toxml()

    # Workaround: XML-Header und Kommentare entfernen
    robot_urdf = re.sub(r'<\?xml[^>]*\?>', '', robot_urdf)
    robot_urdf = re.sub(r'<!--.*?-->', '', robot_urdf, flags=re.DOTALL)
    robot_urdf = re.sub(r'\s+', ' ', robot_urdf).strip()

    # Robot State Publisher
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[{'robot_description': robot_urdf}]
    )

    # Gazebo Server
    gazebo_server = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('gazebo_ros'),
                'launch',
                'gzserver.launch.py'
            ])
        ])
    )

    # Gazebo Client (GUI)
    gazebo_client = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('gazebo_ros'),
                'launch',
                'gzclient.launch.py'
            ])
        ])
    )

    # Roboter in Gazebo spawnen
    urdf_spawn_node = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=['-entity', 'slam_robot', '-topic', 'robot_description'],
        output='screen'
    )

    # ===========================================
    # TWIST STAMPER
    # Konvertiert /cmd_vel (Twist) → /diff_drive_controller/cmd_vel (TwistStamped)
    # ===========================================
    twist_stamper_node = Node(
        package='twist_stamper',
        executable='twist_stamper',
        name='twist_stamper',
        remappings=[
            ('/cmd_vel_in', '/cmd_vel'),
            ('/cmd_vel_out', '/diff_drive_controller/cmd_vel'),
        ],
        output='screen'
    )

    # Controller sequenziell spawnen
    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster', '--controller-manager', '/controller_manager'],
        output='screen'
    )

    diff_drive_controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['diff_drive_controller', '--controller-manager', '/controller_manager'],
        output='screen'
    )

    pan_tilt_controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['pan_tilt_controller', '--controller-manager', '/controller_manager'],
        output='screen'
    )

    return LaunchDescription([
        gazebo_server,
        gazebo_client,
        robot_state_publisher_node,
        urdf_spawn_node,
        twist_stamper_node,  # <-- NEU
        RegisterEventHandler(
            OnProcessExit(
                target_action=urdf_spawn_node,
                on_exit=[joint_state_broadcaster_spawner]
            )
        ),
        RegisterEventHandler(
            OnProcessExit(
                target_action=joint_state_broadcaster_spawner,
                on_exit=[diff_drive_controller_spawner]
            )
        ),
        RegisterEventHandler(
            OnProcessExit(
                target_action=diff_drive_controller_spawner,
                on_exit=[pan_tilt_controller_spawner]
            )
        ),
    ])