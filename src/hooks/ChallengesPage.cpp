#include "../ui/QuestPopup.hpp"

using namespace geode::prelude;

struct GetServerEvent : geode::Event {
    GetServerEvent() {}

    std::string m_url = "";
};

std::string getServer() {
    auto event = GetServerEvent();
    event.post();
    return event.m_url;
}

#include <Geode/modify/ChallengesPage.hpp>
class $modify(myChP, ChallengesPage) {
    bool init() {
        if (!ChallengesPage::init()) {
            return false;
        }

        auto sprite = CircleButtonSprite::createWithSpriteFrameName("scroll.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Medium);
        auto myButton = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(myChP::onQuestsPlus)
        );
        sprite->setScale(0.8);

        auto menu = this->getChildByIDRecursive("main-menu");
        menu->addChild(myButton);
        myButton->setPosition({386,-247});

        myButton->setID("more-quests"_spr);

        return true;
    }

    void onQuestsPlus(CCObject*) {
        QuestPopup::create()->show();
    }
};