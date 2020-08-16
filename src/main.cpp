#include <iostream>
#include <filesystem>
#include <fstream>

#include "ThirdParty/nlohmann/json.hpp"

#ifdef _WINDOWS
#include <windows.h>
#include <libloaderapi.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

// TODO: We need a logging library, ill make one just it aint instant, which NEEDS to include file logging due to services/daemon

// Program main, called by actual main or by the daemon / service
int UMain()
{
    std::cout << "Server starting up" << std::endl;

    return 0;
}

// Should be able to run as a service on windows
#ifdef _WINDOWS

#define SERVICE_NAME L"F4MPService"

static DWORD WINAPI serviceWorkerThread(LPVOID lpParam)
{
    UMain();
    return 0;
}

static void WINAPI serviceMain(DWORD argc, TCHAR** argv)
{
    // service handle shit, set state and so on
}

#endif

int main(int argc, char** argv)
{
    // Welcome message
    std::cout
         << "F4MP  Copyright (C) 2020  Hyunsung Go, Benjamin Kyd\n"
         << "This program comes with ABSOLUTELY NO WARRANTY.\n"
         << "This is free software, and you are welcome to redistribute it\n"
         << "under certain conditions; Read LICENSE for full details.\n" << std::endl << std::endl;

    std::filesystem::path ConfigLocation { "./config.json" };
    // Parse commandline
    // --config=""
    // etc

    // Load config

    if (!std::filesystem::exists(ConfigLocation))
    {
        nlohmann::json Config;
        Config["ip"] = "127.0.0.1";
        Config["port"] = "7779";
        Config["run-as-service"] = false;
        std::ofstream o { ConfigLocation };
        o << std::setw(4) << Config << std::endl;
        std::cout << "ERROR: no config exists at " << ConfigLocation << ". One has been created" << std::endl;
        exit(0);
    }

    nlohmann::json Config;
    std::ifstream InConfig { ConfigLocation };
    InConfig >> Config;

    // If windows, and configured too, attempt to run as a service
    // TODO: Something similar with linux daemons
    if (Config["run-as-service"] == true)
    {
#ifdef _WINDOWS // Use windows service
        std::cout << "Starting as windows service" << std::endl;
        SERVICE_TABLE_ENTRY ServiceTable[] = {
            { reinterpret_cast<LPSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)serviceMain },
            { nullptr, nullptr }
        };
        if (StartServiceCtrlDispatcher(ServiceTable) == false)
        {
            std::cout << "ERROR: Failed to start as a service, resuming" << std::endl;
        }
#else // UNIX daemon
        std::cout << "Starting as UNIX daemon" << std::endl;

        pid_t Pid fork();
        if (pid < 0)
        {
            std::cout << "ERROR: Failed to start fork process, resuming" << std::endl;
        }

        // if this process is the parent, cease exicution
        if (pid > 0)
        {
            std::cout << "Parent process terminating" << std::endl;
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
