#ifndef F4MPSERVER_SERVER_H_
#define F4MPSERVER_SERVER_H_

#ifdef _WINDOWS
#include <windows.h>
#include <libloaderapi.h> // Sleep64
#else
#include <arpa/inet.h>
#endif

#include "./Logger.hpp"

using namespace SLNet;

class Server
{
public:
    Server();

    void Start(void);

    unsigned ConnectionCount(void) const;

    void Update(uint64_t time);

    ~Server();

public:


private:
    Logger& _Logger;

    bool IsBanned(const char *ip);
};

#endif
