#pragma once

#include <string>
#include <queue>
#include <chrono>

#include <tgbot/tgbot.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

class TgAuthBot : public TgBot::Bot {
    public:
        TgAuthBot(const std::string& apiKey, std::int64_t chatId, const std::shared_ptr<spdlog::logger> log);
        bool isApproved(const std::string& prompt);

    private:
        static TgBot::InlineKeyboardButton::Ptr _getButton(const std::string& text, const std::string& data);
        std::shared_ptr<spdlog::logger> _log;
        std::int64_t _chatId;
        static constexpr auto _timeout = std::chrono::seconds(30);
        static constexpr auto _approve = "approve";
        static constexpr auto _decline = "decline";
};
