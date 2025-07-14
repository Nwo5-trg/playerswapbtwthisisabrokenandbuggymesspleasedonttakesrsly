#pragma once

inline const std::unordered_map<int, std::string> usernameMap = {
    {0, "all alone 3:"}, {25635884, "Nwo5 :3c"}, {26714258, "Testing Acc"}, 
    {4230210, "MatMart"}, {33430159, "TophShaw"}, {5797731, "Vernosis"},
    {106255, "Colon"}, {5858220, "Moldy"}, {18496, "Tongii"},
    {25370561, "Tatox3"}, {2719706, "Stallion"}, {199031, "SDSlayer"},
    {16991, "Norcda Childa"}
};

inline const std::unordered_map<int, cocos2d::ccColor3B> usernameColorMap = {
    {0, {0, 0, 0}}, {25635884, {237, 120, 255}}, {26714258, {113, 111, 122}},
    {4230210, {245, 66, 66}}, {33430159, {255, 0, 255}}, {5797731, {255, 232, 117}},
    {106255, {255, 140, 18}}, {5858220, {180, 18, 255}}, {18496, {213, 122, 255}},
    {25370561, {34, 255, 0}}, {2719706, {255, 77, 0}}, {199031, {46, 175, 255}},
    {16991, {176, 18, 0}}
};

void setPlayerMode(PlayerObject* p1, PlayerObject* swap, PlayLayer* playLayer);
void updateInfoLabels(cocos2d::CCLabelBMFont* m_timeLabel, cocos2d::CCLabelBMFont* m_currentPlayerLabel, cocos2d::CCLabelBMFont* m_nextPlayerLabel);
void setupInfoLabels(cocos2d::CCLabelBMFont*& m_timeLabel, cocos2d::CCLabelBMFont*& m_currentPlayerLabel, cocos2d::CCLabelBMFont*& m_nextPlayerLabel, cocos2d::CCLayer* parent);
std::string formatTime(float time);
std::string idToString(int id);
cocos2d::ccColor3B idToCol(int id);