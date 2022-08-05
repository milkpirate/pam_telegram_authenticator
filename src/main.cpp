//
// Created by pschroeder on 8/2/22.
//

#include <string>
#include <tgbot/tgbot.h>
#include <fmt/core.h>
#include <fmt/chrono.h>

using namespace std;

std::string apiKey = "5451040797:AAG4fQ1hvul8oC_IXyWnfsxtPGLvKC4m8zk";
auto chatId = 1390453959;


class TgAuthBot : public TgBot::Bot {

public:
    TgAuthBot(const std::string& apiKey, std::int64_t chatId);
    bool isApproved(const std::string& prompt);

private:
    TgBot::InlineKeyboardButton::Ptr _getButton(const std::string text, const std::string data);
    std::int64_t _chatId;
    static constexpr auto _timeout = std::chrono::seconds(30);
    static constexpr auto _approve = "approve";
    static constexpr auto _decline = "decline";
};


TgAuthBot::TgAuthBot(const std::string& apiKey, std::int64_t chatId):
        _chatId(chatId),
        TgBot::Bot(apiKey)
{
}

TgBot::InlineKeyboardButton::Ptr TgAuthBot::_getButton(const std::string text, const std::string data) {
    TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
    btn->text = text;
    btn->callbackData = data;
    return btn;
}

bool TgAuthBot::isApproved(const std::string& prompt) {
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> keyboardRow;

    keyboardRow.push_back(_getButton("✅ Approve", _approve));
    keyboardRow.push_back(_getButton("Decline ❌", _decline));
    keyboard->inlineKeyboard.push_back(keyboardRow);

    auto api = this->getApi();
    auto req = api.sendMessage(_chatId, prompt, false, 0, keyboard, "MarkdownV2", false);
    auto approve = false;
    auto decided = false;
    auto timeout = false;

    std::condition_variable condVar;

    this->getEvents().onCallbackQuery([this, &api, &approve, &decided](TgBot::CallbackQuery::Ptr query) {
        if(query->data != _approve && query->data != _decline) return;
        approve = (query->data == _approve);
        api.sendMessage(_chatId, "ACK", false, 0, nullptr, "MarkdownV2", false);
        api.deleteMessage(query->message->chat->id, query->message->messageId);
        decided = true;
    });

    signal(SIGINT, [](int s) { exit(0); });

    try {
        api.deleteWebhook();
        TgBot::TgLongPoll longPoll(*this);
        while (!decided || !timeout ) longPoll.start();
    } catch (exception& e) {
        cout<< e.what() <<endl;
    }

    return approve;
}

std::string sanitizeForTelegram(std::string msg) {
    std::vector<std::string> charsToEscape = {"_", "*", "~", "!", "-"};
    for(const auto& from : charsToEscape) {
        size_t start_pos = 0;
        while ((start_pos = msg.find(from, start_pos)) != std::string::npos) {
            std::string to = '\\'+from;
            msg.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }
    return msg;
}

int main() {
    auto bot = TgAuthBot(apiKey, chatId);
    auto user = "user", rhost = "rhost", service = "test", host = "nuc";
    auto code = 23;


    auto dateTime = sanitizeForTelegram(fmt::format("{:%FT%TZ}", std::chrono::system_clock::now()));
    cout<< dateTime <<endl;

    auto msg = fmt::format(
            "🔒 *Approve sign\\-in?*\n\n"
            "Login request:\n\n"
            "```\n"
            "Service:   {1}\n"
            "User:      {2}@{3}\n"
            "Host:      {0}\n"
            "```\n"
            "{4}",
            host, service, user, rhost, dateTime);

    std::cout << bot.isApproved(msg) << std::endl;
    return 0;
};