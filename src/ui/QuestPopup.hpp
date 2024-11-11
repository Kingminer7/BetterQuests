#pragma once

using namespace geode::prelude;

class QuestPopup : public geode::Popup<> {
protected:
    bool setup() override {
        this->setTitle("Better Quests");

        return true;
    }

public:
    static QuestPopup* create() {
        auto ret = new QuestPopup();
        if (ret->initAnchored(420.f, 280.f)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
