#include <iostream>
#include <ctime>

class Log
{
	public :
		Log()
		{
			tm* current = getTime();
			printLog(current, "No logs");
		}

		Log(std::string msg)
		{
			tm* current = getTime();
			printLog(current, msg);
		}

		tm* getTime(void)
		{
			time_t t = time(&t); //time() gets the current time in seconds since epoch.
			tm* now = localtime(&t);//localtime() breaks down result of time() into month, day, hour, min, sec and stores it in the tm struct
			return now;
		}

		void printLog(struct tm* now, const std::string &msg) const
		{
			std::cout <<now->tm_mday << '-' << (now->tm_mon + 1) << '-'
			<< (now->tm_year + 1900) << " " << now->tm_hour << ":" << now->tm_min << ":"
			<< now->tm_sec << " " << msg << std::endl;
		}
};

int main(void)
{
    Log log1("Server Started");
	Log log2("Client1 Connected");
	Log log3("Client2 Connected");
	Log log4("Client1 Disconnected");
	Log log5("Client2 Restarted");
	Log log6;
    return 0;
}