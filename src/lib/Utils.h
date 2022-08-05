#pragma once

#include <chrono>
#include <iostream>
#include <random>
#include <cmath>
#include <string>
#include <regex>
#include <locale>
#include <utility>
#include <cctype>
#include <algorithm>
#include <fstream>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>  // for fmt


spdlog::level::level_enum stringToSpdLevelLookup(std::string level) {
    spdlog::level::level_enum spdLevel = spdlog::level::info;

    // why is everything so cumberson in C++?!
    std::for_each(level.begin(), level.end(), [](char& c) {
        c = ::tolower(c);
    });

    if (level == "trace") {
        spdLevel = spdlog::level::trace;
    } else if (level == "debug") {
        spdLevel = spdlog::level::debug;
    } else if (level == "info") {
        spdLevel = spdlog::level::info;
    } else if (level == "warn") {
        spdLevel = spdlog::level::warn;
    } else if (level == "warning") {
        spdLevel = spdlog::level::warn;
    } else if (level == "err") {
        spdLevel = spdlog::level::err;
    } else if (level == "error") {
        spdLevel = spdlog::level::err;
    }

    return spdLevel;
}

void writeFile(const std::string& content, const std::filesystem::path path) {
    auto file = std::ofstream(path, std::ofstream::out);
    file << content;
    file.close();
}

template <typename T>
bool removeFromVector(const T e, std::vector<T>* v) {
    auto it = std::find(v->begin(), v->end(), e);
    if(it == v->end()) return false;
    v->erase(it);
    return true;
}

uint64_t randomNumber(const uint8_t digits, std::mt19937 mtRand) {
    std::uniform_int_distribution<uint64_t> distribution(0, (uint64_t)pow(10, digits)); // define the range
    return distribution(mtRand);
}

uint64_t randomNumberHw(const uint8_t digits = 6) {
    std::random_device randDev; // obtain a random number from hardware
    std::mt19937 mtRand(randDev()); // seed the generator
    return randomNumber(digits, mtRand);
}

uint64_t randomNumberTimeSeeded(const uint8_t digits = 6) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    uint64_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    auto seed = sec / 30;
    seed ^= __RANDOM_SEED__;
    std::mt19937 mtRand(seed); // seed the generator
    return randomNumber(digits, mtRand);
}

std::string getOTP(const uint8_t digitGroups = 2) {
    const auto digitGroupsLength = 3;
    auto otpLength = digitGroups * digitGroupsLength;
    const auto randNum = randomNumberTimeSeeded(otpLength);
    auto number = fmt::format("{:0<{}d}", randNum, otpLength);
    while(otpLength > digitGroupsLength) {
        otpLength -= digitGroupsLength;
        number = number.insert(otpLength, " ");
    }
    return number;
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


std::string rmSpaces(const std::string& str) {
    auto tmp = str;
    tmp.erase(remove_if(tmp.begin(), tmp.end(), ::isspace), tmp.end());
    return tmp;
}


std::string optMsg(std::string service, const std::string& otp, std::string user, std::string rHost) {
    for(std::string* part : {&service, &user, &rHost}) {
        std::string tmp = sanitizeForTelegram(*part);
        part = &tmp;
    }
    return fmt::format(
        "🔒 *{} MFA*\n\nOTP:  `{}`\n\nLogin from: *{}@{}*",
        service, otp, user, rHost
    );
}
