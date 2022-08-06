#include "TgAuthBot.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

TgAuthBot::TgAuthBot(const std::string& apiKey, std::int64_t chatId, const std::uint32_t timeout, const std::shared_ptr<spdlog::logger>& log):
    _chatId(chatId),
    _log(log),
    TgBot::Bot(apiKey)
{
    _timeout = std::chrono::seconds(timeout);
}

template <typename B>
typename B::Ptr TgAuthBot::_getButton(const std::string &text, const std::string& data) {
    typename B::Ptr btn(new B);
    btn->text = text;
    btn->callbackData = data;
    return btn;
}

std::string TgAuthBot::_getUUID() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

bool TgAuthBot::isApproved(const std::string& prompt) {
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    std::vector<TgBot::InlineKeyboardButton::Ptr> keyboardRow;

    auto approveBtn = _getButton<TgBot::InlineKeyboardButton>("✅ Approve", _getUUID());
    auto declineBtn = _getButton<TgBot::InlineKeyboardButton>("Decline ❌", _getUUID());

    keyboardRow.push_back(approveBtn);
    keyboardRow.push_back(declineBtn);

    keyboard->inlineKeyboard.push_back(keyboardRow);

    TgBot::Message::Ptr request;
    try {
        request = this->getApi().sendMessage(_chatId, prompt, false, 0, keyboard, "MarkdownV2", false);
    } catch (std::exception& e) {
        _log->error("Sending authorization request failed: {}", e.what());
    }

    std::atomic<bool> approved = false, decided = false;
    this->getEvents().onCallbackQuery([this, &approveBtn, &decided, &approved, &request](const TgBot::CallbackQuery::Ptr& query) {
        auto queryMsgId = query->message->messageId;
        auto data = query->data;

        _log->trace("Query received. msg.id,data: {},{}", queryMsgId, data);

        if(queryMsgId != request->messageId) return;

        approved = (data == approveBtn->callbackData);
        decided = true;
    });

    auto timeout = false;
    _log->trace("Poll until decision...");
    try {
        this->getApi().deleteWebhook();
        TgBot::TgLongPoll pollUntilEvent(*this, 100, 3);

        auto start = std::chrono::system_clock::now();
        while(!decided && !timeout) {
            pollUntilEvent.start();
            auto now = std::chrono::system_clock::now();
            timeout = (start+_timeout < now);
        }
    } catch (std::exception& e) {
        _log->error("Could not create polling object: {}", e.what());
    }

    if(timeout) { _log->trace("Timed out {}", approved ? "approved" : "declined"); }
    _log->trace("Received answer: {}", approved ? "approved" : "declined");
    this->getApi().deleteMessage(request->chat->id, request->messageId);
    return approved;
}
