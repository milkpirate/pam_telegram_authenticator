#include "ConfigDTO.hpp"

#include <yaml-cpp/yaml.h>


ConfigDTO::ConfigDTO(std::filesystem::path path) {
    readConfig(path);
}

bool ConfigDTO::readConfig(std::filesystem::path path) {
    auto config = YAML::LoadFile(path);

    if(!_validate(config)) return false;

    chatId = config["chat_id"].as<std::string>();
    apiKey = config["api_key"].as<std::string>();
    logLevel = config["log_level"].as<std::string>();
    emergencyKeys = config["emergency_keys"].as<std::vector<std::string>>();

    return true;
}

bool ConfigDTO::writeConfig(std::filesystem::path path) {
    return true;
}

bool ConfigDTO::_validate(YAML::Node cfg) {
    return true;
}

std::ostream& operator<<(std::ostream &strm, const ConfigDTO& cfg) {
    YAML::Node config;

    config["chat_id"] = cfg.chatId;
    config["api_key"] = cfg.apiKey;
    config["log_level"] = cfg.logLevel;
    config["emergency_keys"] = cfg.emergencyKeys;

    return strm << Dump(config);
}