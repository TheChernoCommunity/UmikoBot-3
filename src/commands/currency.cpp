#include "common.hpp"
#include "umiko_bot.hpp"

// This function is a bit hacky, but it should be fine for the use-case.
//
// The alternative solution would be to allocate new memory for the output, and have the caller
// free it after they're done. But that would be a bit messy on the caller side, and I'm aiming
// for as simple a usage as possible here.
const char* format_currency(const GuildData& guildData, int coinsInCents, bool longName = true) {
    constexpr int MAX_LENGTH = 1024;
    static char buffer[MAX_LENGTH] {};

    const char* name = guildData.currencyLongName.c_str();
    if (!longName) {
        name = guildData.currencyShortName.c_str();
    }

    int charactersTriedToWrite = snprintf(buffer, MAX_LENGTH, "%g %s", coinsInCents / 100.0f, name);
    if (charactersTriedToWrite < 0 || charactersTriedToWrite >= MAX_LENGTH) {
        // @Cleanup: Log an error or a warning here. Maybe we should consider making the bot a singleton
        // so we don't have to pass it around everywhere.
        return "";
    }

    return buffer;
}

static void handle_daily(UmikoBot& bot, const dpp::slashcommand_t& event) {
    GuildData& guildData = bot.get_guild_data(event);
    UserData& userData   = bot.get_user_data(event);

    // @Settings.
    constexpr int AMOUNT_TO_ADD  = 10000;
    userData.coinsInCents       += AMOUNT_TO_ADD;

    event.reply(format_to_string("You now have an extra **%s** in your wallet!", format_currency(guildData, AMOUNT_TO_ADD)));
}

static void handle_wallet(UmikoBot& bot, const dpp::slashcommand_t& event) {
    const dpp::user& issuingUser = event.command.get_issuing_user();

    GuildData& guildData = bot.get_guild_data(event);
    UserData& userData   = bot.get_user_data(event);

    dpp::embed embed {};
    embed.set_author(format_to_string("%s's Wallet", issuingUser.global_name.c_str()), "", issuingUser.get_avatar_url());

    embed.add_field("Current Credits", format_currency(guildData, userData.coinsInCents));
    embed.add_field("Daily Streak", "0 (next bonus at 3)"); // @Incomplete.
    embed.add_field("Today's daily claimed?", "No"); // @Incomplete.

    event.reply(dpp::message { event.command.channel_id, embed });
}

void register_currency_commands(UmikoBot& bot) {
    bot.register_command({ .name = "daily", .description = "Collect your daily coins!", .callback = handle_daily });
    bot.register_command({ .name = "wallet", .description = "Check your net worth!", .callback = handle_wallet });
}
