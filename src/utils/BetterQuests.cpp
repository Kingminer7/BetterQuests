#include "BetterQuests.hpp"

BetterQuests* BetterQuests::instance = nullptr;

BetterQuests* BetterQuests::get() {
    if (instance == nullptr) {
        instance = new BetterQuests();
    }
    return instance;
}