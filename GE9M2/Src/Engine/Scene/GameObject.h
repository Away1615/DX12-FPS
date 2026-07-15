#pragma once
#include <vector>
#include <string>
#include <memory>

class Component;
class Scene;
class Engine;
class Transform;
class RenderContext;
class CameraComponent;

class GameObject {
protected:
    std::vector<std::unique_ptr<Component>> _components;
    std::unique_ptr<Transform> _transform;

    Scene* _scene = nullptr;
    Engine* _engine = nullptr;

    std::string _debugName;

public:

    GameObject();

    ~GameObject();

    void setContext(Scene* scene, Engine* engine);

    Transform& transform();
    const Transform& transform() const;
    Scene* scene() const;
    Engine* engine() const;
    CameraComponent* mainCamera();

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto ownedComponent = std::make_unique<T>(std::forward<Args>(args)...);
        T* component = ownedComponent.get();
        component->setOwner(this);
        _components.push_back(std::move(ownedComponent));

        component->onStart();
        return component;
    }

    template<typename T>
    T* getComponent() {
        for (const auto& component : _components) {
            if (auto result = dynamic_cast<T*>(component.get()))
                return result;
        }
        return nullptr;
    }

    void update(float dt);

    const std::vector<std::unique_ptr<Component>>& components() const;

    void setName(std::string name) { _debugName = name; }
};
