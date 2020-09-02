#include "Server.hpp"

#include <iostream>
#include <cstdlib> // For atoi
#include <cstring> // For strlen
#include <cassert>

#include <cstdio>


#include "Config.hpp"

#define RANDOM_DATA_SIZE_1 50
char randomData1[RANDOM_DATA_SIZE_1];
#define RANDOM_DATA_SIZE_2 100
char randomData2[RANDOM_DATA_SIZE_2];

Server::Server()
    : _Logger(Logger::getInstance())
{

}

void Server::Start(void)
{
    _Logger.Info("Starting up server");
 

//    bool b = peer->Startup((unsigned short)600, &socketDescriptor, 1) == SLNet::RAKNET_STARTED;
 //   if (b)
   //     _Logger.Info("Server has started successfully on port ", Config::getInstance().Port);
   // else
    //    _Logger.Panic("Server has failed to start");

}

/// Checks if a ip is banned
/// \param ip
/// \return bool (True if is banned, it can return false if the ip isn't the right format)
bool Server::IsBanned(const char *ip) {


    return false;
}

unsigned Server::ConnectionCount(void) const
{

    return 0;
}

void Server::Update(uint64_t time)
{

}

Server::~Server()
{
   //Destructor
}

