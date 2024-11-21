#include "QuestPopup.hpp"
#include "../utils/BetterQuests.hpp"
#include "Geode/binding/LoadingCircle.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/ui/Layout.hpp"
#include "QuestNode.hpp"

bool QuestPopup::setup() {

  setID("quest-popup"_spr);
  m_mainLayer->setID("main-layer");

  auto tlCorner =
      CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
  tlCorner->setFlipY(true);
  tlCorner->setID("top-left-corner");
  m_mainLayer->addChildAtPosition(tlCorner, Anchor::TopLeft,
                                  {tlCorner->getContentSize().width / 2,
                                   -tlCorner->getContentSize().height / 2});

  auto trCorner =
      CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
  trCorner->setFlipX(true);
  trCorner->setFlipY(true);
  trCorner->setID("top-right-corner");
  m_mainLayer->addChildAtPosition(trCorner, Anchor::TopRight,
                                  {-trCorner->getContentSize().width / 2,
                                   -trCorner->getContentSize().height / 2});

  auto blCorner =
      CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
  blCorner->setID("bottom-left-corner");
  m_mainLayer->addChildAtPosition(blCorner, Anchor::BottomLeft,
                                  {blCorner->getContentSize().width / 2,
                                   blCorner->getContentSize().height / 2});

  auto brCorner =
      CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
  brCorner->setFlipX(true);
  brCorner->setID("bottom-right-corner");
  m_mainLayer->addChildAtPosition(brCorner, Anchor::BottomRight,
                                  {-brCorner->getContentSize().width / 2,
                                   brCorner->getContentSize().height / 2});

  auto quest = Quest{0,
                     "The Easiest Quest In The World",
                     "Beat Tidal Wave",
                     0,
                     "Easy",
                     "BeatLevel",
                     {}};

  auto questMenu = CCMenu::create();
  questMenu->setID("quest-menu");
  questMenu->setLayout(AxisLayout::create(Axis::Column));
  m_mainLayer->addChildAtPosition(questMenu, Anchor::Center, {0.f, 0.f});

  auto now = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch()).count();

  if (BetterQuests::get()->quests.size() > 0 && BetterQuests::get()->resetsAt > now) {
    for (auto quest : BetterQuests::get()->quests) {
      auto node = QuestNode::create(quest, {360.f, 75.f});
      node->setID(fmt::format("quest-node-{}", quest.id));
      questMenu->addChildAtPosition(node, Anchor::Center, {0.f, 0.f});
    }
    questMenu->updateLayout();

    
    // int h = (BetterQuests::get()->resetsAt - now) / 3600;
    // int m = ((BetterQuests::get()->resetsAt - now) % 3600) / 60;
    // int s = ((BetterQuests::get()->resetsAt - now) % 3600) % 60;

    m_timerLabel = CCLabelBMFont::create(fmt::format("New quests in {:02}:{:02}:{:02}", 0, 0, 0).c_str(), "goldFont.fnt");
    m_timerLabel->setID("timer-label");
    m_timerLabel->setScale(0.5f);
    m_mainLayer->addChildAtPosition(m_timerLabel, Anchor::Bottom, {0.f, 12.5f});
    QuestPopup::updateTimer(0);
    schedule(schedule_selector(QuestPopup::updateTimer), 1.f);

    return true;
  }

  auto loadingCircle = LoadingCircle::create();
  loadingCircle->setID("loading-circle");
  loadingCircle->setParentLayer(this);
  loadingCircle->setPosition({0, 0});
  loadingCircle->setScale(1.f);
  loadingCircle->show();

  m_listener.bind([this, questMenu, loadingCircle](web::WebTask::Event *e) {
    if (web::WebResponse *res = e->getValue()) {
      log::info("{}", res->string().unwrapOrDefault());
      auto quests = res->json().unwrapOrDefault()["quests"].as<std::vector<Quest>>().unwrapOrDefault();
      BetterQuests::get()->quests = quests;
      BetterQuests::get()->resetsAt = res->json().unwrapOrDefault()["next_reset"].as<int>().unwrapOrDefault();
      for (auto quest : quests) {
        auto node = QuestNode::create(quest, {360.f, 75.f});
        node->setID(fmt::format("quest-node-{}", quest.id));
        questMenu->addChildAtPosition(node, Anchor::Center, {0.f, 0.f});
      }
      questMenu->updateLayout();
      
      m_timerLabel = CCLabelBMFont::create(fmt::format("New quests in {:02}:{:02}:{:02}", 0, 0, 0).c_str(), "goldFont.fnt");
      m_timerLabel->setID("timer-label");
      m_timerLabel->setScale(0.5f);
      m_mainLayer->addChildAtPosition(m_timerLabel, Anchor::Bottom, {0.f, 12.5f});
      QuestPopup::updateTimer(0);
      schedule(schedule_selector(QuestPopup::updateTimer), 1.f);

      loadingCircle->fadeAndRemove();
    }
  });

  auto req = web::WebRequest();
  m_listener.setFilter(req.get(fmt::format("{}/enduser/getquests?type={}", BetterQuests::get()->getServerUrl(), difficulty)));

  return true;
}

void QuestPopup::updateTimer(float) {
  if (m_timerLabel == nullptr) return;
  auto now = std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch()).count();
  int h = (BetterQuests::get()->resetsAt - now) / 3600;
  int m = ((BetterQuests::get()->resetsAt - now) % 3600) / 60;
  int s = ((BetterQuests::get()->resetsAt - now) % 3600) % 60;
  m_timerLabel->setString(fmt::format("New quests in {:02}:{:02}:{:02}", h, m, s).c_str());
}

QuestPopup *QuestPopup::create() {
  auto ret = new QuestPopup();
  if (ret->initAnchored(420.f, 280.f)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}