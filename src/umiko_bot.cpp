#include "common.hpp"
#include "umiko_bot.hpp"

#include <dpp/dpp.h>

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <stdarg.h>

UmikoBot::UmikoBot(const std::string& token) : internalBot(token) {
    //
    // Initialising spdlog.
    //

    // This code is mostly a direct grab from https://dpp.dev/spdlog.html.

    spdlog::init_thread_pool(8192, 2);

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(new spdlog::sinks::stdout_color_sink_mt());
    sinks.emplace_back(new spdlog::sinks::rotating_file_sink_mt("log/umiko.log", 1024 * 1024 * 5, 10));

    logger = std::make_shared<spdlog::async_logger>("logs", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                    spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);

    logger->set_pattern("[%Y-%m-%d] %l: %v");
    logger->set_level(spdlog::level::level_enum::debug);

    internalBot.on_log([this](const dpp::log_t& event) {
        switch (event.severity) {
        case dpp::ll_trace:    logger->trace("{}", event.message); break;
        case dpp::ll_debug:    logger->debug("{}", event.message); break;
        case dpp::ll_info:     logger->info("{}", event.message); break;
        case dpp::ll_warning:  logger->warn("{}", event.message); break;
        case dpp::ll_error:    logger->error("{}", event.message); break;

        case dpp::ll_critical:
        default:               logger->critical("{}", event.message); break;
        }
    });

    //
    // Initialising the bot.
    //

    load_from_file();

    internalBot.on_ready([this](const dpp::ready_t& event) {
        UNUSED(event);

        log_info("Bot ready!");

        // Here, register_bot_commands is just a unique identifier, not an actual struct that
        // we've defined. Strange...
        if (dpp::run_once<struct register_bot_commands>()) {
            create_all_commands();
        }

        // Set up a timer to save once every minute.
        internalBot.start_timer(
            [this](const dpp::timer& timer) {
                UNUSED(timer);
                save_to_file();
            },
            60);
    });

    internalBot.on_slashcommand([this](const dpp::slashcommand_t& event) {
        std::string commandName = event.command.get_command_name();
        auto search             = commands.find(commandName);

        if (search == commands.end()) {
            log_error("Tried to use slash command '%s', doesn't exist in our records.", commandName.c_str());
            return;
        }

        UmikoCommand& command = search->second;
        if (!command.callback) {
            log_error("Registered command '%s' has no associated callback.", commandName.c_str());
            return;
        }

        command.callback(*this, event);
    });
}

UmikoBot::~UmikoBot() {
    save_to_file();
}

void UmikoBot::run() {
    internalBot.start(dpp::st_wait);
}

void UmikoBot::register_command(const UmikoCommand& command) {
    commands.emplace(command.name, command);
}

GuildData& UmikoBot::get_guild_data(const dpp::slashcommand_t& event) {
    dpp::snowflake guildId              = event.command.get_guild().id;
    auto [guildDataPair, guildInserted] = allGuildsData.try_emplace(guildId, GuildData {});
    GuildData& guildData                = guildDataPair->second;

    return guildData;
}

UserData& UmikoBot::get_user_data(const dpp::slashcommand_t& event) {
    GuildData& guildData = get_guild_data(event);

    dpp::snowflake userId             = event.command.get_issuing_user().id;
    auto [userDataPair, userInserted] = guildData.allUsersData.try_emplace(userId, UserData {});
    UserData& userData                = userDataPair->second;

    return userData;
}

constexpr const char* UMIKO_DATA_FILE_NAME = "data.json";

void UmikoBot::save_to_file() {
    nlohmann::json json = allGuildsData;

    std::ofstream outputFile { UMIKO_DATA_FILE_NAME, std::ios::out | std::ios::trunc };
    if (!outputFile.is_open()) {
        log_error("Failed to open output file '%s' to save data.", UMIKO_DATA_FILE_NAME);
        return;
    }

    outputFile << json << std::endl;

    // We probably want to comment this out when not using it for debugging, else it will clog up the
    // output file with a whole bunch of these messages (since we're probably saving pretty often).
    log_info("Saved data to JSON file.");
}

void UmikoBot::load_from_file() {
    std::ifstream inputFile { UMIKO_DATA_FILE_NAME };
    if (!inputFile.is_open()) {
        log_info("Failed to open output file '%s' to load data, using defaults.", UMIKO_DATA_FILE_NAME);
        return;
    }

    nlohmann::json json;

    try {
        json = nlohmann::json::parse(inputFile);
    } catch (nlohmann::json::parse_error) {
        log_error("Malformed json file '%s', using defaults.", UMIKO_DATA_FILE_NAME);
        return;
    }

    allGuildsData = json.template get<std::unordered_map<dpp::snowflake, GuildData>>();
    log_info("Loaded data from file '%s'.\nData: %s", UMIKO_DATA_FILE_NAME, to_string(json).c_str());
}

std::string format_to_string(const char* format, ...) {
    va_list args;
    va_start(args, format);

    std::string message = vformat_to_string(format, args);

    va_end(args);
    return message;
}

std::string vformat_to_string(const char* format, va_list args) {
    // We need a copy of the args list because we need to call vsnprintf twice.
    va_list argsCopy;
    va_copy(argsCopy, args);

    size_t bufferLength = vsnprintf(nullptr, 0, format, args) + 1;
    char* buffer        = new char[bufferLength]();
    vsnprintf(buffer, bufferLength, format, argsCopy);

    std::string result = buffer;

    delete[] buffer;
    va_end(argsCopy);

    return result;
}

void UmikoBot::log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);

    std::string message = vformat_to_string(format, args);
    internalBot.log(dpp::loglevel::ll_info, message);

    va_end(args);
}

void UmikoBot::log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);

    std::string message = vformat_to_string(format, args);
    internalBot.log(dpp::loglevel::ll_error, message);

    va_end(args);
}

void UmikoBot::create_all_commands() {
    void register_general_commands(UmikoBot & bot);
    void register_currency_commands(UmikoBot & bot);

    register_general_commands(*this);
    register_currency_commands(*this);

    std::vector<dpp::slashcommand> newCommands;
    newCommands.reserve(commands.size());

    for (auto& [name, command] : commands) {
        newCommands.emplace_back(name, command.description, internalBot.me.id);
        dpp::slashcommand& newCommand = newCommands.back();

        if (command.moderatorOnly) {
            // This is set to the manage messages permission for now because that's what we had
            // before, but this could be improved.
            //
            // @Incomplete: Allow setting permissions for individual commands, such as requiring
            // the ban members permission for the warn command (if we even want that command).
            //
            // @Incomplete: Create a command for administrator only that adds specific users to
            // a whitelist, so that they can use elevated commands.
            newCommand.set_default_permissions(dpp::p_manage_messages);
        }
    }

    // We only deploy to the Cherno Community server for now...
    internalBot.guild_bulk_command_create(newCommands, 776172716268847114);
}
