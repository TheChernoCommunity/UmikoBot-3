#pragma once

#include <dpp/dpp.h>

#include <spdlog/async.h>

#include <string>

class UmikoBot;

using CommandCallback = void (*)(const UmikoBot& bot, const dpp::slashcommand_t& event);

struct UmikoCommand {
    std::string name         = "";
    std::string description  = "";
    CommandCallback callback = NULL;
};

class UmikoBot {
public:
    UmikoBot(const std::string& token);

    void run();

    void register_command(const UmikoCommand& command);

    void log_info(const std::string& message);
    void log_error(const std::string& message);

private:
    void create_all_commands();

public:
    std::unordered_map<std::string, UmikoCommand> commands;

private:
    dpp::cluster internalBot;
    std::shared_ptr<spdlog::async_logger> logger;
};
