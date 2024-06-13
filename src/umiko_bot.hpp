#pragma once

#include "common.hpp"

#include <dpp/dpp.h>

#include <spdlog/async.h>

#include <string>

class UmikoBot;

using CommandCallback = void (*)(const UmikoBot& bot, const dpp::slashcommand_t& event);

struct UmikoCommand {
    std::string name        = "";
    std::string description = "";

    CommandCallback callback = NULL;

    bool moderatorOnly = false;
};

class UmikoBot {
public:
    UmikoBot(const std::string& token);

    void run();

    void register_command(const UmikoCommand& command);

    UMIKO_PRINTF_LIKE void log_info(const char* format, ...);
    UMIKO_PRINTF_LIKE void log_error(const char* format, ...);

private:
    void create_all_commands();

public:
    std::unordered_map<std::string, UmikoCommand> commands;

private:
    dpp::cluster internalBot;
    std::shared_ptr<spdlog::async_logger> logger;
};
