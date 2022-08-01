#pragma once

#include <string>
#include <queue>

#include <tgbot/tgbot.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>

class TgAuthBot: public TgBot::Bot {
    public:
        TgAuthBot(const std::string& apiKey, const std::string& chatId);
        bool deleteMessage(uint64_t msgId);
        bool isApproved(const std::string& msg);

    private:
        std::queue<TgBot::Message::Ptr> _lastMsgs;
        std::string _chatId;
        std::string _apiKey;
};
