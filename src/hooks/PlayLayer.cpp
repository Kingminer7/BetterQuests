using namespace geode::prelude;

#include "../ui/NotifNode.hpp"
#include "../utils/BetterQuests.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include "../apis/eclipse.hpp"

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

static LevelDifficulty getLevelDifficulty(GJGameLevel* level) {
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

class $modify(BQPL, PlayLayer) {
    struct Fields
    {
        std::vector<EffectGameObject*> coins;
    };

    void addObject(GameObject* p0)
    {
        PlayLayer::addObject(p0);
        if (p0->m_objectType == GameObjectType::UserCoin || p0->m_objectType == GameObjectType::SecretCoin) // 142 
        {
           m_fields->coins.push_back(as<EffectGameObject*>(p0));
        }
    }

    int getCollectedCoins()
    {
        int k = 0;
        for (auto coin : m_fields->coins)
        {
            if (coin->getOpacity() == 0)
                k++;
        }
        return k;
    }

    std::vector<std::string> checkStandards(Quest quest) {
        std::vector<std::string> invalidities = {};
        if (m_level->m_stars <= 0) invalidities.push_back("unrated");
        if (quest.type == "BeatLevels") {
            if (quest.specifications["difficulty"].asString().unwrapOrDefault() != "") {
                if (quest.specifications["difficulty"].asString().unwrapOrDefault() != diffToStr(getLevelDifficulty(m_level))) {
                    if (quest.specifications["difficulty"].asString().unwrapOrDefault() == "Demon") {
                        if (diffToStr(getLevelDifficulty(m_level)).find("Demon") == std::string::npos) {
                            invalidities.push_back("difficulty");
                        }
                    } else {
                        invalidities.push_back("difficulty");
                    }
                }
            }
            if (quest.specifications["type"].asString().unwrapOrDefault() != "") {
                auto type = m_level->isPlatformer() ? "platformer" : "classic";
                if (quest.specifications["type"].asString().unwrapOrDefault() != type) {
                    invalidities.push_back("type");
                } else if (quest.specifications["type"].asString().unwrapOrDefault() == "platformer") {
                    if (quest.specifications["max_time"].asInt().unwrapOrDefault() != 0) {
                        if (quest.specifications["max_time"].asInt().unwrapOrDefault() < m_attemptTime) {
                            invalidities.push_back("maxTime");
                        }
                        
                    }
                }
            }
            if (quest.specifications["max_attempts"].asInt().unwrapOrDefault() != 0) {
                if (quest.specifications["max_attempts"].asInt().unwrapOrDefault() < m_attempts) {
                    invalidities.push_back("maxAttempts");
                }
            }

            // Thanks Doggo and Justin
            if (quest.specifications["coins"].asInt().unwrapOrDefault() != 0) {
                int coinsNeeded = quest.specifications["coins"].asInt().unwrapOrDefault();
                if (coinsNeeded > m_fields->coins.size()) {
                    coinsNeeded = m_fields->coins.size();
                }
                if (getCollectedCoins() < coinsNeeded) {
                    invalidities.push_back("coins");
                }
            }
        }
        else if (quest.type == "CompleteLevel") {
            if (quest.specifications["max_attempts"].asInt().unwrapOrDefault() != 0) {
                if (quest.specifications["max_attempts"].asInt().unwrapOrDefault() < m_attempts) {
                    invalidities.push_back("maxAttempts");
                }
            }
            if (quest.specifications["id"].asInt().unwrapOrDefault() != 0) {
                if (quest.specifications["id"].asInt().unwrapOrDefault() != m_level->m_levelID.value()) {
                    invalidities.push_back("id");
                }
            }
            if (m_level->isPlatformer()) {
                if (quest.specifications["max_time"].asInt().unwrapOrDefault() != 0) {
                    if (quest.specifications["max_time"].asInt().unwrapOrDefault() < m_attemptTime) {
                        invalidities.push_back("maxTime");
                    }
                }
            }

            // Thanks Doggo and Justin
            if (quest.specifications["coins"].asInt().unwrapOrDefault() != 0) {
                int coinsNeeded = quest.specifications["coins"].asInt().unwrapOrDefault();
                if (coinsNeeded > m_fields->coins.size()) {
                    coinsNeeded = m_fields->coins.size();
                }
                if (getCollectedCoins() < coinsNeeded) {
                    invalidities.push_back("coins");
                }
            }
        }
        else {
            invalidities.push_back("invalid");
        }
        return invalidities;
    }

    void levelComplete() {
        // funky safe mode crap
        if (m_isTestMode
            || (eclipse::config::get<bool>("trippedSafeMode", false)
                && eclipse::config::getInternal<bool>("global.autosafemode", false))
            || eclipse::config::getInternal<bool>("global.safemode", false)
        ) {
            PlayLayer::levelComplete();
            return;
        };
        PlayLayer::levelComplete();
        if (m_isTestMode) return;

        if (m_isPracticeMode) return;


        for (Quest& quest : BetterQuests::get()->quests) {
            std::vector<std::string> meetsStandards = checkStandards(quest);
            if (meetsStandards.size() <= 0) {
                if (quest.progress < quest.quantity) {
                    quest.progress++;
                    Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
                    if (quest.progress >= quest.quantity) {
                        auto node = NotifNode::create(quest, { 290.f, 70.f });
                        node->setID(fmt::format("notif-node-{}", quest.id));
                        node->setPosition({ -290.f, CCDirector::get()->getWinSize().height });
                        node->setAnchorPoint({ 0, 1 });
                        node->setZOrder(CCDirector::get()->getRunningScene()->getHighestChildZ());
                        node->setScale(0.725f);
                        CCDirector::get()->getRunningScene()->addChild(node);
                    }
                }
            } else {
                auto standardsStr = fmt::to_string(fmt::join(meetsStandards, ", "));
            }
        }
    }

  static void onModify(auto& self) {
      if (!self.setHookPriority("cocos2d::CCLabelBMFont::init", 1'000'000)) {
          geode::log::warn("Failed to set hook priority.");
      }
  }
};