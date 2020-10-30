#ifndef F4MPSERVER_CONFIG_H_
#define F4MPSERVER_CONFIG_H_

#include "./ThirdParty/nlohmann/json.hpp"


// READ ME : If you don't have a JSON Row the library tends to crash
class Config
{
public:
    Config() {}

    // Omit copy constructor(s)
    Config(Config const&) = default;
    void operator=(Config const&) = delete;

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

    inline bool Validate()
    {
        return true;
    }


    inline bool Exists(){
            return std::filesystem::exists(ConfigLocation);
    }

    std::filesystem::path ConfigLocation { "./config.json" };


    std::string Ip;
    uint16_t Port;
    uint16_t PlayerLimit;

    bool RunAsService;

    std::string LogLocation;

    nlohmann::json JSON;

};

#endif
