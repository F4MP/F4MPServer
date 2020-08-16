#include <iostream>
#include <filesystem>
#include <fstream>

#include "slikenet/peerinterface.h"

#include "slikenet/BitStream.h"
#include <cstdlib> // For atoi
#include <cstring> // For strlen
#include "slikenet/Rand.h"
#include "slikenet/statistics.h"
#include "slikenet/MessageIdentifiers.h"
#include <cstdio>
#include "slikenet/Kbhit.h"
#include "slikenet/GetTime.h"
#include <cassert>
#include "slikenet/sleep.h"
#include "slikenet/Gets.h"
#include "slikenet/linux_adapter.h"

#include "ThirdParty/nlohmann/json.hpp"

#ifdef _WINDOWS
#include "slikenet/WindowsIncludes.h" //It needs to be before the windows.h file
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

// TODO: We need a logging library, ill make one just it aint instant, which NEEDS to include file logging due to services/daemon

using namespace SLNet;

#define SERVER_PORT 2001
static const int NUM_CLIENTS=100;

#define RANDOM_DATA_SIZE_1 50
char randomData1[RANDOM_DATA_SIZE_1];
#define RANDOM_DATA_SIZE_2 100
char randomData2[RANDOM_DATA_SIZE_2];

class Server
{
public:
    Server()
    {
        peer = SLNet::RakPeerInterface::GetInstance();
        nextSendTime=0;
    }
    ~Server()
    {
        SLNet::RakPeerInterface::DestroyInstance(peer);
    }
    void Start(void)
    {
        SLNet::SocketDescriptor socketDescriptor;
        socketDescriptor.port=(unsigned short) SERVER_PORT;
        bool b = peer->Startup((unsigned short) 600,&socketDescriptor,1)== SLNet::RAKNET_STARTED;
        RakAssert(b);
        if(b)
            std::cout << "Server has started sucesfull on port 2001" << std::endl;
        else
            std::cout << "Server has failed to start "<< std::endl;
        peer->SetMaximumIncomingConnections(600);
    }
    unsigned ConnectionCount(void) const
    {
        unsigned short numberOfSystems;
        peer->GetConnectionList(0,&numberOfSystems);
        return numberOfSystems;
    }

    void Update(SLNet::TimeMS curTime)
    {
        Packet *p = peer->Receive();
        while (p)
        {
            switch (p->data[0])
            {
                case ID_CONNECTION_LOST:
                case ID_DISCONNECTION_NOTIFICATION:
                case ID_NEW_INCOMING_CONNECTION:
                    printf("Connections = %i\n", ConnectionCount());
                    break;
// 				case ID_USER_PACKET_ENUM:
// 					{
// 						peer->Send((const char*) p->data, p->length, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->guid, true);
// 						break;
// 					}
            }
            peer->DeallocatePacket(p);
            p = peer->Receive();
        }

        if (curTime>nextSendTime)
        {
            if (randomMT()%10==0)
            {
                peer->Send((const char*)&randomData2, RANDOM_DATA_SIZE_2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true);
            }
            else
            {
                peer->Send((const char*)&randomData1,RANDOM_DATA_SIZE_1,HIGH_PRIORITY,RELIABLE_ORDERED,0, SLNet::UNASSIGNED_SYSTEM_ADDRESS,true);
            }

            nextSendTime=curTime+100;
        }
    }


    SLNet::TimeMS nextSendTime;
    RakPeerInterface *peer;
};

// Program main, called by actual main or by the daemon / service
int UMain()
{
    std::cout << "Initializing..." << std::endl;

    Server server;

    SLNet::TimeMS time = SLNet::GetTimeMS();
    SLNet::TimeMS endTime = SLNet::GetTimeMS()+60000*5;

    server.Start();

    while(true) {
        server.Update(time);

        int input;

        std::cin >> input;
        if(input == 0) {
            printf("Logging server statistics\n");
            char text[2048];

            //TODO : You need to get the NUM_CLIENTS from the config file
            for (int i = 0; i < NUM_CLIENTS; i++) {
                RakNetStatistics *rssSender;
                rssSender = server.peer->GetStatistics(server.peer->GetSystemAddressFromIndex(i));
                StatisticsToString(rssSender, text, 2048, 3);
                std::cout << text << std::endl;
            }
        }
    }

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
        Config["port"] = 7779;
        Config["run-as-service"] = false;
        Config["player-limit"] = 100;
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
