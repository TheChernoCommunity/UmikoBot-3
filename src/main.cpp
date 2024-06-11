#include "common.hpp"

#include <dpp/dpp.h>

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <stdio.h>
#include <vector>

class UmikoBot;

using CommandCallback = void (*)(const UmikoBot& bot, const dpp::slashcommand_t& event);

struct UmikoCommand {
    std::string name         = "";
    std::string description  = "";
    CommandCallback callback = NULL;
};

class UmikoBot {
public:
    UmikoBot(const std::string& token) : internalBot(token) {
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

        internalBot.on_ready([this](const dpp::ready_t& event) {
            UNUSED(event);

            log_info("Bot ready!");

            // Here, register_bot_commands is just a unique identifier, not an actual struct that
            // we've defined. Strange...
            if (dpp::run_once<struct register_bot_commands>()) {
                create_all_commands();
            }
        });

        internalBot.on_slashcommand([this](const dpp::slashcommand_t& event) {
            std::string commandName = event.command.get_command_name();
            auto search             = commands.find(commandName);

            if (search == commands.end()) {
                // @Incomplete: Formatted messages.
                // log_error("Tried to use slash command '%s', doesn't exist in our records.", commandName);
                return;
            }

            UmikoCommand& command = search->second;
            if (!command.callback) {
                // @Incomplete: Formatted messages.
                // log_error("Registered command '%s' has no associated callback.", commandName);
                return;
            }

            command.callback(*this, event);
        });
    }

    void run() {
        internalBot.start(dpp::st_wait);
    }

    void register_command(const UmikoCommand& command) {
        commands.emplace(command.name, command);
    }

    void log_info(const std::string& message) {
        internalBot.log(dpp::loglevel::ll_info, message);
    }

    void log_error(const std::string& message) {
        internalBot.log(dpp::loglevel::ll_error, message);
    }

private:
    void register_global_commands() {
        register_command({ .name = "ping", .description = "Ping pong!", .callback = handle_ping });
    }

    void create_all_commands() {
        register_global_commands();

        for (auto [name, command] : commands) {
            dpp::slashcommand newCommand { name, command.description, internalBot.me.id };
            internalBot.global_command_create(newCommand);
        }
    }

    static void handle_ping(const UmikoBot& bot, const dpp::slashcommand_t& event) {
        UNUSED(bot);
        event.reply("Pong!");
    }

public:
    std::unordered_map<std::string, UmikoCommand> commands;

private:
    dpp::cluster internalBot;
    std::shared_ptr<spdlog::async_logger> logger;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Error: incorrect usage!\n");
        printf("Usage: UmikoBot-3 <token>\n");

        return 1;
    }

    std::string token = argv[1];
    if (token.empty()) {
        printf("No token provided.\n");
        return 1;
    }

    UmikoBot bot { token };
    bot.run();

    return 0;
}
