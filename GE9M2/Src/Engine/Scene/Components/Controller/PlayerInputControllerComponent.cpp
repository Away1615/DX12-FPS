#include "PlayerInputControllerComponent.h"
#include "WeaponControllerComponent.h"
#include "../../../Foundation/Base/Transform.h"
#include "../../../Foundation/Window/Window.h"
#include "../../../Engine.h"
#include "../../GameObject.h"
#include "../ColliderComponent.h"

void PlayerInputControllerComponent::onStart() {
    Vec3 f = transform().forward();
    if (f.lengthSqrt() > 0.0001f) {
        f = f.normalized();
        _yaw = std::atan2(f.x, f.z);
        _pitch = std::asin(_clamp(f.y, -1.0f, 1.0f));
    }
}

void PlayerInputControllerComponent::onUpdate(float dt) {
    if (!window) return;

    // =====================================================
    // ESC -> Quit
    // =====================================================
    if (window->keys[VK_ESCAPE]) {
        engine()->quit();
        return;
    }

    // =====================================================
    // P -> Open Debug
    // =====================================================
    if (window->keys['P'] && !_lastShowCollisionBoxes) {
        engine()->toggleShowCollider();
    }

    // =====================================================
    // Mouse Look
    // =====================================================
    float dx = window->mouseDeltaX;
    float dy = window->mouseDeltaY;
    window->mouseDeltaX = 0;
    window->mouseDeltaY = 0;

    _yaw += dx * _mouseSensitivity;
    _pitch += dy * _mouseSensitivity;
    _pitch = _clamp(_pitch, -1.5f, 1.5f);

    Quaternion qYaw = Quaternion::fromAxisAngle(Vec3(0, 1, 0), _yaw);
    Vec3 right = qYaw.rotate(Vec3(1, 0, 0));
    Quaternion qPitch = Quaternion::fromAxisAngle(right, _pitch);

    _fullLookRotation = (qPitch * qYaw).normalized();
    transform().rotation = qYaw;

    // =====================================================
    // Movement (WASD)
    // =====================================================
    float inputX = 0.0f;
    float inputZ = 0.0f;

    if (window->keys['W']) inputZ += 1.0f;
    if (window->keys['S']) inputZ -= 1.0f;
    if (window->keys['A']) inputX -= 1.0f;
    if (window->keys['D']) inputX += 1.0f;

    Vec3 forward = transform().forward(); forward.y = 0;
    Vec3 rightV = transform().right();   rightV.y = 0;

    Vec3 moveDir = (forward * inputZ + rightV * inputX);
    if (moveDir.lengthSqrt() > 0.0001f) {
        moveDir = moveDir.normalized();
    }

    // =====================================================
    // Movement + Collision (AABB axis separation)
    // =====================================================
    auto* selfCollider = _owner->getComponent<ColliderComponent>();
    if (selfCollider && moveDir.lengthSqrt() > 0.0f) {

        Vec3 oldPos = transform().position;
        Vec3 newPos = oldPos + moveDir * (_moveSpeed * dt);

        // -------- X axis --------
        transform().position.x = newPos.x;
        for (auto* obj : scene()->objects()) {
            if (obj == _owner) continue;

            auto* other = obj->getComponent<ColliderComponent>();
            if (!other) continue;

            if (selfCollider->intersect(other)) {
                transform().position.x = oldPos.x;
                break;
            }
        }

        // -------- Z axis --------
        transform().position.z = newPos.z;
        for (auto* obj : scene()->objects()) {
            if (obj == _owner) continue;

            auto* other = obj->getComponent<ColliderComponent>();
            if (!other) continue;

            if (selfCollider->intersect(other)) {
                transform().position.z = oldPos.z;
                break;
            }
        }
    }
    else {
        transform().position += moveDir * (_moveSpeed * dt);
    }

    // =====================================================
    // Weapon Input -> WeaponController
    // =====================================================
    auto* weapon = _owner->getComponent<WeaponControllerComponent>();
    if (weapon) {
        bool mouseLeft = window->mouseButtons[0];
        bool reloadKey = window->keys['R'];

        if (mouseLeft && !_lastMouseLeft) {
            weapon->setIntent(WeaponControllerComponent::Intent::Fire);
        }

        if (reloadKey && !_lastReloadKey) {
            weapon->setIntent(WeaponControllerComponent::Intent::Reload);
        }

        _lastMouseLeft = mouseLeft;
        _lastReloadKey = reloadKey;
    }
}
