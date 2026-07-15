#include "GameObject.h"
#include "Scene.h"
#include "Components/Component.h"
#include "../Engine.h"
#include "../Foundation/Base/Transform.h"

GameObject::GameObject() {
    _transform = std::make_unique<Transform>();
}

GameObject::~GameObject() = default;

void GameObject::setContext(Scene* scene, Engine* engine) {
    _scene = scene;
    _engine = engine;
}

Transform& GameObject::transform() { return *_transform; }
const Transform& GameObject::transform() const { return *_transform; }
Scene* GameObject::scene() const { return _scene; }
Engine* GameObject::engine() const { return _engine; }

CameraComponent* GameObject::mainCamera() {
    return _scene ? _scene->mainCamera() : nullptr;
}

void GameObject::update(float dt) {
    for (const auto& component : _components)
        component->onUpdate(dt);
}

const std::vector<std::unique_ptr<Component>>& GameObject::components() const {
    return _components;
}
