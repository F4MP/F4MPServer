#ifndef F4MPSERVER_SERVER_H_
#define F4MPSERVER_SERVER_H_

#include "slikenet/peerinterface.h"

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

    unsigned ConnectionCount(void) const;

    void Update(SLNet::TimeMS curTime);

    ~Server();

public:

    SLNet::TimeMS nextSendTime;
    RakPeerInterface *peer;
};

#endif
