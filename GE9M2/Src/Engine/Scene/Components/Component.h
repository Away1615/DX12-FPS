#pragma once
#include <string>

class GameObject;
class Scene;
class Engine;
class CameraComponent;
class Transform;
class RenderContext;

enum class RenderLayer {
    Sky = 0,
    World = 1,
    FPS = 2,
    DEBUG = 3
};

class Renderable {
public:
    virtual ~Renderable() = default;

    virtual RenderLayer layer() const = 0;
    virtual void onRender(RenderContext& renderContext) = 0;
};


class Component {
protected:
    GameObject* _owner = nullptr;

public:

    void setOwner(GameObject* owner);

    Transform& transform();
    const Transform& transform() const;
    Scene* scene();
    Engine* engine();
    CameraComponent* mainCamera();

    // Called after component is attached to GameObject
    virtual void onStart() {}

    // Per-frame update logic, before render
    virtual void onUpdate(float dt) {}

    static std::string Name() { return "Component"; }
};

class RenderComponent : public Component, public Renderable {

public:

    virtual ~RenderComponent() = default;

    RenderLayer layer() const override {
        return RenderLayer::World;
    }
};
