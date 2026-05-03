#pragma once
#include "../Component.h"
#include "../../../Graphics/Mesh/Mesh.h"
#include "../../../Graphics/Model/ModelData.h"
#include <vector>

class Material;
class RenderContext;
class MaterialParam;
class SubMesh;

class MeshRenderComponentBase : public RenderComponent {
protected:
    std::vector<SubMesh>   _subMeshes;
    std::vector<Material*> _materials;

protected:
    MeshRenderComponentBase(std::vector<SubMesh>& subMeshes);

    void initMaterials();
    void drawMeshes(RenderContext& renderContext, MaterialParam& param);

    static std::string Name() { return "MeshRenderComponentBase"; }
};
