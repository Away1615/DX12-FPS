#pragma once
#include "MeshRenderComponentBase.h"

class AnimatedMeshRenderComponent : public MeshRenderComponentBase {
public:
    AnimatedMeshRenderComponent(std::vector<SubMesh>& subMeshes);

    void onStart() override;
    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "AnimatedMeshRenderComponent"; }
};
