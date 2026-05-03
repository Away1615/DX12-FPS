#pragma once
#include <vector>
#include "../Graphics/Material/Material.h"
#include "Components/Component.h"
#include "Light.h"
#include "../Graphics/Instancing/InstanceData.h"
#include "../Graphics/Render/StaticMeshRenderPass.h"

class GameObject;
class Engine;
class CameraComponent;
class RenderContext;

class Scene {
private:
    std::vector<GameObject*> _objects;
    std::vector<PointLight> _lights;
    std::vector<StaticMeshInstance> _staticMeshInstances;

    StaticMeshRenderPass _staticMeshRenderPass;
    SkyLight _skyLight;

    Engine* _engine = nullptr;
	CameraComponent* _mainCamera = nullptr;

public:

    Scene(Engine* engine);

    GameObject* createObject();

    void addLight(const PointLight& light);

    void setSkyLight(const SkyLight& light);

    void updateTime(RenderContext& ctx);

    void uploadLights(RenderContext& ctx);

    void update(float dt);

    void render(RenderContext& renderContext);

    void renderLayer(RenderContext& ctx, RenderLayer layer);

    void setMainCamera(CameraComponent* cam);

    const std::vector<GameObject*>& objects() const;

    CameraComponent* mainCamera();

    void addStaticMeshInstance(ModelData* model, const InstanceData& world);

    const std::vector<StaticMeshInstance>& staticMeshInstances() const {
        return _staticMeshInstances;
    }

    ~Scene();

};
