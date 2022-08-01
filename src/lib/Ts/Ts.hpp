#pragma once

#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>

using namespace std;
using json = nlohmann::json;

class Ts {
    constexpr auto static _telegram_api_url = "https://api.telegram.org/bot";

    public:
        Ts(
            string apiKey,
            string chatId,
            shared_ptr<spdlog::logger> log
        );

        static size_t _writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
        json _curlPost(json &payload, const basic_string<char>& func);
        bool deleteMessage(uint64_t msgId);
        uint64_t sendMessage(basic_string<char> msg);

    private:
        string _chatId;
        string _apiKey;
        shared_ptr<spdlog::logger> _log;
};
