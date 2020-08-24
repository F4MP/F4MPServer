#ifndef F4MPSERVER_LOGGER_H_
#define F4MPSERVER_LOGGER_H_

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>


#ifdef _WINDOWS
#include <Windows.h>
namespace EConsoleColour
{
	enum Colour
	{
		FG_BLACK = 0,
		FG_BLUE = FOREGROUND_BLUE,
		FG_GREEN = FOREGROUND_GREEN,
		FG_CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		FG_RED = FOREGROUND_RED,
		FG_MAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
		FG_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		FG_GRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FG_LIGHT_GRAY = FOREGROUND_INTENSITY,
		FG_LIGHT_BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		FG_LIGHT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		FG_LIGHT_CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FG_LIGHT_RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
		FG_LIGHT_MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		FG_LIGHT_YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		FG_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		FG_DEFAULT = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		BG_DEFAULT = 0
	};
}

static HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
static WORD wOldColorAttrs;
static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

#else
namespace EConsoleColour
{
	enum Colour
	{
		FG_DEFAULT = 39,
		FG_BLACK = 30,
		FG_RED = 31,
		FG_GREEN = 32,
		FG_YELLOW = 33,
		FG_BLUE = 34,
		FG_MAGENTA = 35,
		FG_CYAN = 36,
		FG_LIGHT_GRAY = 37,
		FG_DARK_GRAY = 90,
		FG_LIGHT_RED = 91,
		FG_LIGHT_GREEN = 92,
		FG_LIGHT_YELLOW = 93,
		FG_LIGHT_BLUE = 94,
		FG_LIGHT_MAGENTA = 95,
		FG_LIGHT_CYAN = 96,
		FG_WHITE = 97,
		BG_RED = 41,
		BG_GREEN = 42,
		BG_BLUE = 44,
		BG_DEFAULT = 49
	};
}
#endif


class LogManager
{

};

namespace ELogType
{
	enum Type : uint8_t
	{
		NONE,
		INFO,
		DEBUG,
		WARN,
		ERR,
		PANIC
	};
}

struct LogEntity
{
	std::string Message;
	ELogType::Type Type;
};

class Logger
{
public:

	Logger();

	static Logger& getInstance()
	{
		static Logger instance;
		return instance;
	}

	void InitializeLoggingThread();

	template<typename T>
	void LogElement(T t)
	{
		if constexpr (std::is_same<char, T>::value)
		{
			char p = static_cast<char>(t);
			if (p == '\n') std::cout << p; return;
		}
		std::cout << t << " ";
	}

	// Basic log doesn't use the logthread so
	// can be used before the thread is setup
	// unless it is already ready
	template<typename... Args>
	void BasicLog(Args... args)
	{
		if (_IsRunning)
		{
			Log(args...);
			return;
		}
		(LogElement(args), ...);
		LogElement('\n');
	}

	template<typename... Args>
	void Log(Args... args)
	{
		if (!_IsRunning) return;
		std::stringstream s;
		_FillStringStream(s, args...);
		LogEntity* e = new LogEntity{ s.str(), ELogType::NONE };

		_QueueLock.lock();
		_LogQueue.push(e);
		_QueueLock.unlock();

		_TaskEnqueued.notify_all();
	}

	template<typename... Args>
	void Log(ELogType::Type type, Args... args)
	{
		if (!_IsRunning) return;
		std::stringstream s;
		_FillStringStream(s, args...);
		LogEntity* e = new LogEntity{ s.str(), type };

		_QueueLock.lock();
		_LogQueue.push(e);
		_QueueLock.unlock();

		_TaskEnqueued.notify_all();
	}

	template<typename... Args>
	void Info(Args... args)
	{
		if (!_IsRunning) return;
		Log(ELogType::INFO, args...);
	}

	template<typename... Args>
	void Debug(Args... args)
	{
		if (!_IsRunning) return;
		Log(ELogType::DEBUG, args...);
	}

	template<typename... Args>
	void Warn(Args... args)
	{
		if (!_IsRunning) return;
		Log(ELogType::WARN, args...);
	}

	template<typename... Args>
	void Error(Args... args)
	{
		if (!_IsRunning) return;
		Log(ELogType::ERR, args...);
	}

	template<typename... Args>
	void Panic(Args... args)
	{
		if (!_IsRunning) return;
		Log(ELogType::PANIC, args...);
	}

	~Logger();

public:

	std::condition_variable _TaskEnqueued;
	std::mutex _TaskConditionLock;
	std::queue<LogEntity*> _LogQueue;
	std::mutex _QueueLock;
	std::atomic<bool> _IsRunning = false;

protected:

	std::string _GetLogTimeString();
	std::string _LogInfoString();
	std::string _LogDebugString();
	std::string _LogWarnString();
	std::string _LogErrorString();
	std::string _LogPanictring();

private:

	template <typename T, typename... A>
	void _FillStringStream(std::stringstream& s, T head, A... a)
	{
		s << head << ' ';
		if constexpr (sizeof...(a))
		{
			_FillStringStream(s, a...);
		}
	}

	std::thread* _OutputWorker;

};

class ScopedLogger
{

};

// times how long the scope took etc
class ProfileLogger : public ScopedLogger
{

};

#endif
