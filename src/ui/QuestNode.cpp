#include "QuestNode.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"

bool QuestNode::init(Quest quest, CCSize size) {
    if (!CCNode::init()) {
        return false;
    }

    this->quest = quest;

    this->setContentSize(size);
    this->setAnchorPoint({0.5,0.5});

    auto bg = CCScale9Sprite::create("GJ_square02.png");
    bg->setContentSize(size);
    this->addChildAtPosition(bg, Anchor::Center, {0.f, 0.f});

    auto title = CCLabelBMFont::create(quest.name.c_str(), "bigFont.fnt");
    title->setScale(0.65f);
    title->setAnchorPoint({0,0.5});
    title->limitLabelWidth(340, 0.65, 0.25);
    this->addChildAtPosition(title, Anchor::TopLeft, {10.f, -12.f});

    auto desc = CCLabelBMFont::create(quest.description.c_str(), "chatFont.fnt");
    desc->setScale(0.575f);
    desc->setAnchorPoint({0,0.5});
    desc->limitLabelWidth(280, 0.575, 0.15);
    desc->setColor({ 248, 206, 116 });
    this->addChildAtPosition(desc, Anchor::TopLeft, {10.f, -30.f});

    auto reward = CCLabelBMFont::create(fmt::format("{}x", quest.reward).c_str(), "bigFont.fnt");
    reward->setAnchorPoint({1.f, 0.5});
    reward->setScale(0.5f);
    this->addChildAtPosition(reward, Anchor::Right, {-32.5f, 0.f});

    auto scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
    scrollIcon->setScale(1.5f);
    this->addChildAtPosition(scrollIcon, Anchor::Right, {-20.f, 0});

    auto progBar = CCSprite::create("GJ_progressBar_001.png");
    progBar->setAnchorPoint({0,0.5});
    progBar->setScale(0.625f);
    progBar->setColor({0, 0, 0});
    progBar->setOpacity(122);
    this->addChildAtPosition(progBar, Anchor::BottomLeft, {10.f, 20.f});

    auto progBarFill = CCSprite::create("GJ_progressBar_001.png");
    progBarFill->setAnchorPoint({0,0.5});
    progBarFill->setScaleX(0.99f);
    progBarFill->setScaleY(0.78f);
    progBarFill->setTextureRect(CCRect{progBarFill->getTextureRect().origin,{progBarFill->getTextureRect().size.width * clamp( quest.progress / quest.quantity, 0, 1), progBarFill->getTextureRect().size.height} });
    progBarFill->setColor({0, 255, 0});
    progBar->addChildAtPosition(progBarFill, Anchor::Left, {1.7f, 0.f});

    auto progBarLabel = CCLabelBMFont::create(fmt::format("{}/{}", quest.progress, quest.quantity).c_str(), "bigFont.fnt");
    progBarLabel->setScale(0.5f);
    progBarLabel->setAnchorPoint({0.5,0.5});
    progBar->addChildAtPosition(progBarLabel, Anchor::Center, {0.f, 0.f});

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