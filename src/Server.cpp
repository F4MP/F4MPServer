#include "Server.hpp"

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

#include "Config.hpp"

Server::Server()
{
    peer = SLNet::RakPeerInterface::GetInstance();
    nextSendTime=0;
}

void Server::Start(void)
{
    SLNet::SocketDescriptor socketDescriptor;
    socketDescriptor.port=(unsigned short)Config::getInstance().JSON["port"];
    bool b = peer->Startup((unsigned short) 600,&socketDescriptor,1)== SLNet::RAKNET_STARTED;
    RakAssert(b);
    if(b)
        std::cout << "Server has started sucesfull on port 2001" << std::endl;
    else
        std::cout << "Server has failed to start "<< std::endl;
    peer->SetMaximumIncomingConnections(Config::getInstance().JSON["player-limit"]);
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
