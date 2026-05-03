#pragma once
#include <string>
#include <map>
#include "../../../../Third_Party/GEMLoader.h"

class RenderContext;
class MeshLibrary;
class ModelData;

class ModelLoader {
private:
    const std::string                   _prefix = "primitive:";
    MeshLibrary*                        _meshLib;
    RenderContext&                      _renderContext;
    std::map<std::string, ModelData*>   _loadedModelCache;
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
