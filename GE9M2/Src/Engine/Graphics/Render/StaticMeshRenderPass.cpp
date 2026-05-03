#include "StaticMeshRenderPass.h"
#include "../../Scene/Scene.h"
#include "../RenderContext.h"
#include "../../Scene/Components/CameraComponent.h"
#include "../../Graphics/Model/ModelData.h"

void StaticMeshRenderPass::render(Scene& scene, RenderContext& ctx) {

    auto* camera = scene.mainCamera();
    if (!camera) return;

    auto* cmd = ctx.renderer().commandList();

    MaterialParam param;
    param.VP = camera->view * camera->projection;

    for (const auto& inst : scene.staticMeshInstances()) {

        int instanceCount = (int)inst.worlds.size();
        if (instanceCount == 0) continue;

        ctx.instanceBuffer().upload(inst.worlds.data(), instanceCount);

        for (int i = 0; i < inst.model->subMeshes.size(); ++i) {
            inst.materials[i]->apply(ctx, param);
            inst.model->subMeshes[i].mesh->drawInstanced(
                cmd,
                ctx.instanceBuffer().vertexBufferView(),
                instanceCount
            );
        }
    }
}
