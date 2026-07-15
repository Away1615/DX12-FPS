#include "ModelLoader.h"
#include <string>
#include <map>
#include <filesystem>
#include "ModelData.h"
#include "../Mesh/MeshLib.h"
#include "../../../../Third_Party/GEMLoader.h"

ModelLoader::ModelLoader(RenderContext& renderContext) : _renderContext(renderContext) {
    _meshLib = std::make_unique<MeshLibrary>(renderContext);
}

ModelLoader::~ModelLoader() = default;

MeshLibrary* ModelLoader::meshLib() { return _meshLib.get(); }

ModelData* ModelLoader::loadModel(const std::string& modelPath, const std::string& materialKey) {

    // Load from cache
    if (_loadedModelCache.count(modelPath))
        return _loadedModelCache[modelPath].get();

    // Load from primitive
    if (modelPath.starts_with(_prefix))
        return loadPrimitiveModel(modelPath, materialKey);

    // Load from GEM
    if (_loader.isAnimatedModel(modelPath))
        return loadAnimatedGEMModel(modelPath, materialKey);
    return loadStaticGEMModel(modelPath, materialKey);
}

ModelData* ModelLoader::loadPrimitiveModel(const std::string& modelPath, const std::string& materialKey) {
    std::string primitiveName = modelPath.substr(_prefix.size());

    auto ownedData = std::make_unique<ModelData>();
    ModelData* data = ownedData.get();
    Mesh* mesh = &_meshLib->getMesh(primitiveName);

    data->subMeshes.push_back({ mesh, materialKey,"", "" });

    _loadedModelCache[modelPath] = std::move(ownedData);
    return data;

}

ModelData* ModelLoader::loadStaticGEMModel(const std::string& modelPath, const std::string& materialKey) {
    auto ownedData = std::make_unique<ModelData>();
    ModelData* data = ownedData.get();
    std::vector<GEMLoader::GEMMesh> gemmeshes;

    _loader.load(modelPath, gemmeshes);

    std::vector<std::string> temps;

    // ------------ Load Mesh & Textures ------------

    for (auto& gemmesh : gemmeshes) {
        auto ownedSubMesh = std::make_unique<Mesh>();
        Mesh* subMesh = ownedSubMesh.get();

        // Load Meshes
        std::vector<STATIC_VERTEX> vertices;
        for (auto& gemVertex : gemmesh.verticesStatic) {
            STATIC_VERTEX vertex;
            memcpy(&vertex, &gemVertex, sizeof(STATIC_VERTEX));
            vertices.push_back(vertex);
        }

        subMesh->createStatic(_renderContext.device().dxDevice(), _renderContext.uploader(), vertices, gemmesh.indices);
        
        // Load Texture
        std::string albedo = gemmesh.material.find("albedo").getValue();
        if (!albedo.empty()) {
            std::string fileName = std::filesystem::path(albedo).filename().string();
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                albedo,
                fullPath,
                TextureUsage::Color
            );
        }
        std::string nh = gemmesh.material.find("nh").getValue();
        if (!nh.empty()) {
            std::string fileName = std::filesystem::path(nh).filename().string();
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                nh,
                fullPath,
                TextureUsage::Data
            );
        }

        data->subMeshes.push_back({ subMesh, materialKey, albedo, nh });
        _meshes.push_back(std::move(ownedSubMesh));
    }

    _loadedModelCache[modelPath] = std::move(ownedData);
    return data;
}

ModelData* ModelLoader::loadAnimatedGEMModel(const std::string& modelPath, const std::string& materialKey) {
    auto ownedData = std::make_unique<ModelData>();
    ModelData* data = ownedData.get();
    std::vector<GEMLoader::GEMMesh> gemmeshes;
    GEMLoader::GEMAnimation gemanimation;

    _loader.load(modelPath, gemmeshes, gemanimation);

    listAnimationNames(gemanimation);

    // ------------ Load Mesh & Textures ------------

    for (auto& gemmesh : gemmeshes) {
        auto ownedSubMesh = std::make_unique<Mesh>();
        Mesh* subMesh = ownedSubMesh.get();

        // Load Meshes
        std::vector<ANIMATED_VERTEX> animatedVertices;
        for (auto& gemAnimatedVertex : gemmesh.verticesAnimated) {
            ANIMATED_VERTEX vanimatedVertex;
            memcpy(&vanimatedVertex, &gemAnimatedVertex, sizeof(ANIMATED_VERTEX));
            animatedVertices.push_back(vanimatedVertex);
        }

        subMesh->createAnimated(_renderContext.device().dxDevice(), _renderContext.uploader(), animatedVertices, gemmesh.indices);

        // Load Texture
        std::string texName = gemmesh.material.find("albedo").getValue();
        if (!texName.empty()) {
            std::string fileName = std::filesystem::path(texName).filename().string();
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                texName,
                fullPath,
                TextureUsage::Color
            );
        }
        std::string nh = gemmesh.material.find("nh").getValue();
        if (!nh.empty()) {
            std::string fileName = std::filesystem::path(nh).filename().string();
            std::string fullPath = "Src/Assets/Models/Textures/" + fileName;
            _renderContext.textureManager().loadTexture(
                _renderContext.device().dxDevice(),
                _renderContext.uploader(),
                _renderContext.srvHeap(),
                nh,
                fullPath,
                TextureUsage::Data
            );
        }
        data->subMeshes.push_back({ subMesh, materialKey, texName, "" });
        _meshes.push_back(std::move(ownedSubMesh));
    }

    // ------------ Load Animation ------------
    auto ownedAnimation = std::make_unique<AnimationData>();
    data->animation = ownedAnimation.get();

    // Load globalInverse
    memcpy(&data->animation->skeleton.globalInverse, &gemanimation.globalInverse, sizeof(Matrix));

    // Load Skeleton & bones
    for (auto& gemBone : gemanimation.bones) {
        Bone bone;
        bone.name = gemBone.name;
        memcpy(&bone.offset, &gemBone.offset, sizeof(Matrix));
        bone.parentIndex = gemBone.parentIndex;
        data->animation->skeleton.bones.push_back(bone);
    }

    // Load animation sequence data
    for (auto& gemAnimation : gemanimation.animations) {
        std::string name = gemAnimation.name;
        AnimationSequence aseq;
        aseq.ticksPerSecond = gemAnimation.ticksPerSecond;
        for (auto& gemFrame : gemAnimation.frames) {
            AnimationFrame frame;
            for (int index = 0; index < gemFrame.positions.size(); index++) {
                Vec3 p;
                memcpy(&p, &gemFrame.positions[index], sizeof(Vec3));
                Quaternion q;
                memcpy(&q, &gemFrame.rotations[index], sizeof(Quaternion));
                Vec3 s;
                memcpy(&s, &gemFrame.scales[index], sizeof(Vec3));

                frame.boneLocalTransforms.push_back(Transform(p, q, s));
            }
            aseq.frames.push_back(frame);
        }
        data->animation->animations.insert({ name, aseq });
    }

    _animations.push_back(std::move(ownedAnimation));
    _loadedModelCache[modelPath] = std::move(ownedData);
    return data;
}

void ModelLoader::listAnimationNames(const GEMLoader::GEMAnimation& gemanimation) {
    for (int i = 0; i < gemanimation.animations.size(); i++)
    {
        std::cout << gemanimation.animations[i].name << std::endl;
    }
}
