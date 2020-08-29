#ifndef F4MPSERVER_CONFIG_H_
#define F4MPSERVER_CONFIG_H_

#include "./ThirdParty/nlohmann/json.hpp"

#include "Logger.hpp"

class Config
{
public:
    Config() {}

    static Config& getInstance()
    {
        static Config instance;
        return instance;
    }

    inline void Setup()
    {
        Ip = JSON["ip"];
        Port = static_cast<uint16_t>(JSON["port"]);
        PlayerLimit = static_cast<uint16_t>(JSON["player-limit"]);
        RunAsService = static_cast<bool>(JSON["run-as-service"]);
        LogLocation = JSON["log-location"];
    }

    inline void Validate()
    {
        // TODO: This
    }

    std::string Ip;
    uint16_t Port;
    uint16_t PlayerLimit;

    bool RunAsService;

    std::string LogLocation;

    nlohmann::json JSON;

public:

    // Omit copy constructor(s)
    Config(Config const&) = delete;
    void operator=(Config const&) = delete;
};

#endif
