#pragma once

class BetterQuests {
    private:
        static BetterQuests* instance;
        BetterQuests() {};
    public:
        static BetterQuests* get();
};