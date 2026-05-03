#pragma once
#include <string>
#include <map>
#include "Shader.h"
#include <cassert>
#include "../../Foundation/DX12/DX12Resources.h"
#include "../../Foundation/DX12/DX12RootSignature.h"

class ShaderManager {
private:

public:
    std::map<std::string, Shader*> shaders;

    Shader* loadShader(
        const std::string& name,
        const std::string& vs,
        const std::string& ps,
        ID3D12Device5* device,
        DX12RootSignature& rootSignature
        ) {
        if (shaders.find(name) != shaders.end()) {
            return nullptr;
        }

        Shader* shader = new Shader();
        shader->load(device, vs, ps);
        shaders[name] = shader;
        auto& rootSig = rootSignature;

        shader->textureRootIndices["albedoTex"] = rootSig.rpSRV_Albedo;
        shader->textureRootIndices["normalTex"] = rootSig.rpSRV_Normal;
        return shader;
    }

    Shader* find(const std::string& name) {
        Shader* shader = shaders[name];
        assert(shader);
        return shader;
    }

    void apply(ID3D12GraphicsCommandList4* cmd, const std::string& name) {
        find(name)->apply(cmd);
    }

    void updateTexturePS(
        ID3D12GraphicsCommandList4* cmd,
        DX12CBVSRVUAVHeap& srvHeap,
        const std::string& shaderName,
        const std::string& slot,
        int heapOffset
    ) {

        UINT rootIndex = shaders[shaderName]->textureRootIndices[slot];

        D3D12_GPU_DESCRIPTOR_HANDLE handle = srvHeap.gpuHandle;
        handle.ptr += (UINT64)heapOffset * srvHeap.incrementSize;

        cmd->SetGraphicsRootDescriptorTable(rootIndex, handle);
    }

    void updateConstantVS(std::string shaderName, std::string cbName, std::string vName, void* data) {
        find(shaderName)->updateVS(cbName, vName, data);
    }

    void updateConstantPS(std::string shaderName, std::string cbName, std::string vName, void* data) {
        find(shaderName)->updatePS(cbName, vName, data);
    }

    ~ShaderManager() {
        for (auto& shaderPair : shaders) {
            delete shaderPair.second;
        }
    }
};
