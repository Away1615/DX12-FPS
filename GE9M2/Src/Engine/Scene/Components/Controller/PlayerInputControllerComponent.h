#pragma once
#include "../Component.h"
#include "../../../Foundation/Base/Maths.h"

class WeaponControllerComponent;

class PlayerInputControllerComponent : public Component {
private:
    // -------- Movement --------
    float _moveSpeed = 12.0f;

    // -------- Mouse Look --------
    float _mouseSensitivity = 0.002f;
    float _pitch = 0.0f;
    float _yaw = 0.0f;

    Quaternion _fullLookRotation;

    bool _lastMouseLeft = false;
    bool _lastReloadKey = false;
    bool _lastShowCollisionBoxes = false;

public:
    void onStart() override;
    void onUpdate(float dt) override;

    const Quaternion& getLookRotation() const { return _fullLookRotation; }

    static std::string Name() { return "PlayerInputControllerComponent"; }
};
