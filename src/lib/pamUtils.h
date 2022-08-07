#pragma once

#include <string>

#include <security/pam_appl.h>
#include <security/pam_ext.h>

std::string pamReadInput(pam_handle_t* pamh, const std::string& prompt) {
    char* str;
    pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &str, "%s", prompt.c_str());
    return std::string(static_cast<const char*>(str));
}

std::string pamReadPassword(pam_handle_t* pamh, const std::string& prompt) {
    char* str;
    pam_prompt(pamh, PAM_PROMPT_ECHO_OFF, &str, "%s", prompt.c_str());
    return std::string(static_cast<const char*>(str));
}

void pamInfo(pam_handle_t* pamh, const std::string& msg) {
    pam_info(pamh, "%s", msg.c_str());
}

void pamError(pam_handle_t* pamh, const std::string& error) {
    pam_error(pamh, "%s", error.c_str());
}

std::string _pamGetItem(pam_handle_t* pamh, const int& item) {
    const void* str;
    pam_get_item(pamh, item, &str);
    return std::string(static_cast<const char*>(str));
}

std::string pamGetUser(pam_handle_t* pamh) {
    return _pamGetItem(pamh, PAM_USER);
}

std::string pamGetService(pam_handle_t* pamh) {
    return _pamGetItem(pamh, PAM_SERVICE);
}

std::string pamGetRHost(pam_handle_t* pamh) {
    return _pamGetItem(pamh, PAM_RHOST);
}

