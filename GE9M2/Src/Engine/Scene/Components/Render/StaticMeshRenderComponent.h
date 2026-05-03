#pragma once
#include "MeshRenderComponentBase.h"

class StaticMeshRenderComponent : public MeshRenderComponentBase {
public:
    StaticMeshRenderComponent(std::vector<SubMesh>& subMeshes);

    void onStart() override;
    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "StaticMeshRenderComponent"; }
};
