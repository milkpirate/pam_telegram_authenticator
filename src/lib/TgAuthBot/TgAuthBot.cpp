#include "TgAuthBot.hpp"


TgAuthBot::TgAuthBot(const std::string& apiKey, std::int64_t chatId, const std::shared_ptr<spdlog::logger> log):
    _chatId(chatId),
    _log(log),
    TgBot::Bot(apiKey)
{
}

TgBot::InlineKeyboardButton::Ptr TgAuthBot::_getButton(const std::string &text, const std::string& data) {
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

    try {
        api.sendMessage(_chatId, prompt, false, 0, keyboard, "MarkdownV2", false);
    } catch (std::exception& e) {
        _log->error("Sending authorization request failed: {}", e.what());
    }

    auto approve = false;
    auto decided = false;

    this->getEvents().onCallbackQuery([this, &api, &approve, &decided](const TgBot::CallbackQuery::Ptr& query) {
        if(query->data != _approve && query->data != _decline) return;
        approve = (query->data == _approve);
        api.sendMessage(_chatId, "ACK", false, 0, nullptr, "MarkdownV2", false);
        api.deleteMessage(query->message->chat->id, query->message->messageId);
        decided = true;
    });

    try {
        api.deleteWebhook();
        TgBot::TgLongPoll pollUntilEvent(*this);
        while(!decided) pollUntilEvent.start();
    } catch (std::exception& e) {
        _log->error("Could not create polling object: {}", e.what());
    }

    return approve;
}

