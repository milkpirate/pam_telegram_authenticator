# pragma once

#include <string>

#include <security/pam_modules.h>
#include <spdlog/logger.h>

class PamWrapper {
    public:
        PamWrapper(pam_handle_t* pamHandle, const std::shared_ptr<spdlog::logger>& log);

        std::string readInput(const std::string &prompt);
        std::string readPassword(const std::string &prompt);
        void info(const std::string &msg);
        void error(const std::string &error);
        std::string getService();
        std::string getRHost();
        std::string getUser();

    private:
        std::string _pamRead(const char* prompt, int msgStyle);
        std::string _getItem(const int& item);
        pam_handle_t* _pamHandle;
        std::shared_ptr<spdlog::logger> _log;
};
