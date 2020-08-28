#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>

#include "ThirdParty/nlohmann/json.hpp"

#include "slikenet/GetTime.h"
#include "slikenet/sleep.h"
#include "slikenet/Gets.h"
#include "slikenet/linux_adapter.h"
#include "slikenet/statistics.h"

#include "Config.hpp"
#include "Server.hpp"
#include "Logger.hpp"

#if defined(_WINDOWS)
#include "slikenet/WindowsIncludes.h" //It needs to be before the windows.h file
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

using namespace SLNet;

// Program main, called by actual main or by the daemon / service
int UMain()
{
    Logger& _Logger = Logger::getInstance();
    _Logger.InitializeLoggingThread();
    _Logger.Info("Initializing...");

    Server server;

    SLNet::TimeMS time = SLNet::GetTimeMS();
    SLNet::TimeMS endTime = SLNet::GetTimeMS()+60000*5;

    server.Start();

    for(;;) {
        server.Update(time);
        int input;

        std::cin >> input;
        if(input == 0) {
            _Logger.Info("Logging server statistics");
            char text[2048];

            for (unsigned int i = 0; i < server.ConnectionCount(); i++) {
                RakNetStatistics *rssSender;
                rssSender = server.peer->GetStatistics(server.peer->GetSystemAddressFromIndex(i));
                StatisticsToString(rssSender, text, 2048, 3);
                _Logger.Log(text);
            }
        }
    }

    return 0;
}

#if defined(_WINDOWS)
#define SERVICE_NAME L"F4MPService"

static DWORD WINAPI serviceWorkerThread(LPVOID lpParam)
{
    UMain();
    return 0;
}

static void WINAPI serviceMain(DWORD argc, TCHAR** argv)
{
    // service handle shit, set state and so on
    // TODO: This
}

#endif

int main(int argc, char** argv)
{
    // Welcome message
    Logger& _Logger = Logger::getInstance();
    _Logger.BasicLog(
            std::string("F4MP  Copyright (C) 2020  Hyunsung Go, Benjamin Kyd\n") +
            std::string("This program comes with ABSOLUTELY NO WARRANTY.\n") +
            std::string("This is free software, and you are welcome to redistribute it\n") +
            std::string("under certain conditions; Read LICENSE for full details.\n\n"));

    _Logger.BasicLog("F4MP Build No.", GIT_VERSION, "Built", BUILD_TIME);
    std::stringstream version;
    version << "Release Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH;
    _Logger.BasicLog(version.str(), "\n\n");

    std::filesystem::path ConfigLocation { "./config.json" };
    // Parse commandline
    // --config=""
    // etc

    // Load config
    if (!std::filesystem::exists(ConfigLocation))
    {
        nlohmann::json Config;
        Config["ip"] = "127.0.0.1";
        Config["port"] = 7779;
        Config["run-as-service"] = false;
        Config["player-limit"] = 100;
        Config["log-location"] = "./logs.log"; // or NONE
        std::ofstream o { ConfigLocation };
        o << std::setw(4) << Config << std::endl;
        _Logger.BasicLog("ERROR: no config exists at", ConfigLocation, ". One has been created");
        exit(0);
    }

    Config& config = Config::getInstance();
    std::ifstream InConfig { ConfigLocation };
    InConfig >> config.JSON;

    std::string logloc = config.JSON["log-location"];
    if (logloc != "NONE")
        _Logger.InitializeFileLogging({ logloc });

    // If windows, and configured too, attempt to run as a service
    if (Config::getInstance().JSON["run-as-service"] == true)
    {
#if defined(_WINDOWS) // Use windows service
        _Logger.BasicLog("Starting as windows service");

        SERVICE_TABLE_ENTRY ServiceTable[] = {
            { reinterpret_cast<LPSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)serviceMain },
            { nullptr, nullptr }
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == false)
            _Logger.BasicLog("ERROR: Failed to start as a service, resuming");

#else // UNIX daemon
        _Logger.BasicLog("Starting as UNIX daemon");

        pid_t Pid fork();
        if (pid < 0)
            _Logger.BasicLog("ERROR: Failed to start fork process, resuming");

        // if this process is the parent, cease exicution
        if (pid > 0)
        {
            _Logger.BasicLog("Parent process terminating");
            exit(0);
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
		close(STDERR_FILENO);

		UMain();

#endif
    }


    return UMain();
}
