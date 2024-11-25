#pragma once

#include "../utils/BetterQuests.hpp"
#include "QuestPopup.hpp"

using namespace geode::prelude;

class QuestNode : public CCNode {
    protected:
        Quest quest;
        bool init(QuestPopup *popup, Quest quest, CCSize size);
    public:
        QuestPopup *popup;
        static QuestNode* create(QuestPopup *popup, Quest quest, CCSize size);
        void onClaim(CCObject* Sender);
        void exit();
};