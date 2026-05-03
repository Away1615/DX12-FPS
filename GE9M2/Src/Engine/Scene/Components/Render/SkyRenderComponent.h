#pragma once
#include "MeshRenderComponentBase.h"

class RenderContext;
class Material;

class SkyRenderComponent : public MeshRenderComponentBase {
public:
    SkyRenderComponent(std::vector<SubMesh>& subMeshes);

    void onStart() override;
    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::Sky; }

    static std::string Name() { return "SkyRenderComponent"; }
};
