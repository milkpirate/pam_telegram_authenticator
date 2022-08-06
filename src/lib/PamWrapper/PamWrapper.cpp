#include "PamWrapper.hpp"

#include <security/pam_ext.h>

PamWrapper::PamWrapper(pam_handle_t* pamHandle, const std::shared_ptr<spdlog::logger>& log):
    _pamHandle(pamHandle),
    _log(log)
{
    pam_info(_pamHandle, "constr");
}

std::string PamWrapper::readInput(const std::string& prompt) {
    char* str;
    pam_prompt(_pamHandle, PAM_PROMPT_ECHO_ON, &str, "%s", prompt.c_str());
    return std::string(static_cast<const char*>(str));
}

std::string PamWrapper::readPassword(const std::string& prompt) {
    char* str;
    pam_prompt(_pamHandle, PAM_PROMPT_ECHO_OFF, &str, "%s", prompt.c_str());
    return std::string(static_cast<const char*>(str));
}

void PamWrapper::info(const std::string& msg) {
    pam_info(_pamHandle, "%s", msg.c_str());
}

void PamWrapper::error(const std::string& error) {
    pam_error(_pamHandle, "%s", error.c_str());
}

std::string PamWrapper::_getItem(const int& item) {
    const void* str;
    pam_get_item(_pamHandle, item, &str);
    return std::string(static_cast<const char*>(str));
}

std::string PamWrapper::getUser() {
    return _getItem(PAM_USER);
}

std::string PamWrapper::getService() {
    return _getItem(PAM_SERVICE);
}

std::string PamWrapper::getRHost() {
    return _getItem(PAM_RHOST);
}
