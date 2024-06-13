#pragma once

#include "common.hpp"

#include <dpp/dpp.h>

#include <spdlog/async.h>

#include <string>
#include <unordered_map>

class UmikoBot;

using CommandCallback = void (*)(UmikoBot& bot, const dpp::slashcommand_t& event);

struct UmikoCommand {
    std::string name        = "";
    std::string description = "";

    CommandCallback callback = NULL;

    bool moderatorOnly = false;
};

struct UserData {
    int coinsInCents = 0;
};

struct GuildData {
    std::unordered_map<dpp::snowflake, UserData> allUsersData;

    std::string currencyLongName  = "Umiko Coins";
    std::string currencyShortName = "UC";
};

class UmikoBot {
public:
    UmikoBot(const std::string& token);

    void run();

    void register_command(const UmikoCommand& command);

    GuildData& get_guild_data(const dpp::slashcommand_t& event);
    UserData& get_user_data(const dpp::slashcommand_t& event);

    UMIKO_PRINTF_LIKE void log_info(const char* format, ...);
    UMIKO_PRINTF_LIKE void log_error(const char* format, ...);

private:
    void create_all_commands();

public:
    std::unordered_map<std::string, UmikoCommand> commands;
    std::unordered_map<dpp::snowflake, GuildData> allGuildsData;

private:
    dpp::cluster internalBot;
    std::shared_ptr<spdlog::async_logger> logger;
};

// @Cleanup: Move all the logging stuff to a new file?
std::string format_to_string(const char* format, ...);
std::string vformat_to_string(const char* format, va_list args);
