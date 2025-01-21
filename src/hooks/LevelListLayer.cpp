using namespace geode::prelude;

#include "../ui/NotifNode.hpp"
#include "../utils/BetterQuests.hpp"
#include <Geode/modify/LevelListLayer.hpp>

class $modify(BQLLL, LevelListLayer) {

  bool checkStandards(Quest quest) {
    std::vector<std::string> invalidities;
    if (m_levelList->m_diamonds <= 0)
      invalidities.push_back("unrated");
    if (quest.type == "BeatLists") {

    }
    // disabled because players may not be able to do it if list completed
    // already else if (quest.type == "CompleteList") {
    //     if (quest.specifications["id"].asInt().unwrapOrDefault() != 0) {
    //         if (quest.specifications["id"].asInt().unwrapOrDefault() !=
    //         m_levelList->m_listID) {
    //             invalidities.push_back("id");
    //         }
    //     }

    // }
    else {
      invalidities.push_back("invalid");
    }
    return invalidities.size() < 1;
  }

  void onClaimReward(CCObject *obj) {
    LevelListLayer::onClaimReward(obj);
    if (BetterQuests::get()->resetsAt <
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count())
      return;
    for (Quest &quest : BetterQuests::get()->quests) {
      if (checkStandards(quest)) {
        if (quest.progress < quest.quantity) {
          quest.progress++;
          Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
          if (quest.progress >= quest.quantity) {
            auto node = NotifNode::create(quest, {290.f, 70.f}, true);
            node->setID(fmt::format("notif-node-{}", quest.id));
            node->setPosition({-290.f, CCDirector::get()->getWinSize().height});
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

  static void onModify(auto &self) {
    if (!self.setHookPriority("cocos2d::CCLabelBMFont::init", 1'000'000)) {
      geode::log::warn("Failed to set hook priority.");
    }
  }
};