#include "MeshRenderComponentBase.h"
#include "../../../Engine.h"
#include "../../../Graphics/RenderContext.h"
#include "../../../Graphics/Material/Material.h"
#include "../../../Graphics/Material/MaterialManager.h"
#include "../../../Graphics/Model/ModelData.h"

MeshRenderComponentBase::MeshRenderComponentBase(std::vector<SubMesh>& subMeshes)
    : _subMeshes(subMeshes) {
}

void MeshRenderComponentBase::initMaterials() {
    auto& ctx = engine()->renderContext();
    auto& materialManager = ctx.materialManager();

    for (auto& subMesh : _subMeshes) {
        _materials.push_back(
            materialManager.createInstance(ctx, subMesh)
        );
    }
}

void MeshRenderComponentBase::drawMeshes(RenderContext& renderContext, MaterialParam& param) {
    auto* cmd = renderContext.renderer().commandList();

    for (int i = 0; i < _subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _subMeshes[i].mesh->draw(cmd);
    }
}