#include "GameObject.h"
#include "Scene.h"
#include "Components/Component.h"
#include "../Engine.h"
#include "../Foundation/Base/Transform.h"

GameObject::GameObject() {
    _transform = new Transform();
}

GameObject::~GameObject() {
    for (Component* c : _components)
        delete c;
    delete _transform;
}

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
    for (Component* c : _components)
        c->onUpdate(dt);
}

const std::vector<Component*>& GameObject::components() const {
    return _components;
}