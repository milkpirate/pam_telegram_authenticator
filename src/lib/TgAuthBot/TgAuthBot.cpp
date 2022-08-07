#include "TgAuthBot.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace TgAuthBot {
    TgAuthBot::TgAuthBot(const std::string& apiKey, std::int64_t chatId, const std::uint32_t timeout):
        _chatId(chatId),
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
        request = this->getApi().sendMessage(_chatId, prompt, false, 0, keyboard, "MarkdownV2", false);

        std::atomic<bool> approved = false, decided = false;
        this->getEvents().onCallbackQuery([this, &approveBtn, &decided, &approved, &request](const TgBot::CallbackQuery::Ptr& query) {
            auto queryMsgId = query->message->messageId;
            auto data = query->data;

            if(queryMsgId != request->messageId) return;

            approved = (data == approveBtn->callbackData);
            decided = true;
        });

        auto timeout = false;
    //    _log->trace("Poll until decision...");

        this->getApi().deleteWebhook();
        TgBot::TgLongPoll pollUntilEvent(*this, 100, 3);

        auto start = std::chrono::system_clock::now();
        while(!decided && !timeout) {
            pollUntilEvent.start();
            auto now = std::chrono::system_clock::now();
            timeout = (start+_timeout < now);
        }

    //    if(timeout) { _log->trace("Timed out {}", approved ? "approved" : "declined"); }
    //    _log->trace("Received answer: {}", approved ? "approved" : "declined");
        this->getApi().deleteMessage(request->chat->id, request->messageId);
        return approved;
    }
};
