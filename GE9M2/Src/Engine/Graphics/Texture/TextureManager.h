#pragma once
#include <map>
#include <iostream>
#include "Texture.h"

class TextureManager {

public:
	std::map<std::string, Texture*> textures;

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
			return it->second;
		}
			
		Texture* t = new Texture();
		t->init(device, uploader, srvHeap, file, usage);
		textures[name] = t;
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

	~TextureManager() {
		for (auto const& pair : textures) {
			delete pair.second;
		}
	}
};

