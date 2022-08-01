#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <ostream>

#include <yaml-cpp/yaml.h>

class ConfigDTO {
    public:
        ConfigDTO(std::filesystem::path path);

        bool readConfig(std::filesystem::path path);
        bool writeConfig(std::filesystem::path path);

        friend std::ostream& operator <<(std::ostream& outputStream, const ConfigDTO& cfg);

        const std::string chatId();
        const std::string apiKey();
        const std::string logLevel();
        const std::vector<std::string> emergencyKeys();

    private:

        bool _validate(YAML::Node cfg);
};
