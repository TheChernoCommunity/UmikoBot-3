#pragma once

#include "common.hpp"

#include <dpp/dpp.h>

#include <nlohmann/json.hpp>

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
    //
    // @Important @Volatile
    //
    // If you're adding members to this struct, be sure to update the
    // macro invocation below with the new members you've added to allow for automatic
    // serialisation and de-serialisation!
    //

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UserData, coinsInCents);

    int coinsInCents = 0;
};

struct GuildData {
    //
    // @Important @Volatile
    //
    // If you're adding members to this struct, be sure to update the
    // macro invocation below with the new members you've added to allow for automatic
    // serialisation and de-serialisation!
    //

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(GuildData, allUsersData, currencyLongName, currencyShortName);

    std::unordered_map<dpp::snowflake, UserData> allUsersData;

    std::string currencyLongName  = "Umiko Coins";
    std::string currencyShortName = "UC";
};

class UmikoBot {
public:
    UmikoBot(const std::string& token);
    ~UmikoBot();

    UmikoBot(const UmikoBot&)            = delete;
    UmikoBot& operator=(const UmikoBot&) = delete;

    void run();

    void register_command(const UmikoCommand& command);

    GuildData& get_guild_data(const dpp::slashcommand_t& event);
    UserData& get_user_data(const dpp::slashcommand_t& event);

    UMIKO_PRINTF_LIKE void log_info(const char* format, ...);
    UMIKO_PRINTF_LIKE void log_error(const char* format, ...);

private:
    void create_all_commands();

    void save_to_file();
    void load_from_file();

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
