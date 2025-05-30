#pragma once

#include "Geode/loader/Mod.hpp"
#include "Geode/utils/web.hpp"
using namespace geode::prelude;

struct Quest {
  int id;
  std::string name;
  std::string description;
  int reward;
  std::string difficulty;
  std::string type;
  int quantity;
  matjson::Value specifications;

  // stuff managed locally
  float progress;
};

class BetterQuests {
private:
  static BetterQuests *instance;
  std::string serverUrl =
      // "http://localhost:8787";
      "https://bq.km7dev.tech";
  // std::string authUrl = "http://localhost:3000/api/v1";
  BetterQuests() { scrolls = Mod::get()->getSavedValue<int>("scrolls"); };
  int scrolls = 0;

public:
  static BetterQuests *get();
  std::string getServerUrl();
  // std::string getAuthUrl();
  std::vector<Quest> quests;
  EventListener<web::WebTask> listener;
  std::vector<int> completedQuests;
  int lastReload = 0;
  int resetsAt = 0;

  int getScrolls();
  void addScrolls(int amount);

  void completeQuest(Quest quest) {
    scrolls += quest.reward;
    Mod::get()->setSavedValue<int>("scrolls", scrolls);
    completedQuests.push_back(quest.id);
    Mod ::get()->setSavedValue<std::vector<int>>("completedQuests",
                                                 completedQuests);
  }
};

template <> struct matjson::Serialize<Quest> {
  static Result<Quest> fromJson(matjson::Value const &value) {
    return Ok(Quest{
        .id = value["Id"].as<int>().unwrapOrDefault(),
        .name = value["Name"].as<std::string>().unwrapOrDefault(),
        .description = value["Description"].as<std::string>().unwrapOrDefault(),
        .reward = value["Reward"].as<int>().unwrapOrDefault(),
        .difficulty = value["Difficulty"].as<std::string>().unwrapOrDefault(),
        .type = value["Type"].as<std::string>().unwrapOrDefault(),
        .quantity = value["Quantity"].as<int>().unwrapOrDefault(),
        .specifications =
            matjson::parse(value["Specifications"].asString().unwrapOrDefault())
                .unwrapOrDefault(),
        .progress = value["Progress"].as<float>().unwrapOrDefault()});
  }

  static matjson::Value toJson(Quest const &value) {
    auto obj =
        matjson::makeObject({{"Id", value.id},
                             {"Name", value.name},
                             {"Description", value.description},
                             {"Reward", value.reward},
                             {"Difficulty", value.difficulty},
                             {"Type", value.type},
                             {"Quantity", value.quantity},
                             {"Specifications", value.specifications.dump()},
                             {"Progress", value.progress}});
    return obj;
  }
};