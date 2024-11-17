#pragma once

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class QuestPopup : public geode::Popup<> {
protected:
    bool setup() override;
    EventListener<web::WebTask> m_listener;
    std::string difficulty = "Easy";
public:
    static QuestPopup* create();
};
