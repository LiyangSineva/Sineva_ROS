//Sineva  lee8871@126.com 

#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "serial/serial.h"
#include "gxzn_robot/gxzn.hpp"
#include "string"

using std::string;
using namespace serial;

namespace gxzn
{
	//base method
	Gxzn::Gxzn(string uart_path)
	{
		port = uart_path;
		my_serial = new Serial(port,115200,Timeout::simpleTimeout(1000));
		speed_set_l = 0;
		speed_set_r = 0;
		l_previous = 0;
		r_previous = 0;
        velocity = 0;
		angulat_velocity = 0;
		x = 0;
		y = 0;
		pitch = 0;
		battery = NAN;
		is_first_value = true;
	}

	Gxzn::~Gxzn()
	{
		if(my_serial!=NULL)
		{
			delete my_serial;
		}
		return;
	}

	int Gxzn::isConnected()
	{
		if(my_serial->isOpen())
		{
			string result;
			my_serial->write("?v 2/n");
			my_serial->readline(result,64);
			if(result.substr(0,2) == "V=")
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	//--------------------  function line 1  ------------------------------
	//user set speed send to motor;
	int Gxzn::setSpeedRL(double l,double r)
	{
		speed_set_l = mps2int(l);
		speed_set_r = mps2int(r);
		return 0;
	}
	int Gxzn::setSpeedVW(double v,double w)
	{
		/*
		double middle =w * Foots_distance *0.5;
		speed_set_l = mps2int(v-middle);
		speed_set_r = mps2int(v+middle);
		*/

		speed_set_r = v*-120;
		speed_set_l = w*80;

		return 0;
	}
	//transport m/s to int for serial.
	int Gxzn::mps2int(double l)
	{
		int speed;
		speed = (int)(l*Unit_per_meter);
		if(speed>=1000)
		{
			ROS_WARN("Right speed is set > 1000, 1000 will be use");
			speed = 1000;
		}
		if(speed<=-1000)
		{
			ROS_WARN("Right speed is set < -1000, -1000 will be use");
			speed = -1000;
		}
		return speed;
	}
	//sent to motor
	int Gxzn::writeMotorSpeed()
	{
		char temp[64];
		sprintf(temp,"!m %d %d\r",speed_set_l, speed_set_r);
		my_serial->write(temp);
	    string result = "\0";
		//ROS_INFO_STREAM("232 Send: "<<temp);
	}
	//--------------------  function line 2  ------------------------------
	//receive speed from serial

	int Gxzn::readMoveDistance()
	{
		my_serial->flushInput();
		char temp[64] = "?C\r";
		my_serial->write(temp);
	    string result_tilte,result_value;
	    my_serial->readline(result_tilte,64,"\r");
	    my_serial->readline(result_value,64,"\r");
		//ROS_INFO_STREAM("odometry :\nsent:\n"<<temp<<"\nreceive:\n"<<result_value<<"\n");



	    int32_t r_current, l_current;
        sscanf(result_value.c_str(),"C=%d:%d\r",&r_current,&l_current);
        r_current = -r_current;
        ROS_INFO_STREAM("values is "<<l_current<<" : "<<r_current);
        //calculate
        int32_t r_diff = r_current -r_previous;//因为int具有循环性,溢出后的值仍然是准确的.
        int32_t l_diff = l_current -l_previous;//因为int具有循环性,溢出后的值仍然是准确的.
        r_previous = r_current;
        l_previous = l_current;
        if(is_first_value)
        {
        	is_first_value = false;
        	return 0;
        }

        double dr = encoder2meterR(r_diff);
        double dl = encoder2meterR(l_diff);
        ROS_INFO_STREAM("dl&dr is "<<dl<<" : "<<dr);
        double theta = (dl-dr)/Foots_distance;

        ROS_INFO_STREAM("theta is "<< theta );
        if((theta > -0.00001)&&(theta < 0.00001))
		{
            ROS_INFO_STREAM("run in line");
			double vector_move_length = (dl+dr)/2;
			double vector_move_pitch = pitch;
			//move
			x = x + (vector_move_length*cos(vector_move_pitch));
			y = y + (vector_move_length*sin(vector_move_pitch));
			pitch = pitch + theta;

		}
        else
        {
            ROS_INFO_STREAM("run in arc");
			double r =(dl+dr)/(2*theta);
			double vector_move_length = 2*r*sin(theta/2);
			double vector_move_pitch = pitch+(theta/2);
			//move
			x = x + (vector_move_length*cos(vector_move_pitch));
			y = y + (vector_move_length*sin(vector_move_pitch));
			pitch = pitch + theta;
		}
        return 0;
    }
	double Gxzn::encoder2meterR(int32_t encoder)
	{
		return Meter_per_encoder* encoder;
	}

	double Gxzn::get_x()
	{
		return x;
	}
	double Gxzn::get_y()
	{
		return y;
	}
	double Gxzn::get_pitch()
	{
		return pitch;
	}























	//--------------------  function line 3 ------------------------------

	//receive battery voteage from serial
	int Gxzn::readBattery()
	{
		my_serial->flushInput();
		char temp[] = "?V 2\r";
		my_serial->write(temp);
	    string result_tilte,result_value;
	    my_serial->readline(result_tilte,64,"\r");
	    my_serial->readline(result_value,64,"\r");
	    //ROS_INFO_STREAM("sent:\n"<<temp<<"\nreceive:\n"<<result_value<<"\n");


	    int v;
        sscanf(result_value.c_str(),"V=%d\r",&v);
        //ROS_INFO_STREAM("voltage is "<<v);
		battery = v * Voltage_per_num;
	    return 0;
	}
	float Gxzn::get_battery()
	{
		return battery;
	}

}
