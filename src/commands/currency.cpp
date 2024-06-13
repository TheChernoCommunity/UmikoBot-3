#include "common.hpp"
#include "umiko_bot.hpp"

static void handle_daily(UmikoBot& bot, const dpp::slashcommand_t& event) {
    dpp::snowflake guildId              = event.command.get_guild().id;
    auto [guildDataPair, guildInserted] = bot.allGuildsData.try_emplace(guildId, GuildData {});
    GuildData& guildData                = guildDataPair->second;

    dpp::snowflake userId             = event.command.get_issuing_user().id;
    auto [userDataPair, userInserted] = guildData.allUsersData.try_emplace(userId, UserData {});
    UserData& userData                = userDataPair->second;

    // @Settings.
    constexpr int AMOUNT_TO_ADD  = 10000;
    userData.coinsInCents       += AMOUNT_TO_ADD;

    // @Settings.
    event.reply(format_to_string("You now have **%f** more coins in your wallet!", userData.coinsInCents / 100.0f));
}

static void handle_wallet(UmikoBot& bot, const dpp::slashcommand_t& event) {
    // @Cleanup: We'll probably be copying this info getting thing a lot...
    dpp::snowflake guildId              = event.command.get_guild().id;
    auto [guildDataPair, guildInserted] = bot.allGuildsData.try_emplace(guildId, GuildData {});
    GuildData& guildData                = guildDataPair->second;

    dpp::snowflake userId             = event.command.get_issuing_user().id;
    auto [userDataPair, userInserted] = guildData.allUsersData.try_emplace(userId, UserData {});
    UserData& userData                = userDataPair->second;

    // @Settings.
    event.reply(format_to_string("You have **%f** coins.", userData.coinsInCents / 100.0f));
}

void register_currency_commands(UmikoBot& bot) {
    bot.register_command({ .name = "daily", .description = "Collect your daily coins!", .callback = handle_daily });
    bot.register_command({ .name = "wallet", .description = "Check your net worth!", .callback = handle_wallet });
}
