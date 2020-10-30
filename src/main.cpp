#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cassert>
#include <olcnet/olc_net.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "ThirdParty/nlohmann/json.hpp"
#include "Config.hpp"
#if defined(_WINDOWS)
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif



enum class CustomMsgTypes : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
};

const std::string ConfigPath = "./config.json";

class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
    CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
    {

    }

protected:
    virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
    {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerAccept;
        client->Send(msg);
        return true;
    }

    // Called when a client appears to have disconnected
    virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
    {
        std::cout << "Removing client [" << client->GetID() << "]\n";
    }

    // Called when a message arrives
    virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
    {
        switch (msg.header.id)
        {
            case CustomMsgTypes::ServerPing:
            {
                std::cout << "[" << client->GetID() << "]: Server Ping\n";

                // Simply bounce message back to client
                client->Send(msg);
            }
                break;

            case CustomMsgTypes::MessageAll:
            {
                std::cout << "[" << client->GetID() << "]: Message All\n";

                // Construct a new message and send it to all clients
                olc::net::message<CustomMsgTypes> msg;
                msg.header.id = CustomMsgTypes::ServerMessage;
                msg << client->GetID();
                MessageAllClients(msg, client);

            }
                break;
        }
    }
};

void SetupConfig(const std::string& path) {

    std::filesystem::path ConfigLocation {path};
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

}

// Program main, called by actual main or by the daemon / service
int UMain()
{
    CustomServer server(Config::getInstance().Port);
    server.Start();

    while (1)
    {
        server.Update(-1, true);
    }
    return 0;
}

#if defined(_WINDOWS)
#define SERVICE_NAME L"F4MPService"

static DWORD WINAPI serviceWorkerThread(LPVOID lpParam)
{
    SetupConfig(ConfigPath);
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

    std::cout << "F4MP  Copyright (C) 2020 F4MP Project\n"
                 "This program comes with ABSOLUTELY NO WARRANTY. \n"
                 "This is free software, and you are welcome to redistribute it \n"
                 "under certain conditions; Read LICENSE for full details.\n";

    
    SetupConfig("./config.json");

    // If windows, and configured too, attempt to run as a service
    if (Config::getInstance().RunAsService)
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

    auto port = Config::getInstance().Port;

    return UMain();
}
