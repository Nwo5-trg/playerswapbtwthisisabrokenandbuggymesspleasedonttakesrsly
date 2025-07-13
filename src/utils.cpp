#include "playerswap.hpp"
#include "utils.hpp"

using namespace geode::prelude;

void setPlayerMode(PlayerObject* p1, PlayerObject* swap, PlayLayer* playLayer) {
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
    playLayer->playerWillSwitchMode(p1, obj);
}

void updateInfoLabels(CCLabelBMFont* m_timeLabel, CCLabelBMFont* m_currentPlayerLabel, CCLabelBMFont* m_nextPlayerLabel) {
    m_timeLabel->setString(formatTime(PlayerSwap::timeUntilSwap).c_str());
    m_currentPlayerLabel->setString(idToString(PlayerSwap::currentPlayer).c_str());
    m_currentPlayerLabel->setColor(idToCol(PlayerSwap::currentPlayer));
    m_nextPlayerLabel->setString(idToString(PlayerSwap::nextPlayer).c_str());
    m_nextPlayerLabel->setColor(idToCol(PlayerSwap::nextPlayer));
}

void setupInfoLabels(CCLabelBMFont*& m_timeLabel, CCLabelBMFont*& m_currentPlayerLabel, CCLabelBMFont*& m_nextPlayerLabel, CCLayer* parent) {
    auto winSize = CCDirector::get()->getWinSize();
    
    auto timeLabel = CCLabelBMFont::create("", "bigFont.fnt");
    timeLabel->setPosition(winSize);
    timeLabel->setOpacity(125);
    timeLabel->setAnchorPoint(ccp(1.0f, 1.0f));
    parent->addChild(timeLabel);
    m_timeLabel = timeLabel;

    auto currentPlayerLabel = CCLabelBMFont::create("", "bigFont.fnt");
    currentPlayerLabel->setScale(0.5f);
    currentPlayerLabel->setPosition(winSize.width, winSize.height - 32.5f);
    currentPlayerLabel->setOpacity(100);
    currentPlayerLabel->setAnchorPoint(ccp(1.0f, 1.0f));
    parent->addChild(currentPlayerLabel);
    m_currentPlayerLabel = currentPlayerLabel;

    auto nextPlayerLabel = CCLabelBMFont::create("", "bigFont.fnt");
    nextPlayerLabel->setScale(0.4f);
    nextPlayerLabel->setPosition(winSize.width, winSize.height - 50);
    nextPlayerLabel->setOpacity(75);
    nextPlayerLabel->setAnchorPoint(ccp(1.0f, 1.0f));
    parent->addChild(nextPlayerLabel);
    m_nextPlayerLabel = nextPlayerLabel;
}

std::string idToString(int id) {
    if (usernameMap.contains(id)) return usernameMap.at(id);
    return std::to_string(id);
}

ccColor3B idToCol(int id) {
    if (usernameColorMap.contains(id)) return usernameColorMap.at(id);
    return ccWHITE;
}


std::string formatTime(float time) {
    int minutes = std::floor(time / 60.0f);
    int seconds = std::ceil(time - (minutes * 60));
    return fmt::format("{}:{:02}", minutes, seconds);
};