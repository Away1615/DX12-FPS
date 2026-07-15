#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "../Model/ModelData.h"
#include "Material.h"
#include "../RenderContext.h"

class Material;

class MaterialManager {
	std::map<std::string, std::unique_ptr<Material>> materials;
	std::vector<std::unique_ptr<Material>> instances;

public:
	Material* add(const std::string& name, std::unique_ptr<Material> material) {
		Material* result = material.get();
		auto [it, inserted] = materials.emplace(name, std::move(material));
		assert(inserted && "Material already exists");
		return inserted ? result : nullptr;
	}

	Material* find(const std::string& name) {
		auto it = materials.find(name);

		if (it != materials.end()) {
			return it->second.get();
		}
		else {
			return nullptr;
		}
	}

	Material* createInstance(RenderContext& ctx, const SubMesh& subMesh) {

		// Select material from Cache As templete
		Material* base = find(subMesh.materialKey);

		// One mesh match one material
		auto ownedInstance = std::make_unique<Material>(*base);
		Material* instance = ownedInstance.get();

		// apply the model texture
		if (!subMesh.albedoTex.empty()) {
			instance->addTexture("albedoTex", subMesh.albedoTex);
		}

		if (!subMesh.normalTex.empty()) {
			instance->addTexture("normalTex", subMesh.normalTex);
		}

		instances.push_back(std::move(ownedInstance));
		return instance;
	}
};
