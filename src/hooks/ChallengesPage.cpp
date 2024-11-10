#include "../ui/QuestPopup.hpp"

using namespace geode::prelude;

#include <Geode/modify/ChallengesPage.hpp>
class $modify(myChP, ChallengesPage) {
    bool init() {
        if (!ChallengesPage::init()) {
            return false;
        }

        log::debug("Hello from my ChallengesPage::init hook! This layer has {} children.", this->getChildrenCount());

        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
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