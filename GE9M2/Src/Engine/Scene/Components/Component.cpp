#include "Component.h"
#include "CameraComponent.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "../../Engine.h"
#include "../../Foundation/Base/Transform.h"

void Component::setOwner(GameObject* owner) {
    _owner = owner;
}

Transform& Component::transform() {
    return _owner->transform();
}

const Transform& Component::transform() const {
    return _owner->transform();
}

Scene* Component::scene() {
    return _owner->scene();
}

Engine* Component::engine() {
    return _owner->engine();
}

CameraComponent* Component::mainCamera() {
    Scene* s = scene();
    return s ? s->mainCamera() : nullptr;
}
