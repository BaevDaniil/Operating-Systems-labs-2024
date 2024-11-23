#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <atomic>
#include <string>
#include <../utilsConfigurations/safeMap.h>

std::string semWolfName = "/hostWolf"
std::string semGoatName = "/clientGoat"

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
    safeMap<pid_t, std::shared_ptr<Goat>> goatMap;
    std::atomic<int> time;
}

#endif