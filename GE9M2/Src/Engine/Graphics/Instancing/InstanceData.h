#pragma once
#include "../Model/ModelData.h"
#include "../../Foundation/Base/Maths.h"
#include <vector>

struct InstanceData {
    Matrix world;
};


struct StaticMeshInstance {
    ModelData* model = nullptr;
    std::vector<InstanceData> worlds;
    std::vector<Material*> materials;
};
