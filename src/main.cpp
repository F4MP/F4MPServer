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

#ifdef _WINDOWS
#include "slikenet/WindowsIncludes.h" //It needs to be before the windows.h file
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

// TODO: We need a logging library, ill make one just it aint instant,
// which NEEDS to include file logging due to services/daemon

using namespace SLNet;

// Program main, called by actual main or by the daemon / service
int UMain()
{
    std::cout << "Initializing..." << std::endl;

    Server server;

    SLNet::TimeMS time = SLNet::GetTimeMS();
    SLNet::TimeMS endTime = SLNet::GetTimeMS()+60000*5;

    server.Start();

    for(;;) {
        server.Update(time);
        int input;

        std::cin >> input;
        if(input == 0) {
            printf("Logging server statistics\n");
            char text[2048];

            for (unsigned int i = 0; i < server.ConnectionCount(); i++) {
                RakNetStatistics *rssSender;
                rssSender = server.peer->GetStatistics(server.peer->GetSystemAddressFromIndex(i));
                StatisticsToString(rssSender, text, 2048, 3);
                std::cout << text << std::endl;
            }
        }
    }

    return 0;
}

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
    // TODO: This
}

#endif

int main(int argc, char** argv)
{
    // Welcome message
    std::cout
         << "F4MP  Copyright (C) 2020  Hyunsung Go, Benjamin Kyd\n"
         << "This program comes with ABSOLUTELY NO WARRANTY.\n"
         << "This is free software, and you are welcome to redistribute it\n"
         << "under certain conditions; Read LICENSE for full details." << std::endl << std::endl;

    std::cout << "F4MP Build No." << GIT_VERSION << " Built " << BUILD_TIME << std::endl;
    std::stringstream version;
    version << "Release Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH;
    std::cout << version.str() << std::endl << std::endl << std::endl;

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
        Config["log-location"] = "./";
        std::ofstream o { ConfigLocation };
        o << std::setw(4) << Config << std::endl;
        std::cout << "ERROR: no config exists at " << ConfigLocation << ". One has been created" << std::endl;
        exit(0);
    }

    Config& config = Config::getInstance();
    std::ifstream InConfig { ConfigLocation };
    InConfig >> config.JSON;

    // If windows, and configured too, attempt to run as a service
    if (Config::getInstance().JSON["run-as-service"] == true)
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
