#pragma once

#include "../utils/BetterQuests.hpp"

using namespace geode::prelude;

class QuestNode : public CCNode {
    protected:
        Quest quest;
        bool init(Quest quest);
    public:
        static QuestNode* create(Quest quest);
};