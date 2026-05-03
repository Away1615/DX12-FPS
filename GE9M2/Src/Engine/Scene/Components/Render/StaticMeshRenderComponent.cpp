#include "StaticMeshRenderComponent.h"
#include "../CameraComponent.h"
#include "../../Scene.h"
#include "../../../Foundation/Base/Transform.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Model/ModelData.h"

StaticMeshRenderComponent::StaticMeshRenderComponent(std::vector<SubMesh>& subMeshes)
    : MeshRenderComponentBase(subMeshes) {
}

void StaticMeshRenderComponent::onStart() {
    initMaterials();
}

void StaticMeshRenderComponent::onRender(RenderContext& renderContext) {
    Matrix W = transform().worldMatrix();
    Matrix V = mainCamera()->view;
    Matrix P = mainCamera()->projection;

    MaterialParam param;
    param.W = W;
    param.VP = V * P;

    drawMeshes(renderContext, param);
}
