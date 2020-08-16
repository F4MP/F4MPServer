#ifndef F4MPSERVER_CONFIG_H_
#define F4MPSERVER_CONFIG_H_

#include "./ThirdParty/nlohmann/json.hpp"

class Config
{
public:
    Config() {}

    static Config& getInstance()
    {
        static Config instance;
        return instance;
    }

    nlohmann::json JSON;

    // Omit copy constructor(s)
    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;
};

#endif
