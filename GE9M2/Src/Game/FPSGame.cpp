#include "FPSGame.h"
#include "../Engine/Engine.h"
#include "../Engine/Scene/Scene.h"
#include "Level/LevelLoader.h"

void FPSGame::onInit(Engine& engine, Scene& scene) {

    _levelLoader.load("Src/Assets/Levels/level_demo.txt", scene, engine);

}
