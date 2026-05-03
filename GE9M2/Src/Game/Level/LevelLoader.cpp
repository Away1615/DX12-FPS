#include "LevelLoader.h"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include "../../Engine/Engine.h"
#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Scene/GameObject.h"
#include "../../Engine/Graphics/Material/Material.h"
#include "../../Engine/Graphics/Model/ModelData.h"
#include "../../Engine/Graphics/Model/ModelLoader.h"
#include "../../Engine/Scene/Components/Factory/ComponentFactory.h"
#include "../../Engine/Foundation/DX12/DX12Upload.h"
#include "../../Engine/Foundation/DX12/DX12Renderer.h"

static bool blockOpen(std::string line) {
    return line.find('{') != std::string::npos;
}

static bool blockClose(std::string line) {
    return line.find('}') != std::string::npos;
}

void LevelLoader::load(const std::string& path, Scene& scene, Engine& engine) {
    std::ifstream file(path);
    assert(file.is_open());

    std::vector<BlockContext> stack;
    std::string line;

    while (std::getline(file, line)) {

        if (line.empty() || line[0] == '#')
            continue;

        // ---------- block close ----------
        if (line.find('}') != std::string::npos) {
            assert(!stack.empty());
            stack.pop_back();
            continue;
        }

        // ---------- block open ----------
        if (line.find('{') != std::string::npos) {
            openBlock(line, stack, scene, engine);
            continue;
        }

        // ---------- content ----------
        if (!stack.empty()) {
            parseContent(line, stack, scene, engine);
        }
    }
};

void LevelLoader::openBlock(const std::string& line, std::vector<BlockContext>& stack, Scene& scene, Engine& engine) {
    std::stringstream ss(line);
    std::string keyword;
    ss >> keyword;

    BlockContext ctx;

    if (keyword == "texture") {
        ctx.type = BlockType::Texture;
    }

    else if (keyword == "shader") {
        ctx.type = BlockType::Shader;
    }

    else if (keyword == "pso") {
        ctx.type = BlockType::PSO;
    }

    else if (keyword == "material") {
        ctx.type = BlockType::Material;

        std::string matName;
        ss >> matName;

        auto& matMgr = engine.renderContext().materialManager();

        Material* mat = new Material();
        matMgr.add(matName, mat);
        ctx.material = mat;
    }

    else if (keyword == "light") {
        ctx.type = BlockType::Light;
    }

    else if (keyword == "scene") {
        ctx.type = BlockType::Scene;
    }

    else if (keyword == "object") {
        ctx.type = BlockType::Object;
        ss >> ctx.name;
        ctx.object = scene.createObject();
        ctx.object->setName(ctx.name);
    }

    else if (keyword == "static_instances") {
        ctx.type = BlockType::StaticInstances;
    }

    else if (keyword == "static_mesh") {
        ctx.type = BlockType::StaticMesh;
        std::string modelPath, materialKey;
        ss >> modelPath >> materialKey;
        ctx.model = engine.loader().loadModel(modelPath, materialKey);
    }

    stack.push_back(ctx);
}

void LevelLoader::parseContent(const std::string& line, std::vector<BlockContext>& stack, Scene& scene, Engine& engine) {
    BlockContext& ctx = stack.back();

    // ---------- texture ----------
    if (ctx.type == BlockType::Texture) {
        std::string key, path;
        std::stringstream ss(line);
        ss >> key >> path;
        auto* device = engine.renderContext().device().dxDevice();
        auto& uploader = engine.renderContext().uploader();
        auto& heap = engine.renderContext().srvHeap();
        auto& textureManager = engine.renderContext().textureManager();

        TextureUsage usage = (key == "albedoTex") ? TextureUsage::Color : TextureUsage::Data;

        textureManager.loadTexture(device, uploader, heap, key, path, usage);
    }

    // ---------- shader ----------
    if (ctx.type == BlockType::Shader) {
        std::string name, vs, ps;
        std::stringstream ss(line);
        ss >> name >> vs >> ps;
        auto* device = engine.renderContext().device().dxDevice();
        auto& rootSig = engine.renderContext().rootSignature();
        auto& shaderManager = engine.renderContext().shaderManager();

        shaderManager.loadShader(name, vs, ps, device, rootSig);
    }

    // ---------- pso ----------
    if (ctx.type == BlockType::PSO) {
        std::string psoName, shaderName, layout, depthFunc, depthWrite;
        std::stringstream ss(line);
        ss >> psoName >> shaderName >> layout >> depthFunc >> depthWrite;
        auto* device = engine.renderContext().device().dxDevice();
        auto* rootSignature = engine.renderContext().rootSignature().rootSignature();
        auto& shaderManager = engine.renderContext().shaderManager();
        auto& psoManager = engine.renderContext().psoManager();

        Shader* shader = shaderManager.find(shaderName);

        PSOParam param;
        param.psoName = psoName;
        param.vsBlob = shader->vs.Get();
        param.psBlob = shader->ps.Get();

        param.layout =
            (layout == "a") ? DX12VertexLayoutCache::getAnimatedLayout() :
            (layout == "s") ? DX12VertexLayoutCache::getStaticLayout() :
            (layout == "si") ? DX12VertexLayoutCache::getStaticInstancedLayout() :
            DX12VertexLayoutCache::getStaticLayout();

        param.depthFunc =
            (depthFunc == "depth_less_equal") ? D3D12_COMPARISON_FUNC_LESS_EQUAL :
            (depthFunc == "depth_always") ? D3D12_COMPARISON_FUNC_ALWAYS :
            (depthFunc == "depth_less") ? D3D12_COMPARISON_FUNC_LESS :
            D3D12_COMPARISON_FUNC_LESS;

        param.depthWriteMask =
            (depthWrite == "nowrite") ? D3D12_DEPTH_WRITE_MASK_ZERO :
            (depthWrite == "write") ? D3D12_DEPTH_WRITE_MASK_ALL :
            D3D12_DEPTH_WRITE_MASK_ALL;

        psoManager.createPSO(device, rootSignature, param);
    }

    // ---------- material ----------
    if (ctx.type == BlockType::Material) {
        Material* mat = ctx.material;
        assert(mat);

        std::stringstream ss(line);
        std::string keyword;
        ss >> keyword;

        if (keyword == "shader") {
            std::string shader, cb;
            ss >> shader >> cb;
            mat->setShader(shader, cb);
        }
        else if (keyword == "pso") {
            std::string psoName;
            ss >> psoName;
            mat->setPSO(psoName);
        }
        else if (keyword == "texture") {
            std::string slot, tex;
            ss >> slot >> tex;
            mat->addTexture(slot, tex);
        }
        else if (keyword == "uv") {
            float u, v;
            ss >> u >> v;
            mat->setUVScale({ u, v });
        }
        else if (keyword == "alphaTest") {
            int v;
            ss >> v;
            mat->setAlphaTest(v);
        }
        else if (keyword == "vsAnim") {
            int v;
            ss >> v;
            mat->setVSAnim(v);
        }
    }

    // ---------- light ----------
    if (ctx.type == BlockType::Light) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "skylight") {
            float r, g, b, intensity;
            ss >> r >> g >> b >> intensity;

            SkyLight light;
            light.color = Vec3(r, g, b);
            light.intensity = intensity;

            scene.setSkyLight(light);
        }
        else if (type == "pointlight") {
   
            Vec3 position;
            Vec3 color;
            float range, intensity;

            ss >> position.x >> position.y >> position.z;
            ss >> color.x >> color.y >> color.z;
            ss >> range >> intensity;

            PointLight light;
            light.position = position;
            light.color = color;
            light.range = range;
            light.intensity = intensity;

            scene.addLight(light);
        }
    }

    // ---------- object ----------
    if (ctx.type == BlockType::Object) {

        std::string token = firstToken(line);

        if (token == "transform") {
            ctx.object->transform() = getTransform(line);
        }

        else if (token == "component") {

            std::stringstream ss(line);
            std::string keyword;
            ss >> keyword;

            std::string componentName;
            ss >> componentName;

            std::vector<std::string> args;
            std::string arg;
            while (ss >> arg) {
                args.push_back(decodeArg(arg));
            }

            bool success = ComponentFactory::shared().create(
                componentName,
                ctx.object,
                engine,
                args
            );

            assert(success);
        }
    }

    // ---------- static mesh instancing ----------
    if (ctx.type == BlockType::StaticMesh) {
        std::string token = firstToken(line);
        if (token == "transform") {
            Transform t = getTransform(line);
            scene.addStaticMeshInstance(ctx.model, InstanceData{ t.worldMatrix() }
            );
        }
    }
}

std::string LevelLoader::firstToken(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    ss >> token;
    return token;
}


Transform LevelLoader::getTransform(std::string line) {
    std::stringstream ss(line);
    Transform transform;
    std::string keyword;
    ss >> keyword;
    ss >> transform.position.x >> transform.position.y >> transform.position.z;
    ss >> transform.rotation.x >> transform.rotation.y >> transform.rotation.z >> transform.rotation.w;
    ss >> transform.scale.x >> transform.scale.y >> transform.scale.z;

    return transform;
}

std::string LevelLoader::decodeArg(const std::string& in) {
    std::string out = in;
    size_t pos = 0;

    while ((pos = out.find("~", pos)) != std::string::npos) {
        out.replace(pos, 1, " ");
        pos += 1;
    }

    return out;
}