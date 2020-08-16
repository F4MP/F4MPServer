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

#define RANDOM_DATA_SIZE_1 50
char randomData1[RANDOM_DATA_SIZE_1];
#define RANDOM_DATA_SIZE_2 100
char randomData2[RANDOM_DATA_SIZE_2];

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
