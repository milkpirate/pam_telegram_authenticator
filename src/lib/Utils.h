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
#include <stdio.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <fmt/core.h>
#include <fmt/chrono.h>


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
    } else if (level == "infoMsg") {
        spdLevel = spdlog::level::info;
    } else if (level == "warn") {
        spdLevel = spdlog::level::warn;
    } else if (level == "warning") {
        spdLevel = spdlog::level::warn;
    } else if (level == "err") {
        spdLevel = spdlog::level::err;
    } else if (level == "errorMsg") {
        spdLevel = spdlog::level::err;
    }

    return spdLevel;
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

std::string getHostName() {
    std::ifstream hostNameFile("/proc/sys/kernel/hostname");
    std::stringstream hostNameStream;
    hostNameStream << hostNameFile.rdbuf();
    return hostNameStream.str();
}

std::string approvalRequestMsg(std::string service, std::string host, std::string id, std::string user, std::string rHost) {
    auto dateTime = sanitizeForTelegram(fmt::format("{:%FT%TZ}", std::chrono::system_clock::now()));

    for(std::string* part : {&service, &user, &rHost, &dateTime, &host, &id}) {
        std::string tmp = sanitizeForTelegram(*part);
        part = &tmp;
    }

    return fmt::format(
        "🔒 *Approve sign\\-in?*\n\n"
        "*{5}*\n\n"
        "```\n"
        "Service:   {1}\n"
        "User:      {2}@{3}\n"
        "Host:      {0}\n"
        "```\n"
        "{4}",
        host, service, user, rHost, dateTime, id
    );
}
