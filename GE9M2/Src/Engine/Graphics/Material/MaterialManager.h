#pragma once
#include <map>
#include <string>
#include "../Model/ModelData.h"
#include "Material.h"
#include "../RenderContext.h"

class Material;

class MaterialManager {
	std::map<std::string, Material*> materials;

public:
	bool add(const std::string& name, Material* material) {
		auto [it, inserted] = materials.emplace(name, material);
		assert(inserted && "Material already exists");
		return inserted;
	}

	Material* find(const std::string& name) {
		auto it = materials.find(name);

		if (it != materials.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	Material* createInstance(RenderContext& ctx, const SubMesh& subMesh) {

		// Select material from Cache As templete
		Material* base = find(subMesh.materialKey);

		// One mesh match one material
		Material* instance = new Material(*base);

		// apply the model texture
		if (!subMesh.albedoTex.empty()) {
			instance->addTexture("albedoTex", subMesh.albedoTex);
		}

		if (!subMesh.normalTex.empty()) {
			instance->addTexture("normalTex", subMesh.normalTex);
		}

		return instance;
	}
};