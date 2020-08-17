#ifndef F4MPSERVER_SERVER_H_
#define F4MPSERVER_SERVER_H_

#include "slikenet/peerinterface.h"
#include "slikenet/TelnetTransport.h"
#include "slikenet/ConsoleServer.h"
#include "slikenet/commandparser.h"
#include "slikenet/LogCommandParser.h"
#include "slikenet/transport2.h"

#ifdef _WINDOWS
#include "slikenet/WindowsIncludes.h" //It needs to be before the windows.h file
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#endif

using namespace SLNet;

class Server
{
public:
    Server();

    void Start(void);

    void StartTelnet(SLNet::TransportInterface *transportInterface, unsigned short port, SLNet::RakPeerInterface *rakPeer);

    bool IsBanned(const char* ip);

    unsigned ConnectionCount(void) const;

    void Update(SLNet::TimeMS curTime);



    ~Server();

public:
    SLNet::ConsoleServer consoleServer;
    SLNet::LogCommandParser lcp;
    SLNet::TransportInterface *transportInterface;
    SLNet::TimeMS nextSendTime;
    RakPeerInterface *peer;
};

#endif
