#include "CameraComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../Scene.h"
#include "../GameObject.h"
#include "Controller/PlayerInputControllerComponent.h"

CameraComponent::CameraComponent(float aspectRatio) : _aspectRatio(aspectRatio) {}

void CameraComponent::onStart() {
    if (!scene()->mainCamera()) {
        scene()->setMainCamera(this);
    }
}

void CameraComponent::onUpdate(float dt) {
    updateViewMatrix();
    updateProjectionIfNeeded();
}

void CameraComponent::updateProjectionIfNeeded() {
    if (!_needToUpdateProjetion) return;

    projection = Matrix::perspective(_nearPlane, _farPlane, _aspectRatio, _fov);

    _needToUpdateProjetion = false;
}

void CameraComponent::updateViewMatrix() {

    auto* controller = _owner->getComponent<PlayerInputControllerComponent>();

    // get Full rotation
    const Quaternion& lookRotation = controller->getLookRotation();

    Vec3 ownerEyePosition = transform().position + Vec3(0.0f, _eyeHeight, 0.0f);
    // -Z is forward
    Vec3 ownerForward = lookRotation.rotate(Vec3(0, 0, 1));
    Vec3 ownerUp = lookRotation.rotate(Vec3(0, 1, 0));

    view = Matrix::lookAt(ownerEyePosition, ownerEyePosition + ownerForward, ownerUp);
}