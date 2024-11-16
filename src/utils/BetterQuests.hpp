#pragma once

class BetterQuests {
    private:
        static BetterQuests* instance;
        std::string serverUrl = "http://localhost:8787/"; // https://betterquests.km7dev.tech/
        BetterQuests() {};
    public:
        static BetterQuests* get();
        std::string getServerUrl();
};

struct Quest {
    int id;
    std::string name;
    std::string description;
    int reward;
    std::string difficulty;
    std::string type;
    std::unordered_map<std::string,std::string> specifications;
};