#pragma once
#include "Component.h"
#include "../../Foundation/Base/Maths.h"

class Transform;

class CameraComponent : public Component {

private:
    float _fov                   = 75.0f;
    float _nearPlane             = 0.1f;
    float _farPlane              = 1000.0f;
    float _aspectRatio           = 1.0f;
    float _eyeHeight             = 1.8f;
    bool _needToUpdateProjetion  = true;

public:
    Matrix view;
    Matrix projection;

    CameraComponent(float aspectRatio);

    void onStart() override;

    void onUpdate(float dt) override;

    void updateProjectionIfNeeded();

    void updateViewMatrix();

    static std::string Name() { return "CameraComponent"; }
};
