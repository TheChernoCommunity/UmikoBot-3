#include "common.hpp"
#include "umiko_bot.hpp"

#include <string>

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
