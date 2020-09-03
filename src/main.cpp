#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "ThirdParty/nlohmann/json.hpp"

#include "Config.hpp"
#include "Server.hpp"

#if defined(_WINDOWS)
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif


// Program main, called by actual main or by the daemon / service
int UMain()
{

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
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");

    spdlog::get("console")->info("F4MP  Copyright (C) 2020  Alin Octavian, Benjamin Kyd \n"
                                 "This program comes with ABSOLUTELY NO WARRANTY. \n"
                                 "This is free software, and you are welcome to redistribute it \n"
                                 "\"under certain conditions; Read LICENSE for full details. \n \n");


    std::filesystem::path ConfigLocation { "./config.json" };
    // Parse commandline
    // --config=""
    // etc
    //
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
        spdlog::get("stderr")->error("ERROR: no config exists. One has been created");
        exit(0);
    }

    std::ifstream InConfig { ConfigLocation };

    InConfig >> Config::getInstance().JSON;

    Config::getInstance().Setup();
    // If windows, and configured too, attempt to run as a service
    if (Config::getInstance().RunAsService == true)
    {
#if defined(_WINDOWS) // Use windows service
        spdlog::get("console")->info("Starting a Windows service");

        SERVICE_TABLE_ENTRY ServiceTable[] = {
            { reinterpret_cast<LPSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)serviceMain },
            { nullptr, nullptr }
        };

       /* if (StartServiceCtrlDispatcher(ServiceTable) == false)
            _Logger.BasicLog("ERROR: Failed to start as a service, resuming");
*/
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
