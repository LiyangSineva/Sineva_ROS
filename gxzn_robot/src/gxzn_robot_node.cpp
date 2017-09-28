//Sineva  lee8871@126.com
#include "ros/ros.h"
#include "gxzn_robot/gxzn.hpp"


#include "tf/transform_broadcaster.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/BatteryState.h"
#include "geometry_msgs/Twist.h"

using std::string;
using namespace serial;
using namespace gxzn;


Gxzn *robot;
ros::NodeHandle* nh;
//-------------------------------------------------------------------------------------------------------------
//                                         battery publisher function
sensor_msgs::BatteryState battery_state;
ros::Publisher bat_pub;
void batteryStateInit(void)
{
    bat_pub = nh->advertise<sensor_msgs::BatteryState>("battery_state", 3);
    battery_state.current = NAN;
    battery_state.charge = NAN;
    battery_state.capacity = NAN;
    battery_state.design_capacity = NAN;
    battery_state.percentage = NAN;
    battery_state.power_supply_status = battery_state.POWER_SUPPLY_STATUS_DISCHARGING;
    battery_state.power_supply_health = battery_state.POWER_SUPPLY_HEALTH_GOOD;
    battery_state.power_supply_technology = battery_state.POWER_SUPPLY_TECHNOLOGY_LIFE;
    battery_state.present = true;
    battery_state.location = "Blue box in AGV";
    battery_state.serial_number = "24V";
}


void battertStateLoop(void)
{
	robot->readBattery();
	battery_state.voltage = robot->get_battery();
	bat_pub.publish(battery_state);
}



//-------------------------------------------------------------------------------------------------------------
//--------------------------------------odometry publisher function

ros::Publisher odom_pub;
tf::TransformBroadcaster * odom_broadcaster;
nav_msgs::Odometry odometry;
geometry_msgs::TransformStamped odom_tf;

void odometryInit(void)
{
    odom_pub = nh->advertise<nav_msgs::Odometry>("odometry", 50);
    odom_broadcaster = new tf::TransformBroadcaster();
    odometry.header.frame_id = "Global";
    odometry.child_frame_id = "base_link";
    odom_tf.header.frame_id = "odom";
    odom_tf.child_frame_id = "base_link";
}
void odometryLoop(void)
{
	ros::Time current_time;
	current_time = ros::Time::now();

	robot->readMoveDistance();
	odometry.header.stamp = current_time;
	odometry.pose.pose.position.x = robot->get_x();
	odometry.pose.pose.position.y = robot->get_y();
	odometry.pose.pose.position.z = 0;
	//odometry.pose.pose.orientation.x = 0;
	//odometry.pose.pose.orientation.y = 0;
	//odometry.pose.pose.orientation.z = cos(robot->get_pitch() / 2);
	//odometry.pose.pose.orientation.w = sin(robot->get_pitch() / 2);
	odometry.pose.pose.orientation = tf::createQuaternionMsgFromYaw(robot->get_pitch());

	odom_tf.transform.translation.x = odometry.pose.pose.position.x;
	odom_tf.transform.translation.y = odometry.pose.pose.position.y;
	odom_tf.transform.translation.z = odometry.pose.pose.position.z;
	odom_tf.transform.rotation.x = odometry.pose.pose.orientation.x;
	odom_tf.transform.rotation.y = odometry.pose.pose.orientation.y;
	odom_tf.transform.rotation.z = odometry.pose.pose.orientation.z;
	odom_tf.transform.rotation.w = odometry.pose.pose.orientation.w;
	odom_tf.header.stamp = current_time;

	odom_broadcaster->sendTransform(odom_tf);
    odom_pub.publish(odometry);

}


//-------------------------------------------------------------------------------------------------------------
//                                         move functions

ros::Subscriber sub;
int Lose_ctrl_conter = 0;
void moveInit(void)
{
    void onCmdRawReceived(const geometry_msgs::Twist& msg);
    sub = nh->subscribe("cmd_raw",1000,&onCmdRawReceived);
}
void moveLoop(void)
{
    ros::spinOnce();
    Lose_ctrl_conter--;
    if(Lose_ctrl_conter == 0)
    {//If I can not receive next message, I lose.
    	robot->setSpeedVW(0, 0);
    }
    robot->writeMotorSpeed();
}
void onCmdRawReceived(const geometry_msgs::Twist& msg)
{
	Lose_ctrl_conter = 50;
	robot->setSpeedVW(msg.linear.x, msg.angular.z);
}


int main(int argc, char * argv[]) {

    ros::init(argc, argv, "gxzn");
    nh = new ros::NodeHandle();
    if(nh == NULL)
    {
    	return 0;
    }
    //robot initialization
    string port = "/dev/ttyUSB0";
    robot = new Gxzn(port);
    if(nh == NULL)
    {
    	delete nh;
    	return 0;
    }
	robot->setSpeedVW(0, 0);


	batteryStateInit();
	moveInit();
	odometryInit();


	int move_counter = 0;
	int odometry_counter = 0;
	int battery_counter = 0;

	ros::Rate rate(100);
    while(ros::ok())
    {
    	if(battery_counter <= 0)
    	{
    		battertStateLoop();
    		battery_counter = 200;
    	}
    	if(odometry_counter <= 0)
    	{
    		odometryLoop();
    		odometry_counter = 1;
    	}
    	if(move_counter <= 0)
    	{
    		moveLoop();
    		move_counter = 2;
    	}
    	move_counter--;
    	odometry_counter--;
    	battery_counter--;
        rate.sleep();
    }
	ROS_INFO_STREAM("The cow is launched!!!\n");
   // delete robot;
	   // delete odom_broadcaster;
  //  delete nh;
    return 0;
}

