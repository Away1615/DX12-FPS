#include "SkyRenderComponent.h"
#include "../CameraComponent.h"
#include "../../Scene.h"
#include "../../../Foundation/Base/Transform.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Model/ModelData.h"

SkyRenderComponent::SkyRenderComponent(std::vector<SubMesh>& subMeshes) 
    : MeshRenderComponentBase(subMeshes) {}

void SkyRenderComponent::onStart() {
    initMaterials();
}

void SkyRenderComponent::onRender(RenderContext& renderContext) {
    auto* cam = mainCamera();
    if (!cam) return;

    MaterialParam param;
    param.W = Matrix::Translation(cam->transform().position);
    param.VP = cam->view.withoutTranslation() * cam->projection;

    drawMeshes(renderContext, param);
}


