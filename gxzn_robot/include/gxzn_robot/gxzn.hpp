
#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "serial/serial.h"


using std::string;
using namespace serial;
namespace gxzn
{
	class Gxzn
	{
	private:
		Serial * my_serial;
		string port;
		//following 2 variable set by users
		int speed_set_l;
		int speed_set_r;

		bool is_first_value;
		int32_t l_previous ;
		int32_t r_previous ;
	private:
		//the following  variable is motor moving value, which is calculated by mileage
		double velocity ;
		double angulat_velocity ;
		double x ;
		double y ;
		double pitch ;

		float battery;

		//Parameter for robot
		const static double Foots_distance = 0.370;//两个车轮(履带)之间的距离(米)
		const static double Unit_per_meter = 122.5;//没有用到
		//========================================编码器    减速器   等效直径(米)    π
		const static double Meter_per_encoder = (1.0/1000)*(1.0/10.0)*((91.8/1000)*3.1416);//编码器每单位对应的前进距离(米)
		const static float Voltage_per_num = 0.1;//int值和电压之间的关系

	public:
		double get_velocity();
		double get_angulat_velocity();
		double get_x();
		double get_y();
		double get_pitch();
		float get_battery();

		//functions set value to motor by serial port
	public:
		int writeMotorSpeed();
		int readMoveDistance();
		int readBattery();
	private:
		//the in meter per second should be transform to -1000~1000 for send to motor.
		int mps2int(double mps);
		double encoder2meterR(int32_t encoder);
		int32_t Encoder2Moving(int32_t diff,int32_t last);

	public:
		//base method
		Gxzn(string uart_path);
		~Gxzn();
		//This method is use to check weather the AGV connected well
		int isConnected();
	public:
		//set motor speed
		int setSpeedRL(double l,double r);
		int setSpeedVW(double v,double w);
	};
};

