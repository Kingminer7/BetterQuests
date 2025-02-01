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

$on_mod(Loaded) {
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  if (BetterQuests::get()->quests.size() > 0 &&
      BetterQuests::get()->resetsAt > now) {
    return;
  }
  BetterQuests::get()->listener.bind([](web::WebTask::Event *e) {
    log::info("balls");
    if (web::WebResponse *res = e->getValue()) {
      if (!res->ok()) {
        BetterQuests::get()->lastReload =
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        return;
      }
      if (res->json().unwrapOrDefault().contains("error")) {
        auto error = res->json().unwrapOrDefault()["error"];
        log::error("Error");
        BetterQuests::get()->lastReload =
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
        FLAlertLayer::create(
            fmt::format("Error occured, code {}.",
                        error["code"].asDouble().unwrapOrDefault())
                .c_str(),
            fmt::format("An error occured while trying to load quests: {}",
                        error["reason"].asString().unwrapOrDefault())
                .c_str(),
            "OK")
            ->show();
        return;
      }
      auto quests = res->json()
                        .unwrapOrDefault()["quests"]
                        .as<std::vector<Quest>>()
                        .unwrapOrDefault();
      BetterQuests::get()->completedQuests = std::vector<int>();
      BetterQuests::get()->quests = quests;
      BetterQuests::get()->resetsAt = res->json()
                                          .unwrapOrDefault()["next_reset"]
                                          .as<int>()
                                          .unwrapOrDefault();
      Mod::get()->setSavedValue<int>("resetsAt", BetterQuests::get()->resetsAt);
      Mod::get()->setSavedValue<std::vector<Quest>>("quests", quests);

      BetterQuests::get()->lastReload =
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count();
      log::info("Loaded quests");
    }
  });

  auto req = web::WebRequest();
  BetterQuests::get()->listener.setFilter(req.get(fmt::format(
      "{}/enduser/getquests?version=1", BetterQuests::get()->getServerUrl())));
}