#include "QuestPopup.hpp"
#include "../utils/BetterQuests.hpp"
#include "QuestNode.hpp"

bool QuestPopup::setup() {

  setID("quest-popup"_spr);
  m_mainLayer->setID("main-layer");
  m_buttonMenu->setID("button-menu");
  if (auto backBtn = m_buttonMenu->getChildByType<CCMenuItemSpriteExtra>(0)) {
    backBtn->setID("back-button");
    backBtn->removeFromParent();
    backBtn->getChildByType<CCSprite>(0)->setScale(1.f);
    m_buttonMenu->addChildAtPosition(backBtn, Anchor::TopLeft, {10.f, -10.f});
  }

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

  auto scrollIcon = CCSprite::createWithSpriteFrameName("scroll.png"_spr);
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

  auto arrowMenu = CCMenu::create();
  arrowMenu->setID("arrow-menu");
  m_mainLayer->addChildAtPosition(arrowMenu, Anchor::Center, {0.f, 0.f});

  m_leftButton = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName("navArrowBtn_001.png"),
      this, (SEL_MenuHandler)&QuestPopup::onLeft);
  m_leftButton->setID("left-arrow");
  m_leftButton->getChildByType<CCSprite>(0)->setFlipX(true);  

  m_rightButton = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName("navArrowBtn_001.png"),
      this, (SEL_MenuHandler)&QuestPopup::onRight);
  m_rightButton->setID("right-arrow");

  arrowMenu->addChildAtPosition(m_leftButton, Anchor::Center, {-240.f, 0.f});
  arrowMenu->addChildAtPosition(m_rightButton, Anchor::Center, {240.f, 0.f});

  m_titleLabel = CCLabelBMFont::create(fmt::format("{} Quests", difficulty).c_str(), "goldFont.fnt");
  m_titleLabel->setScale(0.75f);
  m_titleLabel->setID("title-label");
  m_mainLayer->addChildAtPosition(m_titleLabel, Anchor::Top, {0.f, -15.f});

  m_reloadBtn = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
      this, menu_selector(QuestPopup::onReload));
  m_reloadBtn->setID("reload-button");
  m_reloadBtn->setVisible(false);
  m_buttonMenu->addChildAtPosition(m_reloadBtn, Anchor::BottomRight,{-15.f, 15.f});

  this->loadQuests();
  schedule(schedule_selector(QuestPopup::updateTimer), 1.f);
  return true;
}

void QuestPopup::loadQuests() {
  runTimer = false;
  if (loading)
    return;
  loading = true;
  m_titleLabel->setString(fmt::format("{} Quests", difficulty).c_str());
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  m_questMenu->removeAllChildren();

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
    runTimer = true;
    QuestPopup::updateTimer(0);
    loadingCircle->fadeAndRemove();
    loading = false;
    return;
  }

  m_listener.bind([this, loadingCircle](web::WebTask::Event *e) {
    if (web::WebResponse *res = e->getValue()) {
      if (!res->ok()) {
        loadingCircle->fadeAndRemove();
        loading = false;
        BetterQuests::get()->lastReload = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
        m_reloadBtn->setVisible(true);
        return;
      }
      if (res->json().unwrapOrDefault().contains("error")) {
        auto error = res->json().unwrapOrDefault()["error"];
        log::error("Error");
        loadingCircle->fadeAndRemove();
        loading = false;
        BetterQuests::get()->lastReload = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
        FLAlertLayer::create(fmt::format("Error occured, code {}.", error["code"].asDouble().unwrapOrDefault()).c_str(), fmt::format("An error occured while trying to load quests: {}", error["reason"].asString().unwrapOrDefault()).c_str(), "OK")->show();
        m_reloadBtn->setVisible(true);
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
      runTimer = true;
      QuestPopup::updateTimer(0);
      loadingCircle->fadeAndRemove();
      loading = false;
      BetterQuests::get()->lastReload = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    }
  });

  auto req = web::WebRequest();
  m_listener.setFilter(
      req.get(fmt::format("{}/enduser/getquests?version=1",
                          BetterQuests::get()->getServerUrl(), difficulty)));
}

void QuestPopup::updateTimer(float) {
  if (m_timerLabel == nullptr)
    return;
  if (!runTimer) return;
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

void QuestPopup::onLeft(CCObject *) {
  if (loading)
    return;
  auto it = std::find(diffs.begin(), diffs.end(), difficulty);
  if (it != diffs.begin()) {
    it--;
    difficulty = *it;
  } else {
    difficulty = diffs[diffs.size() - 1];
  }
  this->loadQuests();
}

void QuestPopup::onRight(CCObject *) {
  if (loading)
    return;
  auto it = std::find(diffs.begin(), diffs.end(), difficulty);
  if (it != diffs.end() - 1) {
    it++;
    difficulty = *it;
  } else {
    difficulty = diffs[0];
  }
  this->loadQuests();
}

void QuestPopup::onReload(CCObject *) {
  if (loading)
    return;
  auto now = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
  if (now - BetterQuests::get()->lastReload < 20) {
    return;
  }
  m_reloadBtn->setVisible(false);
  m_questMenu->removeAllChildren();
  auto loadingCircle = LoadingCircle::create();
  loadingCircle->setID("loading-circle");
  loadingCircle->setParentLayer(m_mainLayer);
  loadingCircle->setScale(1.f);
  loadingCircle->show();
  loadingCircle->setPosition((m_mainLayer->getContentSize() - loadingCircle->getContentSize()) / 2);
  loading = true;
  m_listener.bind([this, loadingCircle](web::WebTask::Event *e) {
    if (web::WebResponse *res = e->getValue()) {
      if (!res->ok()) {
        loadingCircle->fadeAndRemove();
        loading = false;
        BetterQuests::get()->lastReload = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
        return;
      }
      auto quests = res->json().unwrapOrDefault()["quests"].as<std::vector<Quest>>().unwrapOrDefault();
      if (BetterQuests::get()->resetsAt < res->json().unwrapOrDefault()["next_reset"].as<int>().unwrapOrDefault()) {
      } else {
        for (Quest &quest : quests) {
          auto it = std::find_if(BetterQuests::get()->quests.begin(), BetterQuests::get()->quests.end(), [&quest](Quest q) { return q.id == quest.id; });
          if (it != BetterQuests::get()->quests.end()) {
            quest.progress = it->progress;
          }
        }
      }
      BetterQuests::get()->quests = quests;
      BetterQuests::get()->completedQuests = std::vector<int>();
      BetterQuests::get()->resetsAt = res->json().unwrapOrDefault()["next_reset"].as<int>().unwrapOrDefault();
      Mod::get()->setSavedValue<int>("resetsAt", BetterQuests::get()->resetsAt);
      Mod::get()->setSavedValue<std::vector<Quest>>("quests", quests);
      Mod::get()->setSavedValue<std::vector<int>>("completedQuests", BetterQuests::get()->completedQuests);

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
        m_timerLabel = CCLabelBMFont::create(fmt::format("New quests in {:02}:{:02}:{:02}", 0, 0, 0).c_str(), "goldFont.fnt");
        m_timerLabel->setID("timer-label");
        m_timerLabel->setScale(0.5f);
        m_mainLayer->addChildAtPosition(m_timerLabel, Anchor::Bottom, {0.f, 12.5f});
      }
      QuestPopup::updateTimer(0);
      loadingCircle->fadeAndRemove();
      loading = false;
      BetterQuests::get()->lastReload = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    }
  });

  auto req = web::WebRequest();
  m_listener.setFilter(req.get(fmt::format("{}/enduser/getquests?version=1", BetterQuests::get()->getServerUrl(), difficulty)));
}

$on_mod(Loaded) {
  BetterQuests::get()->quests = Mod::get()->getSavedValue<std::vector<Quest>>("quests");
  BetterQuests::get()->resetsAt = Mod::get()->getSavedValue<int>("resetsAt");
  BetterQuests::get()->completedQuests = Mod::get()->getSavedValue<std::vector<int>>("completedQuests");
}
