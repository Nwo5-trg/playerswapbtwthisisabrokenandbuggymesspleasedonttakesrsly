#include "playerswap.hpp"
#include <dankmeme.globed2/include/player.hpp>

using namespace geode::prelude;
using namespace PlayerSwap;

void PlayerSwap::updateTime() {
    size_t playersOnLevel = globed::player::playersOnLevel().unwrap() + 1;
    if (lastPlayersOnLevel != playersOnLevel) {
        lastPlayersOnLevel = playersOnLevel;
        updatePlayerIDsVector();
    }
    if (playersOnLevel < 2) return;
    long double elapsed = std::chrono::duration_cast<std::chrono::duration<long double>>(std::chrono::steady_clock::now() - syncedStart).count();
    long double currentTime = static_cast<long double>(syncedTime) + elapsed;

    if (playerIDs.empty()) {
        timeUntilSwap = -1; 
        return;
    }
    
    long double currentSwap = currentTime / timeBetweenSwaps;
    PlayerSwap::index = ((size_t)(currentSwap) + playerOffset) % playerIDs.size();
    long double nextSwap = (std::floor(currentSwap) + 1) * timeBetweenSwaps;
    timeUntilSwap = nextSwap - currentTime;

    currentPlayer = playerIDs[PlayerSwap::index];
    nextPlayer = playerIDs[(PlayerSwap::index + 1) % playerIDs.size()];
}

void PlayerSwap::updatePlayerIDsVector() {
    playerIDs.clear();
    playerIDs = globed::player::getAllPlayerIds().unwrap();
    playerIDs.push_back(playerID);
    std::sort(playerIDs.begin(), playerIDs.end());
}