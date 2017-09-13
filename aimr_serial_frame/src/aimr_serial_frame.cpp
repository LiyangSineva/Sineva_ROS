#include <ros/ros.h>
#include <std_msgs/String.h>
#include <aimr_serial_frame/AimrState.h>

#define CLOSE_MSG_LENGTH 53
#define LED_MSG_LENGTH 123
#define MSG_HEAD_ONE 17
#define MSG_HEAD_TWO 22
#define MSG_HEAD_THREE 27
#define LED_MSG_DATA 92

aimr_serial_frame::AimrState g_aimr_state;

void serial_frame_callback(const std_msgs::String::ConstPtr& msg) {
    if(CLOSE_MSG_LENGTH == msg->data.length() && 0xaa == (unsigned char)msg->data[MSG_HEAD_ONE] && 0x05 == msg->data[MSG_HEAD_TWO] && 0x0d == msg->data[MSG_HEAD_THREE]) {
        g_aimr_state.btn_state = true;
    }
    if(LED_MSG_LENGTH == msg->data.length() && 0xaa == (unsigned char)msg->data[MSG_HEAD_ONE] && 0x13 == msg->data[MSG_HEAD_TWO] && 0x01 == msg->data[MSG_HEAD_THREE]) {
        g_aimr_state.led_state = (unsigned char)msg->data[LED_MSG_DATA];
    }
}

int main(int argc, char** argv) {
    //初始化节点
    ros::init(argc, argv, "aimr_serial_frame");
    //声明节点句柄
    ros::NodeHandle nh;

    //初始化参数
    std::string aimr_state_topic, serial_frame_topic;
    g_aimr_state.btn_state = false;
    g_aimr_state.led_state = 0;
    nh.param("aimr_state_topic", aimr_state_topic, std::string("/aimr_state"));
    nh.param("serial_frame_topic", serial_frame_topic, std::string("/aimr_power/serial_frame"));

    //发布主题
    ros::Publisher aimr_state_pub = nh.advertise<aimr_serial_frame::AimrState>(aimr_state_topic, 10);
    ros::Subscriber aimr_serial_frame_sub = nh.subscribe(serial_frame_topic, 10, serial_frame_callback);

    ros::Rate loop_rate(1);
    while(ros::ok()) {
        g_aimr_state.header.stamp = ros::Time::now();
        aimr_state_pub.publish(g_aimr_state);
        g_aimr_state.btn_state = false;

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
