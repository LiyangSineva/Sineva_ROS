//Sineva  lee8871@126.com
#include "ros/ros.h"
#include "gxzn_robot/comport.hpp"


using std::string;
using namespace comport;

//Let's try to add some lines, try to push it to github lalalal

string readstr = "";

int main(int argc, char * argv[]) {

    ros::init(argc, argv, "com_test");
    ros::NodeHandle *nh = new ros::NodeHandle();

    //robot initialization
    string port = "/dev/ttyUSB0";
    Comport cp;
    cp.connect(port);
	ros::Rate rate(1);
    while(ros::ok())
    {
    	int a;
    	a = cp.writeStr("?C\r");
    	if(a!=0)
    	{
        	ROS_INFO_STREAM("write fail: "<<a);
    	}
        rate.sleep();


    	char buf[6400];


    	cp.readLine(buf,6400,'\r');
    	readstr = "";
    	readstr.append((char*)buf);
    	ROS_INFO_STREAM("---------------read line0------------------");
    	ROS_INFO_STREAM("read line0"<<readstr.substr(0, readstr.length()-1)  );


    	cp.readLine(buf,64,'\r');
    	readstr = "";
    	readstr.append((char*)buf);
    	ROS_INFO_STREAM("read line1"<<readstr.substr(0, readstr.length()-1)  );


    	cp.readLine(buf,64,'\r');
    	readstr = "";
    	readstr.append((char*)buf);
    	ROS_INFO_STREAM("read line2"<<readstr.substr(0, readstr.length()-1)  );


    	cp.readLine(buf,64,'\r');
    	readstr = "";
    	readstr.append((char*)buf);
    	ROS_INFO_STREAM("read line3"<<readstr.substr(0, readstr.length()-1)  );


    	cp.readLine(buf,64,'\r');
    	readstr = "";
    	readstr.append((char*)buf);
    	ROS_INFO_STREAM("read line4"<<readstr.substr(0, readstr.length()-1)  );
    }
    return 0;
}



