#include <Geode/Geode.hpp>
#include <dashauth.hpp>
#include "../utils/BetterQuests.hpp"

using namespace geode::prelude;
using namespace dashauth;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();

		return true;
	}

	void onMyButton(CCObject*) {
		 DashAuthRequest().getToken(Mod::get(), BetterQuests::get()->getAuthUrl())->except([](std::string err) {
                    FLAlertLayer::create("DashAuth Error", "Failed to get token, view logs for reason", "OK")->show();
                })->then([](std::string const& token) {
                    FLAlertLayer::create("DashAuth Success", "Token: " + token, "OK")->show();
                });
	}
};