#pragma once

#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class QuestPopup : public geode::Popup<> {
protected:
    bool setup() override;
    EventListener<web::WebTask> m_listener;
    std::string difficulty = "easy";
    CCLabelBMFont *m_timerLabel = nullptr;
    void updateTimer(float);
    CCMenu *m_questMenu;
public:
    static QuestPopup* create();
    void loadQuests();
};
