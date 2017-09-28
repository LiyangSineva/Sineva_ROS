//liyang@sineva.com lee8871@126.com
#include "../include/gxzn_robot/comport.hpp"

#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include "string"

using std::string;

namespace comport
{
	Comport::Comport()
	{
		fpath = "";
		fd = -1;
	}
	Comport::~Comport()
	{
		if(fd != -1)
		{
			close(fd);
			fd = -1;
		}
	}
	int Comport::connect(string path)
	{
		if(fd != -1)
		{
			close(fd);
			fd = -1;
		}
		if(path != "")
		{
			fpath = path;
		}
		if(fpath == "")
		{
			return -2;
		}
		fd = open(fpath.c_str(),  O_RDWR | O_NOCTTY | O_NONBLOCK);
		if(fd == -1)
		{
			return -1;
		}
		struct termios opt;
		tcgetattr(fd, &opt);
		opt.c_cflag |= (tcflag_t)  (CLOCAL | CREAD);
		opt.c_lflag &= (tcflag_t) ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN); //|ECHOPRT
		opt.c_oflag &= (tcflag_t) ~(OPOST);
		opt.c_iflag &= (tcflag_t) ~(INLCR | IGNCR | ICRNL | IGNBRK);

		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		opt.c_cflag &=~ CSIZE;   //屏蔽数据位
		opt.c_cflag |=  CS8; 		// 数据位为 8
		opt.c_cflag &=~ CSTOPB;  // 一位停止位， 两位停止为 |= CSTOPB
		opt.c_cflag &=~ PARENB;  // 无校验
		opt.c_cc[VTIME] = 0;  //设置等待时间
		opt.c_cc[VMIN] = 0;     //设置最小字节数
		tcflush(fd,TCIFLUSH);
		if((tcsetattr(fd, TCSANOW, &opt))!=0)
		{
			perror("com set error");
			return -3;
		}
		return 0;

	}

	int Comport::disconnect()
	{
		if(fd != -1)
		{
			close(fd);
			fd = -1;
		}
	}
	int Comport::readOne()
	{
		unsigned char temp;
		if(fd == -1)
		{
			return -2;
		}
		if(read(fd,&temp,1)<=0)
		{
			return -1;
		}
		else
		{
			return (int)temp;
		}
	}
	int Comport::readMut(char *buffer, int size)
	{
		int read_conter = 0;
		while( read_conter<size)
		{
			int temp = readOne();
			if(temp < 0)
			{
				break;
			}
			*buffer = (char)temp;
			buffer++;
			read_conter ++;
		}
		return read_conter;
	}
	int Comport::readLine(char *buffer, int size, char eol)
	{
		int read_conter = 0;
		while(read_conter<(size-1))
		{
			int temp = readOne();
			if(temp < 0)
			{
				*buffer = 0;
				break;
			}
			*buffer = (char)temp;
			buffer++;
			read_conter++;
			if(temp == eol)
			{
				*buffer = 0;
				break;
			}
		}
		return read_conter;
	}
	int Comport::writeMut(const char *buffer,int size)
	{
		if(fd == -1)
		{
			return -2;
		}
		if(-1 == write(fd,buffer,size))
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	int Comport::writeStr(string s)
	{
		const char* temp;
		temp = s.c_str();
		if(fd == -1)
		{
			return -2;
		}
		if(-1 == write(fd, temp, s.length()))
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}

}









