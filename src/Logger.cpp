#include "Logger.hpp"

Logger::Logger()
{

}

void OutputWorker(Logger* _Logger)
{
	while (_Logger->_IsRunning)
	{
		
	}
}

void Logger::InitializeLoggingThread()
{
	_IsRunning = true;
	_OutputWorker = new std::thread(&OutputWorker, this);
}

Logger::~Logger()
{
	// Signal to the thread that its time to stop

	delete _OutputWorker;
}
