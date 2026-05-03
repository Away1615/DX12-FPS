#include "ColliderComponent.h"
#include <string>
#include "Component.h"
#include "CameraComponent.h"
#include "../GameObject.h"
#include "../../Engine.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/RenderContext.h"
#include "../../Graphics/Model/ModelData.h"
#include "../../Graphics/Mesh/MeshLib.h"

const Vec3& ColliderComponent::size() const { return _size; }

Vec3 ColliderComponent::worldCenter() const {
    return Vec3(
        transform().position.x,
        transform().position.y + _size.y * 0.5f,
        transform().position.z
    );
}

Vec3 ColliderComponent::worldMin() const {
    return Vec3(
        transform().position.x - _size.x * 0.5f,
        transform().position.y,
        transform().position.z - _size.z * 0.5f
    );
}

Vec3 ColliderComponent::worldMax() const {
    return Vec3(
        transform().position.x + _size.x * 0.5f,
        transform().position.y + _size.y,
        transform().position.z + _size.z * 0.5f
    );
}

bool ColliderComponent::intersect(const ColliderComponent* other) const {
    Vec3 aMin = worldMin();
    Vec3 aMax = worldMax();
    Vec3 bMin = other->worldMin();
    Vec3 bMax = other->worldMax();

    return !(aMax.x < bMin.x || aMin.x > bMax.x ||
        aMax.y < bMin.y || aMin.y > bMax.y ||
        aMax.z < bMin.z || aMin.z > bMax.z);
}

bool ColliderComponent::raycast(
    const Vec3& rayOrigin,
    const Vec3& rayDirection,
    float& hitDistance
) const
{
    Vec3 min = worldMin();
    Vec3 max = worldMax();

    float tx1 = (min.x - rayOrigin.x) / rayDirection.x;
    float tx2 = (max.x - rayOrigin.x) / rayDirection.x;

    float tmin = (std::min)(tx1, tx2);
    float tmax = (std::max)(tx1, tx2);

    float ty1 = (min.y - rayOrigin.y) / rayDirection.y;
    float ty2 = (max.y - rayOrigin.y) / rayDirection.y;

    tmin = (std::max)(tmin, (std::min)(ty1, ty2));
    tmax = (std::min)(tmax, (std::max)(ty1, ty2));

    float tz1 = (min.z - rayOrigin.z) / rayDirection.z;
    float tz2 = (max.z - rayOrigin.z) / rayDirection.z;

    tmin = (std::max)(tmin, (std::min)(tz1, tz2));
    tmax = (std::min)(tmax, (std::max)(tz1, tz2));

    if (tmax < 0) return false;
    if (tmin > tmax) return false;

    hitDistance = tmin;
    return true;
}


void ColliderComponent::onRender(RenderContext& renderContext) {

    CameraComponent* camera = mainCamera();
    if (!camera) return;

    ID3D12GraphicsCommandList4* cmd = renderContext.renderer().commandList();
    PSOManager& psos = renderContext.psoManager();

    psos.bind(cmd, "debugLinePSO");

    Vec3 min = worldMin();
    Vec3 max = worldMax();

    Vec3 center = (min + max) * 0.5f;
    Vec3 size = max - min;

    Transform collisionT = Transform(center, Quaternion(), size);

    Matrix W = collisionT.localMatrix();
    Matrix VP = mainCamera()->view * mainCamera()->projection;

    renderContext.shaderManager().updateConstantVS("debugShader", "debugCB", "W", &W);
    renderContext.shaderManager().updateConstantVS("debugShader", "debugCB", "VP", &VP);

    renderContext.shaderManager().apply(cmd, "debugShader");

    engine()->loader().meshLib()->debugBox.drawLineList(cmd);
}