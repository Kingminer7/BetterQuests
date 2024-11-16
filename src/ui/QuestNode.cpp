#include "QuestNode.hpp"

bool QuestNode::init(Quest quest) {
    if (!CCNode::init()) {
        return false;
    }

    this->quest = quest;

    auto bg = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    bg->setScaleX(2.f);
    bg->setScaleY(1.f);
    this->addChildAtPosition(bg, geode::Anchor::Center, {0.f, 0.f});

    auto title = CCLabelBMFont::create(quest.name.c_str(), "bigFont.fnt");
    title->setScale(0.5f);
    title->setColor({0, 0, 0});
    this->addChildAtPosition(title, geode::Anchor::Top, {0.f, -bg->getContentSize().height / 2});

    auto desc = CCLabelBMFont::create(quest.description.c_str(), "bigFont.fnt");
    desc->setScale(0.5f);
    desc->setColor({0, 0, 0});
    this->addChildAtPosition(desc, geode::Anchor::Center, {0.f, 0.f});

    auto reward = CCLabelBMFont::create(fmt::format("{}", quest.reward).c_str(), "bigFont.fnt");
    reward->setScale(0.5f);
    reward->setColor({0, 0, 0});
    this->addChildAtPosition(reward, geode::Anchor::Bottom, {0.f, bg->getContentSize().height / 2});

    return true;
}

QuestNode* QuestNode::create(Quest quest) {
    auto ret = new QuestNode;
    if (ret && ret->init(quest))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}