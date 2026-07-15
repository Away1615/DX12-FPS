#pragma once
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "../../../../Third_Party/GEMLoader.h"

class RenderContext;
class MeshLibrary;
class Mesh;
class ModelData;
class AnimationData;

class ModelLoader {
private:
    const std::string                   _prefix = "primitive:";
    std::unique_ptr<MeshLibrary>        _meshLib;
    RenderContext&                      _renderContext;
    std::vector<std::unique_ptr<Mesh>> _meshes;
    std::vector<std::unique_ptr<AnimationData>> _animations;
    std::map<std::string, std::unique_ptr<ModelData>> _loadedModelCache;
    GEMLoader::GEMModelLoader           _loader;

public:
    ModelLoader(RenderContext& renderContext);
    ~ModelLoader();

    ModelData* loadModel(const std::string& modelPath, const std::string& materialKey);
    MeshLibrary* meshLib();

private:
    ModelData* loadPrimitiveModel(const std::string& modelPath, const std::string& materialKey);
    ModelData* loadStaticGEMModel(const std::string& modelPath, const std::string& materialKey);
    ModelData* loadAnimatedGEMModel(const std::string& modelPath, const std::string& materialKey);
    void listAnimationNames(const GEMLoader::GEMAnimation& gemanimation);
};
