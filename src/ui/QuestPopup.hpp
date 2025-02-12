#pragma once

#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class QuestPopup : public geode::Popup<> {
protected:
    bool setup() override;
    std::string difficulty = "easy";
    CCLabelBMFont *m_timerLabel = nullptr;
    void updateTimer(float);
    bool runTimer = false;
    CCMenu *m_questMenu;
    CCMenuItemSpriteExtra *m_leftButton;
    CCMenuItemSpriteExtra *m_rightButton;
    CCMenuItemSpriteExtra *m_reloadBtn;
    CCLabelBMFont *m_titleLabel = nullptr;
    bool loading = false;
    std::vector<std::string> diffs = std::vector<std::string>{"easy", "medium", "hard", "expert", "extreme", "master"};
public:
    CCLabelBMFont *m_scrollLabel = nullptr;
    static QuestPopup* create();
    void loadQuests();
    void onLeft(CCObject *);
    void onRight(CCObject *);
    void onReload(CCObject *);
};
