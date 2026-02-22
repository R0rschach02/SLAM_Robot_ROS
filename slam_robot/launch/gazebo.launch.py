from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
import os
import xacro
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    share_dir = get_package_share_directory('slam_robot')

    xacro_file = os.path.join(share_dir, 'model', 'slam_robot.xacro')
    robot_description_config = xacro.process_file(xacro_file)
    robot_urdf = robot_description_config.toxml()

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        parameters=[
            {'robot_description': robot_urdf}
        ]
    )

    gazebo_server = IncludeLaunchDescription(
    PythonLaunchDescriptionSource([
        PathJoinSubstitution([
            FindPackageShare('gazebo_ros'),
            'launch',
            'gzserver.launch.py'
        ])
    ])
)

    gazebo_client = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('gazebo_ros'),
                'launch',
                'gzclient.launch.py'
            ])
        ])
    )

    urdf_spawn_node = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        arguments=[
            '-entity', 'slam_robot',
            '-topic', 'robot_description'
        ],
        output='screen'
    )

    load_joint_state_broadcaster = Node(
    package='controller_manager',
    executable='spawner',
    arguments=['joint_state_broadcaster'],
    )

    load_pan_tilt_controller = Node(
    package='controller_manager',
    executable='spawner',
    arguments=['pan_tilt_controller'],
    )

    

    return LaunchDescription([
    gazebo_server,       # 1. Gazebo Server
    gazebo_client,       # 2. Gazebo Client  
    robot_state_publisher_node,  # 3. RSP
    urdf_spawn_node,     # 4. Roboter spawnen
    load_joint_state_broadcaster,  # 5. erst dann Controller
    load_pan_tilt_controller,      # 6.
])
