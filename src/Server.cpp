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
{
    peer = SLNet::RakPeerInterface::GetInstance();
    nextSendTime=0;
}

void Server::Start(void)
{
    SLNet::SocketDescriptor socketDescriptor((unsigned short)Config::getInstance().JSON["port"],Config::getInstance().JSON["ip"].get<std::string *>()->c_str());

    bool b = peer->Startup((unsigned short) 600,&socketDescriptor,1)== SLNet::RAKNET_STARTED;
    if(b)
        std::cout << "Server has started successfully on port " << Config::getInstance().JSON["port"] << std::endl;
    else
        std::cout << "Server has failed to start "<< std::endl;
    peer->SetMaximumIncomingConnections(Config::getInstance().JSON["player-limit"]);
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
                std::cout << "Connection lost" << std::endl;
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

Server::~Server()
{
    SLNet::RakPeerInterface::DestroyInstance(peer);
}
