#pragma once

namespace PlayerSwap {
    inline bool enabled = true;

    inline bool lockSwapUpdates = false;
    inline int currentPlayer;
    inline float timeBetweenSwaps = 10.0f;
    inline float timeUntilSwap = 0.0f;
    inline int playerOffset = 0;
    inline size_t index;
    inline std::vector<int> playerIDs;

    inline size_t lastPlayersOnLevel = 0;
    inline int playerID;

    inline PlayerObject* p1 = nullptr;
    inline PlayerObject* p2 = nullptr;

    void updateTime();
    void updatePlayerIDsVector();
    std::string formatTime(float time);
}