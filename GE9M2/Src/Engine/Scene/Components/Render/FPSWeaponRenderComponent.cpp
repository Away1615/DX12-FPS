#include "FPSWeaponRenderComponent.h"
#include "../CameraComponent.h"
#include "../Animator/AnimatorComponent.h"
#include "../../GameObject.h"
#include "../../../Foundation/Base/Transform.h"
#include "../../../Foundation/Base/Maths.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Model/ModelData.h"

FPSWeaponRenderComponent::FPSWeaponRenderComponent(std::vector<SubMesh>& subMeshes)
    : MeshRenderComponentBase(subMeshes) {
}

void FPSWeaponRenderComponent::onStart() {
    initMaterials();
}

void FPSWeaponRenderComponent::onRender(RenderContext& renderContext) {
    auto* animator = _owner->getComponent<AnimatorComponent>();
    if (!animator) return;

    // Rotate model to right direction
    Matrix armsRotateFix = Matrix::RotateY(M_PI);
    Matrix armsOffset = Matrix::Translation(_baseOffset);

    // Move model to right position
    Matrix W = armsRotateFix * armsOffset;
    // The weapon will fix on the screen, so it won't use camera view matrix
    Matrix V = Matrix::Identity();
    // use projection matrix to ensure the right perspective
    Matrix P = mainCamera()->projection;

    MaterialParam param;
    param.W = W;
    param.VP = V * P;
    param.bones = animator->bonesMatrixs();

    param.isWeapon = 1;

    drawMeshes(renderContext, param);
}
