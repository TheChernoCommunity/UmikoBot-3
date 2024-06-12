#pragma once

#include <dpp/dpp.h>

class UmikoBot;

void register_general_commands(UmikoBot& bot);

void handle_ping(const UmikoBot& bot, const dpp::slashcommand_t& event);
