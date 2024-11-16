#include "QuestPopup.hpp"
#include "QuestNode.hpp"
#include "../utils/BetterQuests.hpp"

bool QuestPopup::setup() {
    auto tlCorner = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    tlCorner->setFlipY(true);
    tlCorner->setID("top-left-corner");
    m_mainLayer->addChildAtPosition(tlCorner, geode::Anchor::TopLeft, {tlCorner->getContentSize().width / 2, -tlCorner->getContentSize().height / 2});

    auto trCorner = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    trCorner->setFlipX(true);
    trCorner->setFlipY(true);
    trCorner->setID("top-right-corner");
    m_mainLayer->addChildAtPosition(trCorner, geode::Anchor::TopRight, {-trCorner->getContentSize().width / 2, -trCorner->getContentSize().height / 2});

    auto blCorner = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    blCorner->setID("bottom-left-corner");
    m_mainLayer->addChildAtPosition(blCorner, geode::Anchor::BottomLeft, {blCorner->getContentSize().width / 2, blCorner->getContentSize().height / 2});

    auto brCorner = CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
    brCorner->setFlipX(true);
    brCorner->setID("bottom-right-corner");
    m_mainLayer->addChildAtPosition(brCorner, geode::Anchor::BottomRight, {-brCorner->getContentSize().width / 2, brCorner->getContentSize().height / 2});

    auto quest = Quest{0, "test", "test", 0, "test", "test", {}};

    auto node = QuestNode::create(quest);
    node->setID("quest-node");
    m_mainLayer->addChildAtPosition(node, geode::Anchor::Center, {0.f, 0.f});

    return true;
}

QuestPopup* QuestPopup::create() {
    auto ret = new QuestPopup();
    if (ret->initAnchored(420.f, 280.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}