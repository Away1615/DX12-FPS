#include "AnimatedMeshRenderComponent.h"
#include "../CameraComponent.h"
#include "../Animator/AnimatorComponent.h"
#include "../../GameObject.h"
#include "../../../Foundation/Base/Transform.h"
#include "../../../Foundation/Base/Maths.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Model/ModelData.h"

AnimatedMeshRenderComponent::AnimatedMeshRenderComponent(std::vector<SubMesh>& subMeshes)
    : MeshRenderComponentBase(subMeshes) {
}

void AnimatedMeshRenderComponent::onStart() {
    initMaterials();
}

void AnimatedMeshRenderComponent::onRender(RenderContext& renderContext) {
    auto* animator = _owner->getComponent<AnimatorComponent>();
    if (!animator) return;

    Matrix W = transform().worldMatrix();
    Matrix V = mainCamera()->view;
    Matrix P = mainCamera()->projection;

    MaterialParam param;
    param.W = W;
    param.VP = V * P;
    param.bones = animator->bonesMatrixs();

    drawMeshes(renderContext, param);
}
