#include "NotifNode.hpp"

bool NotifNode::init(Quest quest, CCSize size) {
  if (!CCNode::init()) {
    return false;
  }
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
  title->limitLabelWidth(270, 0.65, 0.25);
  title->setID("title-label");
  this->addChildAtPosition(title, Anchor::TopLeft, {10.f, -12.f});

  auto desc = CCLabelBMFont::create(quest.description.c_str(), "chatFont.fnt");
  desc->setScale(0.575f);
  desc->setAnchorPoint({0, 1});
  desc->limitLabelWidth(270, 0.575, 0.15);
  desc->setColor({248, 206, 116});
  desc->setID("description-label");
  this->addChildAtPosition(desc, Anchor::TopLeft, {10.f, -23.f});

  auto reward = CCLabelBMFont::create(fmt::format("{}x", quest.reward).c_str(),
                                      "bigFont.fnt");
  reward->setAnchorPoint({1.f, 0.5});
  reward->setScale(0.5f);
  reward->setID("reward-label");
  this->addChildAtPosition(reward, Anchor::Right, {-32.5f, -13.f});

  auto scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
  scrollIcon->setScale(1.5f);
  scrollIcon->setID("scroll-icon");
  this->addChildAtPosition(scrollIcon, Anchor::Right, {-20.f, -13});

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

  scheduleOnce(schedule_selector(NotifNode::playAnim), 4);
  return true;
}

NotifNode *NotifNode::create(Quest quest, CCSize size) {
  auto ret = new NotifNode;
  if (ret && ret->init(quest, size)) {
    ret->autorelease();
    return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

void NotifNode::playAnim(int) {

  FMODAudioEngine::sharedEngine()->playEffect("achievement_01.ogg");
  auto eIn = CCEaseIn::create(
      CCMoveTo::create(.3, {0, CCDirector::get()->getWinSize().height}), 1.2);
  auto del = CCDelayTime::create(3);
  auto eOut = CCEaseOut::create(
      CCMoveTo::create(.3, {-290.f, CCDirector::get()->getWinSize().height}),
      1.2);
  auto func =
      CCCallFunc::create(this, callfunc_selector(NotifNode::removeFromParent));
  auto func2 =
      CCCallFunc::create(this, callfunc_selector(NotifNode::release));
  auto seq = CCSequence::create(eIn, del, eOut, func, func2, 0);
  runAction(seq);
}