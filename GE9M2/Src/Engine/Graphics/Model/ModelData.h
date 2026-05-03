#pragma once
#include "../Mesh/Mesh.h"
#include "../Material/Material.h"
#include "../Animation/AnimationData.h"

struct SubMesh {
public:
    Mesh* mesh = nullptr;
    std::string materialKey;
    std::string albedoTex;
    std::string normalTex;
};

class ModelData {
public:
    AnimationData* animation = nullptr;
    std::vector<SubMesh>      subMeshes;

    bool isAnimated() { return animation != nullptr; }

    ModelData() = default;

    ModelData(const ModelData&) = delete;
    ModelData& operator=(const ModelData&) = delete;
};