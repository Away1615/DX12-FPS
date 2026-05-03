#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "../../GameObject.h"
#include "../../../Engine.h"

using ComponentArgs = std::vector<std::string>;
using ComponentCreator = std::function<void(GameObject*, Engine&, const ComponentArgs&)>;

class ComponentFactory {

private:
    std::unordered_map<std::string, ComponentCreator> _creators;

public:
    static ComponentFactory& shared();

    void registerComponent(const std::string& name, ComponentCreator creator);
    bool create(const std::string& name, GameObject* obj, Engine& engine, const ComponentArgs& args);

    void registerAllComponents();

};
