#include "common.hpp"

#include <dpp/dpp.h>

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
        internalBot.on_ready([this](dpp::ready_t event) {
            UNUSED(event);

            umiko_info("Bot ready!");

            // Here, register_bot_commands is just a unique identifier, not an actual struct that
            // we've defined. Strange...
            if (dpp::run_once<struct register_bot_commands>()) {
                create_all_commands();
            }
        });

        internalBot.on_slashcommand([this](dpp::slashcommand_t event) {
            std::string commandName = event.command.get_command_name();
            auto search             = commands.find(commandName);

            if (search == commands.end()) {
                umiko_error("Tried to use slash command '%s', doesn't exist in our records.", commandName.c_str());
                return;
            }

            UmikoCommand& command = search->second;
            if (!command.callback) {
                umiko_error("Registered command '%s' has no associated callback.", commandName.c_str());
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
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Error: incorrect usage!\n");
        printf("Usage: UmikoBot-3 <token>\n");

        return 1;
    }

    std::string token = argv[1];
    if (token.empty()) {
        umiko_error("No token provided.\n");
        return 1;
    }

    UmikoBot bot { token };
    bot.run();

    return 0;
}
