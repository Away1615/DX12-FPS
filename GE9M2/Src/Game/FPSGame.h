#pragma once
#include "Game.h"
#include "Level/LevelLoader.h"

class FPSGame : public Game {

private:
    LevelLoader _levelLoader;

public:
    void onInit(Engine& engine, Scene& scene) override;
};

