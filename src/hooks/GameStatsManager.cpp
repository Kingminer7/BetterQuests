#include "Geode/binding/GameStatsManager.hpp"
using namespace geode::prelude;

#include "../ui/NotifNode.hpp"
#include "../utils/BetterQuests.hpp"
#include <Geode/modify/GameStatsManager.hpp>

class $modify(BQGSM, GameStatsManager) {

  std::vector<std::string> checkStandards(Quest quest) {
    std::vector<std::string> invalidities = {};
    if (quest.type == "DestroyIcons") {

    } else {
      invalidities.push_back("invalid");
    }
    return invalidities;
  }

  void incrementStat(const char *stat, int incr) {
    GameStatsManager::incrementStat(stat, incr);
    if (strcmp(stat, "9") == 0) {
      if (BetterQuests::get()->resetsAt <
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count())
        return;
      for (Quest &quest : BetterQuests::get()->quests) {
        std::vector<std::string> meetsStandards = checkStandards(quest);
        if (meetsStandards.size() <= 0) {
          if (quest.progress + incr <= quest.quantity) {
            quest.progress += incr;
            Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
            if (quest.progress >= quest.quantity) {
              auto node = NotifNode::create(quest, {290.f, 70.f}, true);
              node->setID(fmt::format("notif-node-{}", quest.id));
              node->setPosition(
                  {-290.f, CCDirector::get()->getWinSize().height});
              node->setAnchorPoint({0, 1});
              node->setZOrder(
                  CCDirector::get()->getRunningScene()->getHighestChildZ());
              node->setScale(0.725f);
              CCDirector::get()->getRunningScene()->addChild(node);
            }
          }
        }
      }
    }
  }

  static void onModify(auto &self) {
    if (!self.setHookPriority("cocos2d::CCLabelBMFont::init", 1'000'000)) {
      geode::log::warn("Failed to set hook priority.");
    }
  }
};