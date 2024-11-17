#include "QuestNode.hpp"

bool QuestNode::init(Quest quest, CCSize size) {
    if (!CCNode::init()) {
        return false;
    }

    this->quest = quest;

    this->setContentSize(size);
    this->setAnchorPoint({0.5,0.5});

    auto bg = CCScale9Sprite::create("GJ_square02.png");
    bg->setContentSize(size);
    this->addChildAtPosition(bg, geode::Anchor::Center, {0.f, 0.f});

    auto title = CCLabelBMFont::create(quest.name.c_str(), "bigFont.fnt");
    title->setScale(0.65f);
    title->setAnchorPoint({0,0.5});
    title->limitLabelWidth(340, 0.65, 0.25);
    this->addChildAtPosition(title, geode::Anchor::TopLeft, {10.f, -12.f});

    auto desc = CCLabelBMFont::create(quest.description.c_str(), "goldFont.fnt");
    desc->setScale(0.575f);
    desc->setAnchorPoint({0,0.5});
    desc->limitLabelWidth(340, 0.575, 0.15);
    this->addChildAtPosition(desc, geode::Anchor::TopLeft, {10.f, -30.f});

    auto reward = CCLabelBMFont::create(fmt::format("{}x", quest.reward).c_str(), "bigFont.fnt");
    reward->setAnchorPoint({1.f, 0.5});
    reward->setScale(0.5f);
    this->addChildAtPosition(reward, geode::Anchor::Right, {-32.5f, 0.f});

    auto scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
    scrollIcon->setScale(1.5f);
    this->addChildAtPosition(scrollIcon, geode::Anchor::Right, {-20.f, 0});

    return true;
}

QuestNode* QuestNode::create(Quest quest, CCSize size) {
    auto ret = new QuestNode;
    if (ret && ret->init(quest, size))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}