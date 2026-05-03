#include "Scene.h"
#include <algorithm>
#include "GameObject.h"
#include "Components/Component.h"
#include "Components/ColliderComponent.h"
#include "Components/CameraComponent.h"
#include "../Engine.h"
#include "../Graphics/Model/ModelLoader.h"
#include "../Graphics/Mesh/MeshLib.h"


Scene::Scene(Engine* engine) : _engine(engine) {}

Scene::~Scene() {
    for (auto* obj : _objects)
        delete obj;
}

GameObject* Scene::createObject() {
    GameObject* obj = new GameObject();
    obj->setContext(this, _engine);
    _objects.push_back(obj);
    return obj;
}

void Scene::addLight(const PointLight& light) {
    _lights.push_back(light);
}

void Scene::setSkyLight(const SkyLight& light) {
    _skyLight = light;
}

void Scene::updateTime(RenderContext& ctx) {
    float t = _engine->time();

    ctx.shaderManager().updateConstantVS("smShader", "smCB", "time", &t);
    ctx.shaderManager().updateConstantVS("smiShader", "smCB", "time", &t);
}

void Scene::uploadLights(RenderContext& ctx) {
    const int MAX_POINT_LIGHTS = 10;
    auto& shaderManager = ctx.shaderManager();
    // Sky Light
    Vec3 skyColor = _skyLight.color;
    float skyIntensity = _skyLight.intensity;

    shaderManager.updateConstantPS("smShader", "smCB", "skyLightColor", &skyColor);
    shaderManager.updateConstantPS("smShader", "smCB", "skyLightIntensity", &skyIntensity);

    shaderManager.updateConstantPS("smiShader", "smCB", "skyLightColor", &skyColor);
    shaderManager.updateConstantPS("smiShader", "smCB", "skyLightIntensity", &skyIntensity);

    shaderManager.updateConstantPS("amShader", "amCB", "skyLightColor", &skyColor);
    shaderManager.updateConstantPS("amShader", "amCB", "skyLightIntensity", &skyIntensity);

    // Point Light
    struct Align16 { float x, y, z, w; };
    Align16 lightPosWS[MAX_POINT_LIGHTS] = {};
    Align16 lightColor[MAX_POINT_LIGHTS] = {};
    Align16 lightParams[MAX_POINT_LIGHTS] = {};

    int count = _lights.size() > MAX_POINT_LIGHTS ? MAX_POINT_LIGHTS : _lights.size();

    for (int i = 0; i < count; ++i) {
        const PointLight& light = _lights[i];

        lightPosWS[i] = { light.position.x, light.position.y, light.position.z, 1};
        lightColor[i] = { light.color.x, light.color.y, light.color.z, 1 };
        lightParams[i] = { light.range, light.intensity, 0, 0 };
    }

    shaderManager.updateConstantPS("smShader", "smCB", "pointLightCount", &count);
    shaderManager.updateConstantPS("smShader", "smCB", "lightPosWS", lightPosWS);
    shaderManager.updateConstantPS("smShader", "smCB", "lightParams", lightParams);
    shaderManager.updateConstantPS("smShader", "smCB", "lightColor", lightColor);

    shaderManager.updateConstantPS("smiShader", "smCB", "pointLightCount", &count);
    shaderManager.updateConstantPS("smiShader", "smCB", "lightPosWS", lightPosWS);
    shaderManager.updateConstantPS("smiShader", "smCB", "lightParams", lightParams);
    shaderManager.updateConstantPS("smiShader", "smCB", "lightColor", lightColor);

    shaderManager.updateConstantPS("amShader", "amCB", "pointLightCount", &count);
    shaderManager.updateConstantPS("amShader", "amCB", "lightPosWS", lightPosWS);
    shaderManager.updateConstantPS("amShader", "amCB", "lightParams", lightParams);
    shaderManager.updateConstantPS("amShader", "amCB", "lightColor", lightColor);
}

void Scene::update(float dt) {
    for (auto& obj : _objects)
        obj->update(dt);
}

void Scene::render(RenderContext& renderContext) {
    updateTime(renderContext);
    uploadLights(renderContext);

    // Render Instanced Mesh
    _staticMeshRenderPass.render(*this, renderContext);

    renderLayer(renderContext, RenderLayer::Sky);
    renderLayer(renderContext, RenderLayer::World);
    renderLayer(renderContext, RenderLayer::FPS);

    if (_engine->getShowCollisionBoxes()) {
        renderLayer(renderContext, RenderLayer::DEBUG);
    }
}

void Scene::renderLayer(RenderContext& renderContext, RenderLayer layer) {
    for (auto* obj : _objects) {
        for (auto* comp : obj->components()) {
            if (auto* render = dynamic_cast<Renderable*>(comp)) {
                if (render->layer() == layer)
                    render->onRender(renderContext);
            }
        }
    }
}

void Scene::setMainCamera(CameraComponent* cam) {
    _mainCamera = cam;
}

const std::vector<GameObject*>& Scene::objects() const {
    return _objects;
}

CameraComponent* Scene::mainCamera() {
    return _mainCamera;
}

void Scene::addStaticMeshInstance(ModelData* model, const InstanceData& world) {
    for (auto& inst : _staticMeshInstances) {
        if (inst.model == model) {
            inst.worlds.push_back(world);
            return;
        }
    }

    StaticMeshInstance inst;
    inst.model = model;
    inst.worlds.push_back(world);
    auto& renderContext = _engine->renderContext();

    for (auto& subMesh : model->subMeshes) {
        inst.materials.push_back(
            renderContext.materialManager().createInstance(renderContext, subMesh)
        );
    }
    _staticMeshInstances.push_back(std::move(inst));
}