#pragma once

using namespace geode::prelude;

struct Quest {
    int id;
    std::string name;
    std::string description;
    int reward;
    std::string difficulty;
    std::string type;
    std::unordered_map<std::string,std::string> specifications;
};

class BetterQuests {
    private:
        static BetterQuests* instance;
        std::string serverUrl = "http://localhost:8787"; // https://betterquests.km7dev.tech/
        BetterQuests() {};
    public:
        static BetterQuests* get();
        std::string getServerUrl();
        std::vector<Quest> quests;
};

template <>
struct matjson::Serialize<Quest>
{
    static Result<Quest> fromJson(matjson::Value const &value)
    {
        return Ok(Quest{
            .id = value["Id"].as<int>().unwrapOrDefault(),
            .name = value["Name"].as<std::string>().unwrapOrDefault(),
            .description = value["Description"].as<std::string>().unwrapOrDefault(),
            .reward = value["Reward"].as<int>().unwrapOrDefault(),
            .difficulty = value["Difficulty"].as<std::string>().unwrapOrDefault(),
            .type = value["Type"].as<std::string>().unwrapOrDefault(),
            .specifications = value["Specifications"].as<std::unordered_map<std::string,std::string>>().unwrapOrDefault()
        });
    }

    // doubt ill need this
    static matjson::Value toJson(Quest const &value)
    {
        auto obj = matjson::makeObject({{"id", value.id}, {"name", value.name}, {"description", value.description}, {"reward", value.reward}, {"difficulty", value.difficulty}, {"type", value.type}, {"specifications", value.specifications}});
        return obj;
    }
};