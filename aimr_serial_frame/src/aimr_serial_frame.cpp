#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>

#define CLOSE_MSG_LENGTH 53
#define LED_MSG_LENGTH 123
#define MSG_HEAD_ONE 17
#define MSG_HEAD_TWO 22
#define MSG_HEAD_THREE 27
#define LED_MSG_DATA 92

std_msgs::Bool g_btn_state;
std_msgs::Int32 g_led_state;

void serial_frame_callback(const std_msgs::String::ConstPtr& msg) {
    if(CLOSE_MSG_LENGTH == msg->data.length() && 0xaa == (unsigned char)msg->data[MSG_HEAD_ONE] && 0x05 == msg->data[MSG_HEAD_TWO] && 0x0d == msg->data[MSG_HEAD_THREE]) {
        g_btn_state.data = true;
    }
    if(LED_MSG_LENGTH == msg->data.length() && 0xaa == (unsigned char)msg->data[MSG_HEAD_ONE] && 0x13 == msg->data[MSG_HEAD_TWO] && 0x01 == msg->data[MSG_HEAD_THREE]) {
        g_led_state.data = (unsigned char)msg->data[LED_MSG_DATA];
    }
}

int main(int argc, char** argv) {
    //初始化节点
    ros::init(argc, argv, "aimr_serial_frame");
    //声明节点句柄
    ros::NodeHandle nh;

    //初始化参数
    std::string aimr_btn_state_topic, aimr_led_state_topic, serial_frame_topic;
    g_btn_state.data = false;
    g_led_state.data = 0;
    nh.param("aimr_btn_state_topic", aimr_btn_state_topic, std::string("/aimr_power/btn_state"));
    nh.param("aimr_led_state_topic", aimr_led_state_topic, std::string("/aimr_power/led_state"));
    nh.param("serial_frame_topic", serial_frame_topic, std::string("/aimr_power/serial_frame"));

    //发布主题
    ros::Publisher aimr_btn_state_pub = nh.advertise<std_msgs::Bool>(aimr_btn_state_topic, 10);
    ros::Publisher aimr_led_state_pub = nh.advertise<std_msgs::Int32>(aimr_led_state_topic, 10);
    ros::Subscriber aimr_serial_frame_sub = nh.subscribe(serial_frame_topic, 10, serial_frame_callback);

    ros::Rate loop_rate(1);
    while(ros::ok()) {
        aimr_btn_state_pub.publish(g_btn_state);
        aimr_led_state_pub.publish(g_led_state);
        g_btn_state.data = false;

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
