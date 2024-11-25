using namespace geode::prelude;

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

class $modify(PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();
        auto diff = getLevelDifficulty(m_level);
        log::info("{}: {}, {}", m_level->m_levelName, diffToStr(diff), m_level->m_stars > 0);

        for (Quest& quest : BetterQuests::get()->quests) {
            log::info("{},{}", quest.name, quest.specifications["difficulty"].asString().unwrapOrDefault());
            if (quest.type == "BeatLevels" 
              && quest.specifications["difficulty"].asString().unwrapOrDefault() == diffToStr(diff) 
              && m_level->m_stars > 0) {
                log::info("Completed quest part: {}", quest.name);
                if (quest.progress < quest.quantity) {
                  log::info("Adding to total of {}", quest.name);
                    quest.progress++;
                    Mod::get()->setSavedValue("quests", BetterQuests::get()->quests);
                    log::info("{} {}/{}", quest.name, quest.progress, quest.quantity);
                }
            }
        }
    }
};