//liyang@sineva.com lee8871@126.com
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
	class Comport
	{
	private:
		int fd;
		string fpath;
	public:
		Comport();
		~Comport();
		int connect(string path = "");
		int disconnect();
		int readOne();
		int readMut(char *buffer, int size);
		int readLine(char *buffer, int size, char eol);
		int writeOne(char buffer);
		int writeMut(const char *buffer,int size);
		int writeStr(string s);
	};

}
