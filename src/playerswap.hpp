#pragma once

namespace PlayerSwap {
    inline bool enabled = true;

    inline int syncedTime = 0;
    inline std::chrono::steady_clock::time_point syncedStart;

    inline bool lockSwapUpdates = false;
    inline int currentPlayer;
    inline int nextPlayer;
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
}