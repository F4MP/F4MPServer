#include "Logger.hpp"

Logger::Logger()
{

}

void LoggingThread(Logger* _Logger)
{
	while (_Logger->_IsRunning)
	{

	}
}

void Logger::InitializeLoggingThread()
{
	_IsRunning = true;
	_LoggingThread = new std::thread(&LoggingThread, this);
}

Logger::~Logger()
{
	// Signal to the thread that its time to stop

	delete _LoggingThread;
}
