#pragma once

class Engine;
class Scene;

class Game {
public:
    virtual ~Game() = default;

    virtual void onInit(Engine& engine, Scene& scene) = 0;
    virtual void onUpdate(float dt) {}
};
