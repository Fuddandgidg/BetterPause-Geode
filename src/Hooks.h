#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "BetterPauseManager.h"
#include "BetterPause.hpp"

using namespace geode::prelude;

// This fixes the Pause Menu for Windows 64-bit (2.2081)
class $modify(PauseLayer) {
    struct Fields {
        bool hasPosibleExitHotKey = false;
        bool hasConfirmPopup = false;
    };

    static void onModify(auto & self) {
        self.setHookPriority("PauseLayer::create", -99);
    }

    static PauseLayer* create(bool isEditor) {
        auto ret = PauseLayer::create(isEditor);
        if (!ret) return nullptr;

        if (Mod::get()->getSettingValue<int64_t>("type-pause")) {
            // 64-bit fix: Use IDs instead of counting nodes
            if (auto centerMenu = ret->getChildByID("center-menu")) {
                centerMenu->setVisible(false);
            }
            if (auto rightMenu = ret->getChildByID("right-button-menu")) {
                rightMenu->setVisible(false);
            }

            auto betterPauseMenu = BetterPause::create(ret);
            betterPauseMenu->setID("better-pause-node");
            ret->addChild(betterPauseMenu, 100);
        }

        return ret;
    }

    void onResume(cocos2d::CCObject * sender) {
        if (Mod::get()->getSettingValue<bool>("confirm-resume-level") && !m_fields->hasConfirmPopup) {
            geode::createQuickPopup(
                "Resume Level",
                "Are you sure you want to <cr>resume</c>?",
                "Cancel", "Resume",
                [this, sender](auto, bool btn2) {
                    if (btn2) {
                        m_fields->hasConfirmPopup = true;
                        PauseLayer::onResume(sender);
                        m_fields->hasConfirmPopup = false;
                    }
                }
            );
            return;
        }
        PauseLayer::onResume(sender);
    }
};

class $modify(PlayLayer) {
    // 64-bit fix: Removed the "offset" math that crashes the 64-bit compiler
    bool init(GJGameLevel * p0, bool p1, bool p2) {
        if (!PlayLayer::init(p0, p1, p2)) return false;
        return true;
    }
};
