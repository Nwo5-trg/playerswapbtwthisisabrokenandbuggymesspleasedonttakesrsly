#pragma once

inline const std::unordered_map<int, std::string> usernameMap = {
    {0, "all alone 3:"}, {25635884, "Nwo5 :3c"}, {26714258, "Testing Acc"}, 
};

inline const std::unordered_map<int, cocos2d::ccColor3B> usernameColorMap = {
    {0, {0, 0, 0}}, {25635884, {237, 120, 255}}, {26714258, {113, 111, 122}}, 
};

void setPlayerMode(PlayerObject* p1, PlayerObject* swap, PlayLayer* playLayer);
void updateInfoLabels(cocos2d::CCLabelBMFont* m_timeLabel, cocos2d::CCLabelBMFont* m_currentPlayerLabel, cocos2d::CCLabelBMFont* m_nextPlayerLabel);
void setupInfoLabels(cocos2d::CCLabelBMFont*& m_timeLabel, cocos2d::CCLabelBMFont*& m_currentPlayerLabel, cocos2d::CCLabelBMFont*& m_nextPlayerLabel, cocos2d::CCLayer* parent);
std::string formatTime(float time);
std::string idToString(int id);
cocos2d::ccColor3B idToCol(int id);