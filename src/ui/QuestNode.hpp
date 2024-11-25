#pragma once

#include "../utils/BetterQuests.hpp"

using namespace geode::prelude;

class QuestNode : public CCNode {
    protected:
        Quest quest;
        bool init(Quest quest, CCSize size);
    public:
        static QuestNode* create(Quest quest, CCSize size);
        void onClaim(CCObject* Sender);
        void exit();
};