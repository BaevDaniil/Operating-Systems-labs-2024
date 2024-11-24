#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <atomic>
#include <string>
#include <../utilsConfigurations/safeMap.h>

enum connectState
{
    CONNECTION,
    DISCONNECTION,
    LOAD,
};

enum goatStatus
{
    ALIVE,
    DEAD,
    FINISH,
};

struct Goat
{
    std::atomic<int> goatNumber;
    std::atomic<goatStatus> status;
    std::atomic<connectState> stateClient;
};

struct GameWorld
{
    std::atomic<int> wolfNumber;
    std::atomic<int> aliveGoatNumber;
    std::atomic<int> deadGoatNumder;
    SafeMap<pid_t, std::shared_ptr<Goat>> goatMap;
    std::atomic<int> time;
};

#endif