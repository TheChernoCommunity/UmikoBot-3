#pragma once

#include <dpp/dpp.h>

class UmikoBot;

// @Cleanup: Do we really need this header file at all?
// The register_general_commands() function is used in one place only, it could be forward-declared
// there. The handle_*() functions are only used in the general.cpp file, so not needed elsewhere.

void register_general_commands(UmikoBot& bot);

void handle_ping(const UmikoBot& bot, const dpp::slashcommand_t& event);
