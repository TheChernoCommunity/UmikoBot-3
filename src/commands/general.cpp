#include "commands/general.hpp"
#include "common.hpp"
#include "umiko_bot.hpp"

void register_general_commands(UmikoBot& bot) {
    bot.register_command({ .name = "ping", .description = "Ping pong!", .callback = handle_ping });
    bot.register_command(
        { .name = "moderator-ping", .description = "Moderator ping pong!", .callback = handle_ping, .moderatorOnly = true });
}

void handle_ping(const UmikoBot& bot, const dpp::slashcommand_t& event) {
    UNUSED(bot);
    event.reply("Pong!");
}
