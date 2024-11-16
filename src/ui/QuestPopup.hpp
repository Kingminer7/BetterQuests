#pragma once

using namespace geode::prelude;

class QuestPopup : public geode::Popup<> {
protected:
    bool setup() override;

public:
    static QuestPopup* create();
};
