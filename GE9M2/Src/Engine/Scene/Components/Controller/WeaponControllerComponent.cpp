#include "WeaponControllerComponent.h"
#include "../../GameObject.h"
#include "../Animator/AnimatorComponent.h"
#include "../../Scene.h"
#include "../ColliderComponent.h"
#include "../CameraComponent.h"
#include "CowControllerComponent.h"


WeaponControllerComponent::WeaponControllerComponent(const WeaponAnimConfig& config) 
    : _animConfig(config) {}

void WeaponControllerComponent::setIntent(Intent intent) {
    // Only accept new intent when idle
    if (_state != State::Idle)
        return;

    if (_intent == Intent::None)
        _intent = intent;
}

void WeaponControllerComponent::onUpdate(float dt) {
    auto* animator = _owner->getComponent<AnimatorComponent>();
    if (!animator) return;

    // Init default animation
    if (!_idleInitialized) {
        animator->play(_animConfig.idle, true);
        _state = State::Idle;
        _idleInitialized = true;
        return;
    }

    switch (_state) {
    case State::Idle:
        if (_intent == Intent::Fire) {
            fireRaycast();
            animator->play(_animConfig.fire, true);
            _state = State::Firing;
        }
        else if (_intent == Intent::Reload) {
            animator->play(_animConfig.reload, true);
            _state = State::Reloading;
        }
        _intent = Intent::None;
        break;

    case State::Firing:
        if (animator->finished()) {
            animator->play(_animConfig.idle);
            _state = State::Idle;
        }
        break;

    case State::Reloading:
        if (animator->finished()) {
            animator->play(_animConfig.idle);
            _state = State::Idle;
        }
        break;
    }
}

void WeaponControllerComponent::fireRaycast()
{
    auto* scene = _owner->scene();
    if (!scene) return;

    auto* cam = scene->mainCamera();
    if (!cam) return;

    Vec3 rayOrigin = cam->transform().position;
    Vec3 rayDir = cam->transform().forward().normalized();

    float closestT = FLT_MAX;
    GameObject* hitObj = nullptr;

    for (auto* obj : scene->objects()) {
        auto* collider = obj->getComponent<ColliderComponent>();
        if (!collider) continue;
        if (obj == _owner) continue;

        float t;
        if (collider->raycast(rayOrigin, rayDir, t)) {
            if (t < closestT) {
                closestT = t;
                hitObj = obj;
            }
        }
    }

    if (hitObj) {
        if (auto* cow = hitObj->getComponent<CowControllerComponent>()) {
            cow->applyDamage(15);
        }
    }
}