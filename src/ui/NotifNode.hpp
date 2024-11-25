#pragma once

#include "../utils/BetterQuests.hpp"

using namespace geode::prelude;

class NotifNode : public CCNode {
    protected:
        Quest quest;
        bool init(Quest quest, CCSize size);
    public:
        static NotifNode* create(Quest quest, CCSize size);
        void playAnim(int);
};