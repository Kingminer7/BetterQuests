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

  m_questMenu = CCMenu::create();
  m_questMenu->setID("quest-menu");
  m_questMenu->setLayout(AxisLayout::create(Axis::Column));
  m_mainLayer->addChildAtPosition(m_questMenu, Anchor::Center, {0.f, 0.f});

  CCSprite *scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
  scrollIcon->setScale(1.5f);
  scrollIcon->setID("scroll-icon");
  m_mainLayer->addChildAtPosition(scrollIcon, Anchor::TopRight, {-15.f, -15.f});

  m_scrollLabel = CCLabelBMFont::create(
      fmt::format("{} ", BetterQuests::get()->getScrolls()).c_str(),
      "bigFont.fnt");
  m_scrollLabel->setScale(0.5f);
  m_scrollLabel->setAnchorPoint({1, 0.5});
  m_scrollLabel->setID("scroll-label");
  m_mainLayer->addChildAtPosition(m_scrollLabel, Anchor::TopRight,
                                  {-25.f, -15.f});

  this->loadQuests();

  return true;
}

void QuestPopup::loadQuests() {
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  for (CCNode *node : CCArrayExt<CCNode *>(m_questMenu->getChildren())) {
    delete node;
  }

  auto loadingCircle = LoadingCircle::create();
  loadingCircle->setID("loading-circle");
  loadingCircle->setParentLayer(m_mainLayer);
  loadingCircle->setScale(1.f);
  loadingCircle->show();
  loadingCircle->setPosition((m_mainLayer->getContentSize() - loadingCircle->getContentSize()) / 2);

  if (BetterQuests::get()->quests.size() > 0 &&
      BetterQuests::get()->resetsAt > now) {

    int id = 0;
    for (auto quest : BetterQuests::get()->quests) {
      if (quest.difficulty == difficulty && std::find(BetterQuests::get()->completedQuests.begin(), BetterQuests::get()->completedQuests.end(), quest.id) == BetterQuests::get()->completedQuests.end()) {
        id++;
        auto node = QuestNode::create(this, quest, {360.f, 70.f});
        node->setID(fmt::format("quest-node-{}", id));
        m_questMenu->addChildAtPosition(node, Anchor::Center, {0.f, 0.f});
      }
    }
    m_questMenu->updateLayout();

    if (!m_timerLabel) {
      m_timerLabel = CCLabelBMFont::create(
          fmt::format("New quests in {:02}:{:02}:{:02}", 0, 0, 0).c_str(),
          "goldFont.fnt");
      m_timerLabel->setID("timer-label");
      m_timerLabel->setScale(0.5f);
      m_mainLayer->addChildAtPosition(m_timerLabel, Anchor::Bottom,
                                      {0.f, 12.5f});
    }

    QuestPopup::updateTimer(0);
    schedule(schedule_selector(QuestPopup::updateTimer), 1.f);
    loadingCircle->fadeAndRemove();
    return;
  }

  m_listener.bind([this, loadingCircle](web::WebTask::Event *e) {
    if (web::WebResponse *res = e->getValue()) {
      log::info(res->string());
      if (!res->ok()) {
        loadingCircle->fadeAndRemove();
        return;
      }
      auto quests = res->json()
                        .unwrapOrDefault()["quests"]
                        .as<std::vector<Quest>>()
                        .unwrapOrDefault();
      BetterQuests::get()->completedQuests = std::vector<int>();
      BetterQuests::get()->quests = quests;
      BetterQuests::get()->resetsAt = res->json()
                                          .unwrapOrDefault()["next_reset"]
                                          .as<int>()
                                          .unwrapOrDefault();
      Mod::get()->setSavedValue<int>("resetsAt", BetterQuests::get()->resetsAt);
      Mod::get()->setSavedValue<std::vector<Quest>>("quests", quests);

      int id = 0;
      for (auto quest : BetterQuests::get()->quests) {
        if (quest.difficulty == difficulty && std::find(BetterQuests::get()->completedQuests.begin(), BetterQuests::get()->completedQuests.end(), quest.id) == BetterQuests::get()->completedQuests.end()) {
          id++;
          auto node = QuestNode::create(this, quest, {360.f, 70.f});
          node->setID(fmt::format("quest-node-{}", id));
          m_questMenu->addChildAtPosition(node, Anchor::Center, {0.f, 0.f});
        }
      }
      m_questMenu->updateLayout();

      if (!m_timerLabel) {
        m_timerLabel = CCLabelBMFont::create(
            fmt::format("New quests in {:02}:{:02}:{:02}", 0, 0, 0).c_str(),
            "goldFont.fnt");
        m_timerLabel->setID("timer-label");
        m_timerLabel->setScale(0.5f);
        m_mainLayer->addChildAtPosition(m_timerLabel, Anchor::Bottom,
                                        {0.f, 12.5f});
      }
      QuestPopup::updateTimer(0);
      schedule(schedule_selector(QuestPopup::updateTimer), 1.f);
      loadingCircle->fadeAndRemove();
    }
  });

  auto req = web::WebRequest();
  m_listener.setFilter(
      req.get(fmt::format("{}/enduser/getquests",
                          BetterQuests::get()->getServerUrl(), difficulty)));
}

void QuestPopup::updateTimer(float) {
  if (m_timerLabel == nullptr)
    return;
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  if (now > BetterQuests::get()->resetsAt) {
    this->loadQuests();
  }
  int h = clamp((BetterQuests::get()->resetsAt - now) / 3600, 0, INT_MAX);
  int m = clamp(((BetterQuests::get()->resetsAt - now) % 3600) / 60, 0, 59);
  int s = clamp(((BetterQuests::get()->resetsAt - now) % 3600) % 60, 0, 59);
  m_timerLabel->setString(
      fmt::format("New quests in {:02}:{:02}:{:02}", h, m, s).c_str());
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
