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

        auto myButton = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName("scroll.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::SmallAlt),
            this,
            menu_selector(myChP::onQuestsPlus)
        );

        auto menu = this->getChildByIDRecursive("top-right-menu");
        menu->addChild(myButton);

        myButton->setID("more-quests"_spr);

        menu->updateLayout();

        return true;
    }

    void onQuestsPlus(CCObject*) {
        QuestPopup::create()->show();
    }
};