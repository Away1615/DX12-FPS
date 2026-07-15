#pragma once
#include <map>
#include <iostream>
#include <memory>
#include "Texture.h"

class TextureManager {

public:
	std::map<std::string, std::unique_ptr<Texture>> textures;

	TextureManager() {}

	Texture* loadTexture(
		ID3D12Device5* device,
		DX12Upload& uploader,
		DX12CBVSRVUAVHeap& srvHeap,
		const std::string& name,
		const std::string& file,
		TextureUsage usage
	) {
		auto it = textures.find(name);
		if (it != textures.end()) {
			return it->second.get();
		}
			
		auto ownedTexture = std::make_unique<Texture>();
		Texture* t = ownedTexture.get();
		t->init(device, uploader, srvHeap, file, usage);
		textures[name] = std::move(ownedTexture);
		assert(t);
		return t;
	}

	int find(const std::string& name) {
		auto it = textures.find(name);
		if (it == textures.end()) {
			return find("__default");
		}
		return it->second->heapOffset;
	}
};

