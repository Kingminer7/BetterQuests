#include "BetterQuests.hpp"

BetterQuests* BetterQuests::instance = nullptr;

BetterQuests* BetterQuests::get() {
    if (instance == nullptr) {
        instance = new BetterQuests();
    }
    return instance;
}

std::string BetterQuests::getServerUrl() {
    return this->serverUrl;
}

// std::string BetterQuests::getAuthUrl() {
//     return this->authUrl;
// }

int BetterQuests::getScrolls() {
    return this->scrolls;
}

void BetterQuests::addScrolls(int amount) {
    this->scrolls += amount;
    Mod::get()->setSavedValue<int>("scrolls", scrolls);
}