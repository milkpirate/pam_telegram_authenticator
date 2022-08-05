#include "PamWrapper.hpp"

PamWrapper::PamWrapper(pam_handle_t *pamHandle):
    _pamHandle(pamHandle)
{}

std::string PamWrapper::_pamRead(const char* prompt, int pamMsgStyle) {
    struct pam_response *pamResp = nullptr;
    struct pam_conv *conv = nullptr;

    // I have no idea why that have to be like that
    struct pam_message msg[1];
    msg[0].msg_style = pamMsgStyle;
    msg[0].msg = prompt;

    const pam_message *pmsg[1];
    pmsg[0] = &msg[0];

    auto pamReturnCode = pam_get_item(_pamHandle, PAM_CONV, (const void**) &conv);

    // all hacky but we hide it here...
    if(pamMsgStyle >= PAM_ERROR_MSG) {
        return "";
    }

    if(pamReturnCode == PAM_SUCCESS) {
        conv->conv(1, (const struct pam_message**)pmsg, &pamResp, conv->appdata_ptr);
    }

    if(!pamResp) {
        // PAM_CONV_ERR
        return "";
    }

    auto userInput = std::string(pamResp[0].resp);
    pamResp[0].resp = nullptr;    // null out after read?

    return userInput;
}


std::string PamWrapper::readInput(const std::string& prompt) {
    return _pamRead(prompt.c_str(), PAM_PROMPT_ECHO_ON);
}

std::string PamWrapper::readPassword(const std::string& prompt) {
    return _pamRead(prompt.c_str(), PAM_PROMPT_ECHO_OFF);
}

void PamWrapper::info(const std::string& msg) {
    _pamRead(msg.c_str(), PAM_TEXT_INFO);
}

void PamWrapper::error(const std::string& error) {
    _pamRead(error.c_str(), PAM_ERROR_MSG);
}

std::string PamWrapper::getUser() {
    const char* user;
    pam_get_user(_pamHandle, &user, "");
    return std::string(user);
}
