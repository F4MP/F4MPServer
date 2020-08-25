#include "Logger.hpp"

#include <iomanip>
#include <chrono>
#include <ctime>

Logger::Logger()
{

}

constexpr const char* magic(ELogType::Type e)
{
	switch (e)
	{
		default: return ""; break;
		case ELogType::Type::NONE: return ""; break;
		case ELogType::Type::INFO: return "INFO"; break;
		case ELogType::Type::DEBUG: return "DEBUG"; break;
		case ELogType::Type::WARN: return "WARN"; break;
		case ELogType::Type::ERR: return "ERROR"; break;
		case ELogType::Type::PANIC: return "PANIC"; break;
	}
}

void OutputWorker(Logger* _Logger)
{
	while (_Logger->_IsRunning)
	{
		std::unique_lock<std::mutex> lock(_Logger->_QueueLock);
		_Logger->_TaskEnqueued.wait(lock, [&] { return !_Logger->_LogQueue.empty(); });

		LogEntity* entity = _Logger->_LogQueue.front();
		_Logger->_LogQueue.pop();
		lock.unlock();

		// Get C Time 
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		std::ostringstream oss;
		oss << "[" << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << "]";
		


		std::cout << oss.str() << std::endl;

		delete entity;
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

	_IsRunning = false;
	_OutputWorker->join();

	delete _OutputWorker;
}
