#include "Logger.hpp"

Logger::Logger()
{

}

void OutputWorker(Logger* _Logger)
{
	while (_Logger->_IsRunning)
	{
		std::unique_lock<std::mutex> lock(_Logger->_QueueLock);
		_Logger->_TaskEnqueued.wait(lock);

		while (!_Logger->_LogQueue.empty())
		{
			_Logger->_QueueLock.lock();
			LogEntity* entity = _Logger->_LogQueue.front();
			_Logger->_LogQueue.pop();
			_Logger->_QueueLock.unlock();

			std::cout << entity->Message << std::endl;
		}

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
