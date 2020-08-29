#include "Server.hpp"

#include <iostream>
#include <cstdlib> // For atoi
#include <cstring> // For strlen
#include <cassert>

#include "slikenet/BitStream.h"
#include "slikenet/Rand.h"
#include "slikenet/statistics.h"
#include "slikenet/MessageIdentifiers.h"
#include <cstdio>
#include "slikenet/Kbhit.h"
#include "slikenet/GetTime.h"
#include "slikenet/sleep.h"
#include "slikenet/Gets.h"
#include "slikenet/linux_adapter.h"

#include "Config.hpp"

#define RANDOM_DATA_SIZE_1 50
char randomData1[RANDOM_DATA_SIZE_1];
#define RANDOM_DATA_SIZE_2 100
char randomData2[RANDOM_DATA_SIZE_2];

using namespace SLNet;

Server::Server()
    : _Logger(Logger::getInstance())
{
    peer = SLNet::RakPeerInterface::GetInstance();
    transportInterface = SLNet::TelnetTransport::GetInstance();

    nextSendTime=0;
}

void Server::Start(void)
{
    _Logger.Info("Starting up server");
 
    SLNet::SocketDescriptor socketDescriptor(Config::getInstance().Port, Config::getInstance().Ip.c_str());

    bool b = peer->Startup((unsigned short)600, &socketDescriptor, 1) == SLNet::RAKNET_STARTED;
    if (b)
        _Logger.Info("Server has started successfully on port ", Config::getInstance().Port);
    else
        _Logger.Panic("Server has failed to start");
    peer->SetMaximumIncomingConnections(Config::getInstance().PlayerLimit);

    Server::StartTelnet(transportInterface, 32 ,Server::peer);
}

/// Checks if a ip is banned
/// \param ip
/// \return bool (True if is banned, it can return false if the ip isn't the right format)
bool Server::IsBanned(const char *ip) {
    if(strlen(ip) > 16)
        return false;

    return peer->IsBanned(ip);
}

unsigned Server::ConnectionCount(void) const
{
    unsigned short numberOfSystems;
    peer->GetConnectionList(0,&numberOfSystems);
    return numberOfSystems;
}

void Server::Update(SLNet::TimeMS curTime)
{
    Packet *p = peer->Receive();
    while (p)
    {
        switch (p->data[0])
        {
            case ID_CONNECTION_LOST:
                _Logger.Warn("Connection lost");
            case ID_DISCONNECTION_NOTIFICATION:
            case ID_NEW_INCOMING_CONNECTION:
                _Logger.Debug("Connections = ", ConnectionCount());
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

Server::~Server()
{
    SLNet::RakPeerInterface::DestroyInstance(peer);
}

void Server::StartTelnet(SLNet::TransportInterface *transportInterface, unsigned short port, SLNet::RakPeerInterface *rakPeer)
{

    SLNet::TimeMS lastLog=0;

    _Logger.Info("Command server started on port ", port);
    consoleServer.AddCommandParser(&lcp);

    consoleServer.SetTransportProvider(transportInterface, port);

    consoleServer.SetPrompt("> "); // Show this character when waiting for user input
    lcp.AddChannel("TestChannel");

    for(;;) {
        consoleServer.Update();

        lcp.WriteLog("TestChannel", "Test of logger");
    }

#if defined(_WINDOWS)
    Sleep(30);
#else
    usleep( 30 * 1000 );
#endif
}
