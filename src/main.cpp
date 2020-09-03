#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>

#include "ThirdParty/nlohmann/json.hpp"


#include "Config.hpp"
#include "Server.hpp"
#include "Logger.hpp"

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
    std::cout << "WOW" << std::endl;


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

   /*
    _Logger.BasicLog(
            std::string("F4MP  Copyright (C) 2020  Hyunsung Go, Benjamin Kyd\n") +
            std::string("This program comes with ABSOLUTELY NO WARRANTY.\n") +
            std::string("This is free software, and you are welcome to redistribute it\n") +
            std::string("under certain conditions; Read LICENSE for full details.\n\n"));*/

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
       //* _Logger.BasicLog("ERROR: no config exists at", ConfigLocation, ". One has been created");*//*
        exit(0);
    }

    std::ifstream InConfig { ConfigLocation };

    InConfig >> Config::getInstance().JSON;

    Config::getInstance().Setup();
    std::cout << "WOW4" << std::endl;
    // If windows, and configured too, attempt to run as a service
    if (Config::getInstance().RunAsService == true)
    {
#if defined(_WINDOWS) // Use windows service
      /*  _Logger.BasicLog("Starting as windows service");
*/
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
