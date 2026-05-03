#pragma once
#include <string>
#include <vector>
#include "../../Engine/Foundation/Base/Transform.h"

class Scene;
class Engine;
class GameObject;
class ModelData;
class Material;

enum class BlockType {
    None,
    Texture,
    Shader,
    PSO,
    Material,
    Light,
    Scene,
    Object,
    StaticInstances,
    StaticMesh
};

struct BlockContext {
    BlockType type = BlockType::None;

    std::string name;

    // scene / object
    GameObject* object = nullptr;

    // static mesh instancing
    ModelData* model = nullptr;

    Material* material = nullptr;
};

class LevelLoader {

public:
    void load(const std::string& path, Scene& scene, Engine& engine);

private:
    std::string decodeArg(const std::string& in);

    Transform getTransform(std::string line);

    void openBlock(const std::string& line, std::vector<BlockContext>& stack, Scene& scene, Engine& engine);

    void parseContent(const std::string& line, std::vector<BlockContext>& stack, Scene& scene, Engine& engine);
    std::string firstToken(const std::string& line);
};