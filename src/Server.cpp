#include "Server.hpp"

#include <iostream>
#include <cstdlib> // For atoi
#include <cstring> // For strlen
#include <cassert>

#include <cstdio>

#include "Config.hpp"

Server::Server()
    : _Logger(Logger::getInstance())
{

}

void Server::Start(void)
{

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

