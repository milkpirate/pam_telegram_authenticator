#include "TgAuthBot.hpp"

TgAuthBot::TgAuthBot(const std::string& apiKey, const std::string& chatId):
    _apiKey(apiKey),
    _chatId(chatId),
    TgBot::Bot(apiKey)
{}

bool TgAuthBot::isApproved(const std::string& msg) {
//    TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
//    TgBot::KeyboardButton::Ptr lApprove(new TgBot::KeyboardButton);
//    TgBot::KeyboardButton::Ptr rDecline(new TgBot::KeyboardButton);
//    std::vector<TgBot::KeyboardButton::Ptr> keyboardRow;

    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    TgBot::InlineKeyboardButton::Ptr lApprove(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr rDecline(new TgBot::InlineKeyboardButton);
    std::vector<TgBot::InlineKeyboardButton::Ptr> keyboardRow;

    lApprove->text = "Approve ✅";
    rDecline->text = "Decline ❌";
    lApprove->callbackData = "approve";
    rDecline->callbackData = "decline";

    keyboardRow.push_back(lApprove);
    keyboardRow.push_back(rDecline);

    keyboard->inlineKeyboard.push_back(keyboardRow);

    this->getApi().sendMessage(_chatId, msg, false, 0, keyboard, "MarkdownV2", false);
}

bool TgAuthBot::deleteMessage(uint64_t msgId) {
    return true;
}
