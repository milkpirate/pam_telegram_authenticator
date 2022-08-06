#include <filesystem>

#include <security/pam_modules.h>
#include <security/pam_ext.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>
#include <yaml-cpp/yaml.h>

#include "PamWrapper/PamWrapper.hpp"
#include "TgAuthBot/TgAuthBot.hpp"
#include "Utils.h"

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int authenticate(PamWrapper& pam, int flags, int argc, const char** argv, const std::shared_ptr<spdlog::logger>& slog) {
    slog->info("Authenticating...");

    auto user = pam.getUser();
    slog->info("User tries to log in: {}", user);

    // TODO: read config into DTO
    const std::filesystem::path configFile = "/home/"+user+"/.telegram_authenticator.yaml";
    slog->trace("Reading config file: {}", configFile.string());
    auto config = YAML::LoadFile(configFile);
    slog->trace("Config read:\n{}", Dump(config));

    auto logLevel = config["log_level"].as<std::string>();
    slog->debug("Setting log level to: {}", logLevel);
    slog->set_level(stringToSpdLevelLookup(logLevel));

    slog->trace(pam.readInput("foo: "));

    auto requestId = fmt::format("{:02}", randomNumberHw(2));
    pam.info(fmt::format("Approve request, with ID: {}", requestId));

    auto tgBot = TgAuthBot(config["api_key"].as<std::string>(), config["chat_id"].as<std::int64_t>(), 30, slog);
    slog->trace("Telegram AuthBot created");

    auto msg = approvalRequestMsg(pam.getService(), getHostName(), requestId, pam.getUser(), pam.getRHost());
    slog->trace("Telegram message: {}", msg);

    auto approved = tgBot.isApproved(msg);
    slog->trace("User approved request: {}", approved ? "yes" : "no");

    slog->info("Authenticated: {}", approved);
    return approved ? PAM_SUCCESS : PAM_AUTH_ERR;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv) {
    auto slog = spdlog::syslog_logger_mt("syslog", "pam_telegram_authenticator", LOG_PID);
    slog->set_level(spdlog::level::trace);

    auto pam = PamWrapper(pamh, slog);
    pam_info(pamh, "post wrap");
    try {
        pam_info(pamh, "pre auth");
        return authenticate(pam, flags, argc, argv, slog);
    } catch (std::exception& e) {
        slog->error("error: {}", e.what());
    }
}

/* expected hook */
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
