#include "QuestNode.hpp"
#include "Geode/binding/FMODAudioEngine.hpp"
#include "Geode/cocos/actions/CCActionInstant.h"

bool QuestNode::init(QuestPopup *popup, Quest quest, CCSize size) {
  if (!CCNode::init()) {
    return false;
  }

  this->popup = popup;
  this->quest = quest;

  this->setContentSize(size);
  this->setAnchorPoint({0.5, 0.5});

  auto bg = CCScale9Sprite::create("GJ_square02.png");
  bg->setContentSize(size);
  bg->setID("background");
  this->addChildAtPosition(bg, Anchor::Center, {0.f, 0.f});

  auto title = CCLabelBMFont::create(quest.name.c_str(), "bigFont.fnt");
  title->setScale(0.65f);
  title->setAnchorPoint({0, 0.5});
  title->limitLabelWidth(280, 0.65, 0.25);
  title->setID("title-label");
  this->addChildAtPosition(title, Anchor::TopLeft, {10.f, -12.f});

  auto desc = CCLabelBMFont::create(quest.description.c_str(), "chatFont.fnt");
  desc->setScale(0.575f);
  desc->setAnchorPoint({0, 1});
  desc->limitLabelWidth(280, 0.575, 0.15);
  desc->setColor({248, 206, 116});
  desc->setID("description-label");
  this->addChildAtPosition(desc, Anchor::TopLeft, {10.f, -23.f});

  auto reward = CCLabelBMFont::create(fmt::format("{}x", quest.reward).c_str(),
                                      "bigFont.fnt");
  reward->setAnchorPoint({1.f, 0.5});
  reward->setScale(0.5f);
  reward->setID("reward-label");
  this->addChildAtPosition(reward, Anchor::Right, {-32.5f, 13.f});

  auto scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
  scrollIcon->setScale(1.5f);
  scrollIcon->setID("scroll-icon");
  this->addChildAtPosition(scrollIcon, Anchor::Right, {-20.f, 13});

  auto progBar = CCSprite::create("GJ_progressBar_001.png");
  progBar->setAnchorPoint({0, 0.5});
  progBar->setScale(0.625f);
  progBar->setColor({0, 0, 0});
  progBar->setOpacity(122);
  progBar->setID("progress-bar");
  this->addChildAtPosition(progBar, Anchor::BottomLeft, {10.f, 15.f});

  auto progBarFill = CCSprite::create("GJ_progressBar_001.png");
  progBarFill->setAnchorPoint({0, 0.5});
  progBarFill->setScaleX(0.99f);
  progBarFill->setScaleY(0.78f);
  progBarFill->setTextureRect(
      CCRect{progBarFill->getTextureRect().origin,
             {progBarFill->getTextureRect().size.width *
                  clamp(quest.progress / quest.quantity, 0, 1),
              progBarFill->getTextureRect().size.height}});
  progBarFill->setColor({0, 255, 0});
  progBarFill->setID("progress-fill");
  progBar->addChildAtPosition(progBarFill, Anchor::Left, {1.7f, 0.f});

  auto progBarLabel = CCLabelBMFont::create(
      fmt::format("{}/{}", quest.progress, quest.quantity).c_str(),
      "bigFont.fnt");
  progBarLabel->setScale(0.5f);
  progBarLabel->setAnchorPoint({0.5, 0.5});
  progBarLabel->setID("progress-label");
  progBar->addChildAtPosition(progBarLabel, Anchor::Center, {0.f, 0.f});

  auto claimMenu = CCMenu::create();
  claimMenu->setAnchorPoint({0, 0});
  claimMenu->setPosition({0, 0});
  claimMenu->setID("claim-menu");
  claimMenu->setContentSize(this->getContentSize());
  this->addChild(claimMenu);

  CCSprite *claimSpr;
  auto enabled = true;
  if (quest.progress / quest.quantity >= 1) {
    claimSpr = CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png");
  } else if (quest.type == "CompleteLevel") {
    claimSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
  } else {
    claimSpr = CCSpriteGrayscale::createWithSpriteFrameName("GJ_rewardBtn_001.png");
    enabled = false;
  }
  auto claimBtn = CCMenuItemSpriteExtra::create(
      claimSpr, this, menu_selector(QuestNode::onClaim));
  claimBtn->setScale(0.6f);
  claimBtn->m_baseScale = 0.6f;
  claimBtn->setEnabled(enabled);
  claimMenu->addChildAtPosition(claimBtn, Anchor::Right, {-20.f, -13.f});

  return true;
}

QuestNode *QuestNode::create(QuestPopup *popup, Quest quest, CCSize size) {
  auto ret = new QuestNode;
  if (ret && ret->init(popup, quest, size)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

void QuestNode::onClaim(CCObject *Sender) {
  if (quest.progress >= quest.quantity) {
    BetterQuests::get()->completeQuest(quest);
    popup->m_scrollLabel->setString(
        fmt::format("{} ", BetterQuests::get()->getScrolls()).c_str());
    FMODAudioEngine::sharedEngine()->playEffect("gold01.ogg");
    this->exit();
  } else if (quest.type == "CompleteLevel") {
    auto scene = LevelBrowserLayer::scene(GJSearchObject::create(SearchType::Search, fmt::format("{}", quest.specifications["id"].asInt().unwrapOrDefault())));
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, scene));
  }
}

void QuestNode::exit() {
  auto elast = CCEaseElasticIn::create(CCMoveTo::create(.3, {CCDirector::get()->getWinSize().width / 570 * -getContentWidth(), getPositionY()}), 1.2);
  auto func = CCCallFunc::create(this, callfunc_selector(QuestNode::removeFromParent));
  auto func2 = CCCallFunc::create(this, callfunc_selector(QuestNode::release));
  auto seq = CCSequence::create(elast, func, func2, 0);
  runAction(seq);
}
