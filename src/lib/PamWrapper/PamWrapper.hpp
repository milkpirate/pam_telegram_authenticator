# pragma once

#include <string>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

class PamWrapper {
    public:
        explicit PamWrapper(pam_handle_t* pamHandle);

        std::string readInput(const std::string &prompt);
        std::string readPassword(const std::string &prompt);
        void info(const std::string &msg);
        void error(const std::string &error);
        std::string getUser();

    private:
        pam_handle_t* _pamHandle;
        std::string _pamRead(const char* prompt, int pamMsgStyle);
};
