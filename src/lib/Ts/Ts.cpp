#include "Ts.hpp"

#include <utility>

Ts::Ts(string apiKey, string chatId, shared_ptr<spdlog::logger> log):
    _apiKey(std::move(apiKey)),
    _chatId(std::move(chatId)),
    _log(std::move(log))
{
}

// TODO: parse response into a DTO

uint64_t Ts::sendMessage(std::basic_string<char> msg) {
    json payload = {
        {"text", msg},
        {"chat_id", _chatId},
        {"parse_mode", "MarkdownV2"},
        {"disable_notification", false}
//        {"protect_content", true},
    };

    auto response = _curlPost(payload, "sendMessage");
    uint64_t msgId = response["ok"].get<bool>() ? response["result"]["message_id"].get<uint64_t>() : 0;
    return msgId;
}

bool Ts::deleteMessage(uint64_t msgId) {
    json payload = {
        {"message_id", msgId},
        {"chat_id", _chatId}
    };

    auto resp = _curlPost(payload, "deleteMessage");
    return resp["ok"].get<bool>();
}

size_t Ts::_writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    auto bytestWritten = size * nmemb;
    ((std::string*)userp)->append((char*)contents, bytestWritten);
    return bytestWritten;
}

json Ts::_curlPost(json& payload, const basic_string<char>& func) {
    string url = _telegram_api_url+_apiKey+"/"+func;
    _slog->trace("URL to request: {}", url);

    auto json_response = json();

    auto curl = curl_easy_init();
    if(!curl) {
        _slog->error("Curl instance could not be created!");
        curl_global_cleanup();
        return json_response;
    }

    _slog->trace("Setting curl options...");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, payload.dump().c_str());
    _slog->trace("Payload: {}", payload.dump(2));

    struct curl_slist *headers;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCallback);

    string curl_response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_response);

    auto ret = curl_easy_perform(curl);
    _slog->trace("Curl return code: {}", ret);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (ret != CURLE_OK) {
        _slog->error("curl_easy_perform() failed: {}", curl_easy_strerror(ret));
        return json_response;
    }

    json_response = json::parse(curl_response);
    _slog->trace("JSON parsed response: {}", json_response.dump(2));
    return json_response;
}