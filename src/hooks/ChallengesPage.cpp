using namespace geode::prelude;

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
        FLAlertLayer::create("BetterQuests", "BetterQuests is no longer in support and no longer functions. You can just uninstall the mod.", "Ok")->show();
    }
};
