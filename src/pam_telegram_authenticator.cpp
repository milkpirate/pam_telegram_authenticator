#include <filesystem>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>
#include <yaml-cpp/yaml.h>


#include "PamWrapper/PamWrapper.hpp"
#include "TgAuthBot/TgAuthBot.hpp"
#include "Utils.h"

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv) {
    auto slog = spdlog::syslog_logger_mt("syslog", "pam_telegram_authenticator", LOG_PID);
    slog->set_level(spdlog::level::debug);
    slog->info("Authenticating...");

    auto pam = PamWrapper(pamh);
    auto user = pam.getUser();
    slog->debug("Got user: {}", user);

    // TODO: read config into DTO
    const std::filesystem::path configFile = "/home/"+user+"/.telegram_authenticator.yaml";
    slog->trace("Reading config file: {}", configFile.string());
    auto config = YAML::LoadFile(configFile);
    slog->trace("Config read:\n{}", Dump(config));

    auto logLevel = config["log_level"].as<std::string>();
    slog->debug("Setting log level to: {}", logLevel);
    slog->set_level(stringToSpdLevelLookup(logLevel));

//    Ts ts = Ts(config["api_key"].as<std::string>(), config["chat_id"].as<std::string>(), slog);
//    slog->debug("Telegram sender initialized");

    auto tgBot = TgAuthBot(config["api_key"].as<std::string>(), config["chat_id"].as<std::string>());
    slog->trace("decision: {}", tgBot.isApproved("decide!"));

    auto sentOtp = getOTP(2);
    slog->trace("Generated OTP: {}", sentOtp);

    auto msg = optMsg("sericve", sentOtp, "host", "user");
    slog->trace("Telegram message: {}", msg);

//    auto msgId = ts.sendMessage(msg);
//    slog->trace("Sent message ID: {}", msgId);

    pam.info("OTP sent via Telegram.");

    auto maxTries = 3;
    auto currTries = 1;
    auto otpMatches = false;

    while(!otpMatches && (currTries <= maxTries)) {
        auto userOtp = pam.readPassword(fmt::format("Telegram verification code (try {:d}/{:d}): ", currTries, maxTries));

        userOtp = rmSpaces(userOtp);
        sentOtp = rmSpaces(sentOtp);

        otpMatches = (sentOtp == userOtp);
        slog->debug("OTPs match: '{}', user's OTP '{}', our OTP '{}'", otpMatches, userOtp, sentOtp);
        currTries++;
    }

//    ts.deleteMessage(msgId);
    slog->trace("OTP message deleted");

    if(!otpMatches) {
        auto userEm = pam.readPassword("One may still enter an emergency key: ");
        userEm = rmSpaces(userEm);
        slog->debug("Emergency key given: '{}'", userEm);
        auto emKeys = config["emergency_keys"].as<std::vector<std::string>>();
        slog->trace("> keys read");
        otpMatches = removeFromVector<std::string>(userEm, &emKeys);
        slog->debug("Given emergency key found: {}", otpMatches);
        config["emergency_keys"] = emKeys;
        slog->trace("New config:\n{}", Dump(config));
        writeFile(Dump(config), configFile);
        slog->trace("New config written to: {}", configFile.string());
    }

    slog->info("Authenticated: {}", otpMatches);
    return otpMatches ? PAM_SUCCESS : PAM_AUTH_ERR;
}

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t * pamh, int flags, int argc, const char ** argv ) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt( pam_handle_t * pamh, int flags, int argc, const char ** argv ) {
    return PAM_SUCCESS;
}

//PAM_EXTERN int pam_sm_open_session( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
//    FILE *fdx;
//    char *user;
//    int retval;
//    //char path[512];
//    char *filename=NULL;
//
//    retval = pam_get_item(pamh, PAM_USER, (void *) &user);
//
//    if (retval == PAM_SUCCESS || user != NULL || *user != '\0') {
//        retval = pam_get_data(pamh, "pam_2fa_user_conf_filename", (const void **) &filename);
//        if (retval != PAM_NO_MODULE_DATA) {
//            update_safe_code_entry(filename);
//        }
//    }
//
//    if (filename != NULL)
//        free(filename); // IT was allocated at pam_sm_authenticate
//
//    return PAM_SUCCESS;
//}