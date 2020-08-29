#include "Logger.hpp"

#include <iomanip>
#include <chrono>
#include <ctime>

Logger::Logger()
{
#if defined(_WINDOWS)
	GetConsoleScreenBufferInfo(h, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes;
#endif
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
		oss << "[" << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << "] ";
		
		while (_Logger->_IsLogging) {}
		_Logger->_IsLogging = true;

		if (entity->Type == ELogType::NONE)
		{
			oss << entity->Message;
			std::cout << oss.str() << std::endl;
			_Logger->_IsLogging = false;
			if (_Logger->_HasFileHandle) _Logger->_FileOutput << oss.str() << std::endl;
			delete entity;
			continue;
		}
		
		oss << "[";
		std::cout << oss.str();

		switch (entity->Type)
		{
		case ELogType::INFO:
			Colour::ConsoleColour(EConsoleColour::FG_GREEN);
			break;
		case ELogType::DEBUG:
			Colour::ConsoleColour(EConsoleColour::FG_BLUE);
			break;
		case ELogType::WARN:
			Colour::ConsoleColour(EConsoleColour::FG_YELLOW);
			break;
		case ELogType::ERR:
			Colour::ConsoleColour(EConsoleColour::FG_LIGHT_RED);
			break;
		case ELogType::PANIC:
			Colour::ConsoleColour(EConsoleColour::FG_RED);
			break;
		}

		std::cout << magic(entity->Type);
		Colour::ResetColour();
		std::cout << "] " << entity->Message << std::endl;

		_Logger->_IsLogging = false;

		if (_Logger->_HasFileHandle) 
			_Logger->_FileOutput << oss.str() << magic(entity->Type) << "] " << entity->Message << std::endl;

		// TODO: This won't exit, i need it to exit the main thread
		//if (entity->Type == ELogType::PANIC) delete entity; exit(0);

		delete entity;
	}
}

void Logger::InitializeFileLogging(std::filesystem::path path)
{
	if (_HasFileHandle) return;
	_FileOutput = std::ofstream();
	_FileOutput.open(static_cast<std::string>(Config::getInstance().LogLocation), std::ios_base::app);
	_HasFileHandle = true;
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

	if (_OutputWorker != nullptr)
		delete _OutputWorker;
}
