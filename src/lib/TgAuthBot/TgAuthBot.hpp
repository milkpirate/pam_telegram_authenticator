#pragma once

#include <string>
#include <queue>
#include <chrono>

#include <tgbot/tgbot.h>
#include <spdlog/logger.h>

namespace TgAuthBot {
    class TgAuthBot : public TgBot::Bot {
        public:
            TgAuthBot(const std::string &apiKey, std::int64_t chatId, std::uint32_t timeout);

            bool isApproved(const std::string &prompt);

        private:
            template<typename B>
            static typename B::Ptr _getButton(const std::string &text, const std::string &data);

            static std::string _getUUID();

            std::shared_ptr <spdlog::logger> _log;
            std::int64_t _chatId;
            std::chrono::seconds _timeout;
    };
}