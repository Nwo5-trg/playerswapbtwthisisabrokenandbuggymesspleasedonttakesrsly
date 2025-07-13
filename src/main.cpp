#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "playerswap.hpp"
#include <dankmeme.globed2/include/general.hpp>
#include <dankmeme.globed2/include/player.hpp>
#include <dankmeme.globed2/include/net.hpp>
#include <Geode/utils/web.hpp>
#include "utils.hpp"

using namespace geode::prelude;

// preface: i am ashamed of this code
// preface 2: :3c

class $modify(Play, PlayLayer) {
    struct Fields {
        CCLabelBMFont* timeLabel;
        CCLabelBMFont* currentPlayerLabel;
        CCLabelBMFont* nextPlayerLabel;
        EventListener<web::WebTask> listener;
        int apiTrys = 0;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        if (!PlayerSwap::enabled) return true;
        if (!globed::isLoaded() || !globed::net::isConnected().unwrap()) {
            log::error("bad no good");
            return true;
        }

        PlayerSwap::timeBetweenSwaps = Mod::get()->getSettingValue<double>("loop-time");

        PlayerSwap::playerID = GJAccountManager::get()->m_accountID;

        PlayerSwap::p1 = nullptr; PlayerSwap::p2 = nullptr;
        PlayerSwap::p1 = m_player1; PlayerSwap::p2 = m_player2;

        setupInfoLabels(m_fields->timeLabel, m_fields->currentPlayerLabel, m_fields->nextPlayerLabel, this);

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
        if (m_fields->apiTrys++ > 25) {
            Notification::create("failed to get synced time 3:");
            return;
        }
        auto req = web::WebRequest();
        m_fields->listener.setFilter(req.get("http://worldtimeapi.org/api/timezone/America/New_York"));
    }

    void updateTime(float dt) {
        auto fields = m_fields.self();
        PlayerSwap::updateTime();

        updateInfoLabels(m_fields->timeLabel, m_fields->currentPlayerLabel, m_fields->nextPlayerLabel);

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
            
            auto player = PlayerSwap::p1;
            auto swapToPlayer = p1Objs.first;

            setPlayerMode(player, swapToPlayer, this);
            player->togglePlayerScale(fabsf(swapToPlayer->getScale()) < 1.0f, true); // dumb workaround but it works
            if (player && swapToPlayer) PlayerSwap::p1->m_position = p1Objs.first->getPosition();

            PlayerSwap::lockSwapUpdates = true;
        }
    }
};

class $modify(Pause, PauseLayer) {
    struct Fields {
        CCLabelBMFont* timeLabel;
        CCLabelBMFont* currentPlayerLabel;
        CCLabelBMFont* nextPlayerLabel;
    };

    void customSetup() {
        PauseLayer::customSetup();

        setupInfoLabels(m_fields->timeLabel, m_fields->currentPlayerLabel, m_fields->nextPlayerLabel, this);

        CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(Pause::updateTime), this, 0.0f, false);
    }

    void updateTime(float dt) {
        auto fields = m_fields.self();
        PlayerSwap::updateTime();

        updateInfoLabels(m_fields->timeLabel, m_fields->currentPlayerLabel, m_fields->nextPlayerLabel);

        if (!PlayerSwap::lockSwapUpdates && PlayerSwap::timeUntilSwap < 0.05l) {
            size_t nextIndex = (PlayerSwap::index + 1) % PlayerSwap::playerIDs.size();
            swapPlayers(PlayerSwap::currentPlayer, PlayerSwap::playerIDs[nextIndex]);
        }
    }

    void swapPlayers(int id1, int id2) {
        if (PlayerSwap::playerID == id2 && PlayerSwap::playerIDs.size() > 1) {
            onResume(nullptr);
            static_cast<Play*>(PlayLayer::get())->swapPlayers(id1, id2);
        }
    }
};