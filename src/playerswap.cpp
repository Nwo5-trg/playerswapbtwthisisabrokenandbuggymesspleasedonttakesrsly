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
    long double currentTime = duration_cast<std::chrono::duration<long double>>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (playerIDs.empty()) {
        timeUntilSwap = -1; 
        return;
    }
    
    long double currentSwap = currentTime / timeBetweenSwaps;
    PlayerSwap::index = ((size_t)(currentSwap) + playerOffset) % playerIDs.size();
    long double nextSwap = (std::floor(currentSwap) + 1) * timeBetweenSwaps;
    timeUntilSwap = nextSwap - currentTime;

    currentPlayer = playerIDs[PlayerSwap::index];
}

void PlayerSwap::updatePlayerIDsVector() {
    playerIDs.clear();
    playerIDs = globed::player::getAllPlayerIds().unwrap();
    playerIDs.push_back(playerID);
    std::sort(playerIDs.begin(), playerIDs.end());
}

std::string PlayerSwap::formatTime(float time) {
    int minutes = std::floor(time / 60.0f);
    int seconds = std::ceil(time - (minutes * 60));
    return fmt::format("{}:{:02}", minutes, seconds);
};