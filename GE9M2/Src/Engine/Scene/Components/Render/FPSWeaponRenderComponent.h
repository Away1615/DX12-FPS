#pragma once
#include "MeshRenderComponentBase.h"
#include "../../../Foundation/Base/Maths.h"

class FPSWeaponRenderComponent: public MeshRenderComponentBase {

private:
    Vec3 _baseOffset = Vec3(0.0f, 0.0f, 2.0f);

public:
    FPSWeaponRenderComponent(std::vector<SubMesh>& subMeshes);

    void onStart() override;
    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::FPS; }

    static std::string Name() { return "FPSWeaponRenderComponent"; }
};
