#include "Geode/cocos/cocoa/CCGeometry.h"
using namespace geode::prelude;

#include "../ui/NotifNode.hpp"
#include "../utils/BetterQuests.hpp"
#include <Geode/modify/PlayLayer.hpp>

// Thank you prevter
enum class LevelDifficulty {
  NA,
  Auto,
  Easy,
  Normal,
  Hard,
  Harder,
  Insane,
  EasyDemon,
  MediumDemon,
  HardDemon,
  InsaneDemon,
  ExtremeDemon
};

static LevelDifficulty getLevelDifficulty(GJGameLevel *level) {
  if (level->m_autoLevel)
    return LevelDifficulty::Auto;
  auto diff = level->m_difficulty;

  if (level->m_ratingsSum != 0)
    diff = static_cast<GJDifficulty>(level->m_ratingsSum / 10);

  if (level->m_demon > 0) {
    switch (level->m_demonDifficulty) {
    case 3:
      return LevelDifficulty::EasyDemon;
    case 4:
      return LevelDifficulty::MediumDemon;
    case 5:
      return LevelDifficulty::InsaneDemon;
    case 6:
      return LevelDifficulty::ExtremeDemon;
    default:
      return LevelDifficulty::HardDemon;
    }
  }

  switch (diff) {
  case GJDifficulty::Easy:
    return LevelDifficulty::Easy;
  case GJDifficulty::Normal:
    return LevelDifficulty::Normal;
  case GJDifficulty::Hard:
    return LevelDifficulty::Hard;
  case GJDifficulty::Harder:
    return LevelDifficulty::Harder;
  case GJDifficulty::Insane:
    return LevelDifficulty::Insane;
  case GJDifficulty::Demon:
    return LevelDifficulty::HardDemon;
  default:
    return LevelDifficulty::NA;
  }
}

std::string diffToStr(LevelDifficulty diff) {
  switch (diff) {
  case LevelDifficulty::NA:
    return "NA";
  case LevelDifficulty::Auto:
    return "Auto";
  case LevelDifficulty::Easy:
    return "Easy";
  case LevelDifficulty::Normal:
    return "Normal";
  case LevelDifficulty::Hard:
    return "Hard";
  case LevelDifficulty::Harder:
    return "Harder";
  case LevelDifficulty::Insane:
    return "Insane";
  case LevelDifficulty::EasyDemon:
    return "EasyDemon";
  case LevelDifficulty::MediumDemon:
    return "MediumDemon";
  case LevelDifficulty::HardDemon:
    return "HardDemon";
  case LevelDifficulty::InsaneDemon:
    return "InsaneDemon";
  case LevelDifficulty::ExtremeDemon:
    return "ExtremeDemon";
  default:
    return "Unknown";
  }
}

class $modify(PlayLayer) {
  void levelComplete() {
    PlayLayer::levelComplete();
    auto diff = getLevelDifficulty(m_level);
    for (Quest &quest : BetterQuests::get()->quests) {
      if (quest.type == "BeatLevels" &&
          quest.specifications["difficulty"].asString().unwrapOrDefault() ==
              diffToStr(diff) &&
          m_level->m_stars > 0) {
        if (quest.progress < quest.quantity) {
          quest.progress++;
          Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
          if (quest.progress >= quest.quantity) {
            auto node = NotifNode::create(quest, {290.f, 70.f});
            node->setID(fmt::format("notif-node-{}", quest.id));
            node->setPosition({-290.f, CCDirector::get()->getWinSize().height});
            node->setAnchorPoint({0, 1});
            node->setZOrder(CCDirector::get()->getRunningScene()->getHighestChildZ());
            node->setScale(0.725f);
            CCDirector::get()->getRunningScene()->addChild(node);
          }
        }
      }
      if (quest.type == "CompleteLevel" &&
          quest.specifications["id"].asInt().unwrapOrDefault() ==
              m_level->m_levelID.value()) {
        if (quest.progress < quest.quantity) {
          quest.progress++;
          Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
          if (quest.progress >= quest.quantity) {
            auto node = NotifNode::create(quest, {290.f, 70.f});
            node->setID(fmt::format("notif-node-{}", quest.id));
            node->setPosition({-290.f, CCDirector::get()->getWinSize().height});
            node->setAnchorPoint({0, 1});
            node->setZOrder(CCDirector::get()->getRunningScene()->getHighestChildZ());
            node->setScale(0.725f);
            CCDirector::get()->getRunningScene()->addChild(node);
          }
        }
      }
    }
  }
};