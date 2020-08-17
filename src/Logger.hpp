#ifndef F4MPSERVER_LOGGER_H_
#define F4MPSERVER_LOGGER_H_

#include <thread>

class Logger
{
public:

	Logger();

	void InitializeThreads();



private:

	std::thread _LoggingThread;

};


#endif
