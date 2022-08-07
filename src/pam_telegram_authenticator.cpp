#include <docopt/docopt.h>
#include <security/pam_modules.h>

#include "PamWrapper/PamWrapper.hpp"
#include "TgAuthBot/TgAuthBot.hpp"
#include "Utils.h"


static const char USAGE[] =
R"(Telegram Pluggable Authentication Modules

    One need to pass the chat ID (a number) and the API token (of the form
    `\\d{{10}}:[\w\d]{{35}}`. The log levels coincide with the syslog ones.
    See also: https://linux.die.net/man/3/syslog

    Usage:
        libpam_telegram_authenticator.so (-c <chat_id>) (-t <token>) [-l <level>]
        libpam_telegram_authenticator.so (-h | --help)
        libpam_telegram_authenticator.so --version

    Options:
        -c <chat_id> --chat_id=<chat_id>        Telegram chat ID.
        -t <token> --token=<token>              Telegram API token.
        -l <log_level> --log_level=<log_level>  Log level to use default: WARN].
        -h -? --help                            Print this screen.
        --version                               Print the version of this script.
)";


int authenticate(PamWrapper::PamWrapper& pam, int flags, int argc, const char** argv) {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, { argv + 1, argv + argc }, true, "Naval Fate 2.0");

    for(auto const& arg : args) {
        pam.error(arg.first);
        pam.error(arg.second);
        pam.error("=====");
    }

    pam.debug("debug");
    pam.info("info {}");
    pam.notice("notice");
    pam.warn("warn");
    pam.error("error");

    pam.info("Authenticating...");
    pam.info("foo");

    auto user = pam.getUser();
    pam.info("User tries to log in: {}", user);

//    auto logLevel = config["log_level"].as<std::string>();
//    pam.debug("Setting log level to: {}", logLevel);

    pam.debug(pam.readInput("foo: "));

    auto requestId = fmt::format("{:02}", randomNumberHw(2));
    pam.infoMsg(fmt::format("Approve request, with ID: {}", requestId));

//    auto tgBot = TgAuthBot::TgAuthBot(config["api_key"].as<std::string>(), config["chat_id"].as<std::int64_t>(), 30);
    pam.debug("Telegram AuthBot created");

    auto msg = approvalRequestMsg(pam.getService(), getHostName(), requestId, pam.getUser(), pam.getRHost());
    pam.debug("Telegram message: {}", msg);

    auto approved = false; //tgBot.isApproved(msg);
    pam.debug("User approved request: {}", approved ? "yes" : "no");

    pam.infoMsg("Authenticated: {}", approved);
    return approved ? PAM_SUCCESS : PAM_AUTH_ERR;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv) {
    auto pam = PamWrapper::PamWrapper(pamh);

    try {
        return authenticate(pam, flags, argc, argv);
    } catch (std::exception& e) {
        pam.error(e.what());
    }
}

/* expected hook */
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
