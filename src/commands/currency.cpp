#include "common.hpp"
#include "umiko_bot.hpp"

static void handle_daily(UmikoBot& bot, const dpp::slashcommand_t& event) {
    GuildData& guildData = bot.get_guild_data(event);
    UserData& userData   = bot.get_user_data(event);

    // @Settings.
    constexpr int AMOUNT_TO_ADD  = 10000;
    userData.coinsInCents       += AMOUNT_TO_ADD;

    event.reply(format_to_string("You now have **%g** more %s in your wallet!", userData.coinsInCents / 100.0f,
                                 guildData.currencyLongName.c_str()));
}

static void handle_wallet(UmikoBot& bot, const dpp::slashcommand_t& event) {
    const dpp::user& issuingUser = event.command.get_issuing_user();

    GuildData& guildData = bot.get_guild_data(event);
    UserData& userData   = bot.get_user_data(event);

    dpp::embed embed {};
    embed.set_author(format_to_string("%s's Wallet", issuingUser.global_name.c_str()), "", issuingUser.get_avatar_url());

    embed.add_field("Current Credits",
                    format_to_string("%g %s", userData.coinsInCents / 100.0f, guildData.currencyLongName.c_str()));
    embed.add_field("Daily Streak", "0 (next bonus at 3)"); // @Incomplete.
    embed.add_field("Today's daily claimed?", "No"); // @Incomplete.

    event.reply(dpp::message { event.command.channel_id, embed });
}

void register_currency_commands(UmikoBot& bot) {
    bot.register_command({ .name = "daily", .description = "Collect your daily coins!", .callback = handle_daily });
    bot.register_command({ .name = "wallet", .description = "Check your net worth!", .callback = handle_wallet });
}
