#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "playerswap.hpp"
#include <dankmeme.globed2/include/general.hpp>
#include <dankmeme.globed2/include/player.hpp>
#include <dankmeme.globed2/include/net.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(Play, PlayLayer) {
    struct Fields {
        CCLabelBMFont* timeLabel;
        CCLabelBMFont* playerIDLabel;
        EventListener<web::WebTask> listener;
        int apiTrys = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        if (!PlayerSwap::enabled) return true;
        if (!globed::isLoaded() || !globed::net::isConnected().unwrap()) {
            Notification::create("player swap :3 : globed is not enabled 3:", NotificationIcon::Error);
            return true;
        }

        PlayerSwap::playerID = GJAccountManager::get()->m_accountID;

        PlayerSwap::p1 = nullptr; PlayerSwap::p2 = nullptr;
        PlayerSwap::p1 = m_player1; PlayerSwap::p2 = m_player2;

        auto winSize = CCDirector::get()->getWinSize();

        auto timeLabel = CCLabelBMFont::create("", "bigFont.fnt");
        timeLabel->setPosition(winSize);
        timeLabel->setAnchorPoint(ccp(1.0f, 1.0f));
        this->addChild(timeLabel);
        m_fields->timeLabel = timeLabel;

        auto playerIDLabel = CCLabelBMFont::create("", "bigFont.fnt");
        playerIDLabel->setScale(0.5f);
        playerIDLabel->setPosition(winSize.width, winSize.height - 40);
        playerIDLabel->setAnchorPoint(ccp(1.0f, 1.0f));
        this->addChild(playerIDLabel);
        m_fields->playerIDLabel = playerIDLabel;

        m_fields->listener.bind([this] (web::WebTask::Event* event) { // actual toothpicks vro
            if (web::WebResponse* res = event->getValue()) {
                if (res->ok()) {
                    auto json = res->json().unwrap();
                    PlayerSwap::syncedTime = json["unixtime"].asInt().unwrap();
                    PlayerSwap::syncedStart = std::chrono::steady_clock::now();
                    log::error("{}", PlayerSwap::syncedTime);
                    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(Play::updateTime), this, 0.0f, false);
                } else {
                    makeReq();
                }
            }
        });

        makeReq();

        return true;
    }

    void makeReq() {
        if (m_fields->apiTrys++ > 10) {
            Notification::create("failed to get synced time 3:");
            return;
        }
        auto req = web::WebRequest();
        m_fields->listener.setFilter(req.get("http://worldtimeapi.org/api/timezone/America/New_York"));
    }

    void updateTime(float dt) {
        auto fields = m_fields.self();
        PlayerSwap::updateTime();
        fields->timeLabel->setString(PlayerSwap::formatTime(PlayerSwap::timeUntilSwap).c_str());
        fields->playerIDLabel->setString((std::to_string(PlayerSwap::currentPlayer) + ", ID: " + std::to_string(PlayerSwap::playerID)).c_str());
        if (PlayerSwap::lockSwapUpdates && PlayerSwap::timeUntilSwap > 0.05l) PlayerSwap::lockSwapUpdates = false;
        if (!PlayerSwap::lockSwapUpdates && !m_isPaused && PlayerSwap::playerIDs.size() > 1 && PlayerSwap::playerID != PlayerSwap::currentPlayer) {
            pauseGame(true);
        }
    }

    void swapPlayers(int id1, int id2) {
        if (PlayerSwap::playerID == id2) {
            auto res = globed::player::getPlayerObjectsForId(id1);
            if (!res.ok()) return;
            auto p1Objs = res.unwrap();
            
            // for (int i = 0; i < 2; i++) {
                // auto player = i == 1 ? PlayerSwap::p2 : PlayerSwap::p1;
                // auto swapToPlayer = i == 1 ? p1Objs.second : p1Objs.first;
                auto player = PlayerSwap::p1;
                auto swapToPlayer = p1Objs.first;

                setPlayerMode(player, swapToPlayer);
                player->togglePlayerScale(fabsf(swapToPlayer->getScale()) < 1.0f, true); // dumb workaround but it works
                if (player && swapToPlayer) PlayerSwap::p1->m_position = p1Objs.first->getPosition();
                // player->m_playerSpeed = swapToPlayer->m_playerSpeed;
                // player->flipGravity(swapToPlayer->m_isUpsideDown, true);
                // player->doReversePlayer(swapToPlayer->m_isGoingLeft);

            // }
            PlayerSwap::lockSwapUpdates = true;
        }
    }

    void setPlayerMode(PlayerObject* p1, PlayerObject* swap) {
        int gamemode;
        if (swap->isInNormalMode()) gamemode = 0;
        else if (swap->m_isShip) gamemode = 1;
        else if (swap->m_isBall) gamemode = 2;
        else if (swap->m_isBird) gamemode = 3;
        else if (swap->m_isDart) gamemode = 4;
        else if (swap->m_isRobot) gamemode = 5;
        else if (swap->m_isSpider) gamemode = 6;
        else if (swap->m_isSwing) gamemode = 7;

        switch (gamemode) {
            case 0: {
                p1->toggleFlyMode(false, false);
                p1->toggleRollMode(false, false);
                p1->toggleBirdMode(false, false);
                p1->toggleDartMode(false, false);
                p1->toggleRobotMode(false, false);
                p1->toggleSpiderMode(false, false);
                p1->toggleSwingMode(false, false);
                break;
            }
            case 1: { p1->toggleFlyMode(true, true); break; }
            case 2: { p1->toggleRollMode(true, true); break; }
            case 3: { p1->toggleBirdMode(true, true); break; }
            case 4: { p1->toggleDartMode(true, true); break; }
            case 5: { p1->toggleRobotMode(true, true); break; }
            case 6: { p1->toggleSpiderMode(true, true); break; }
            case 7: { p1->toggleSwingMode(true, true); break; }
        }

        auto obj = TeleportPortalObject::create("edit_eGameRotBtn_001.png", true);
        obj->m_cameraIsFreeMode = true;
        this->playerWillSwitchMode(p1, obj);
    }
};

class $modify(Pause, PauseLayer) {
    struct Fields {
        CCLabelBMFont* timeLabel;
        CCLabelBMFont* playerIDLabel;
    };

    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::get()->getWinSize();

        auto timeLabel = CCLabelBMFont::create("", "bigFont.fnt");
        timeLabel->setPosition(winSize);
        timeLabel->setAnchorPoint(ccp(1.0f, 1.0f));
        this->addChild(timeLabel);
        m_fields->timeLabel = timeLabel;

        auto playerIDLabel = CCLabelBMFont::create("", "bigFont.fnt");
        playerIDLabel->setScale(0.5f);
        playerIDLabel->setPosition(winSize.width, winSize.height - 40);
        playerIDLabel->setAnchorPoint(ccp(1.0f, 1.0f));
        this->addChild(playerIDLabel);
        m_fields->playerIDLabel = playerIDLabel;

        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(Pause::updateTime), this, 0.0f, false);
    }

    void updateTime(float dt) {
        auto fields = m_fields.self();
        PlayerSwap::updateTime();
        fields->timeLabel->setString(PlayerSwap::formatTime(PlayerSwap::timeUntilSwap).c_str());
        fields->playerIDLabel->setString((std::to_string(PlayerSwap::currentPlayer) + ", ID: " + std::to_string(PlayerSwap::playerID)).c_str());
        if (!PlayerSwap::lockSwapUpdates && PlayerSwap::timeUntilSwap < 0.05l) {
            size_t nextIndex = (PlayerSwap::index + 1) % PlayerSwap::playerIDs.size();
            swapPlayers(PlayerSwap::currentPlayer, PlayerSwap::playerIDs[nextIndex]);
        }
    }

    void swapPlayers(int id1, int id2) {
        if (PlayerSwap::playerID == id2) {
            onResume(nullptr);
            static_cast<Play*>(PlayLayer::get())->swapPlayers(id1, id2);
        }
    }
};