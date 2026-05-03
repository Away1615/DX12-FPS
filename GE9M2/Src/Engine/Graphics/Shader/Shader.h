#pragma once
#include <string>
#include <vector>
#include <map>
#include <d3d12.h>
#include "ShaderCompiler.h"
#include <wrl/client.h>

#pragma comment(lib, "d3d12.lib")

using Microsoft::WRL::ComPtr;

struct ConstantBufferVariable {
public:
    UINT offset;
    UINT size;
};

class ConstantBuffer {

public:
    ComPtr<ID3D12Resource>       constantBuffer;
    unsigned char* mappedData = nullptr;
    std::string                  name;
    UINT                         cbSizeInBytes = 0;
    UINT                         offsetIndex = 0;
    UINT                         numInstances = 0;

    std::map<std::string, ConstantBufferVariable>   cbData;

    void create(ID3D12Device5* device, UINT sizeInBytes, UINT maxDrawCalls = 1024) {

        cbSizeInBytes = (sizeInBytes + 255) & ~255;
        unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
        numInstances = maxDrawCalls;
        offsetIndex = 0;

        D3D12_HEAP_PROPERTIES heapprops;
        memset(&heapprops, 0, sizeof(D3D12_HEAP_PROPERTIES));
        heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC cbDesc;
        memset(&cbDesc, 0, sizeof(D3D12_RESOURCE_DESC));
        cbDesc.Width = cbSizeInBytesAligned;
        cbDesc.Height = 1;
        cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        cbDesc.DepthOrArraySize = 1;
        cbDesc.MipLevels = 1;
        cbDesc.SampleDesc.Count = 1;
        cbDesc.SampleDesc.Quality = 0;
        cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        device->CreateCommittedResource(&heapprops,
            D3D12_HEAP_FLAG_NONE,
            &cbDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            NULL,
            IID_PPV_ARGS(&constantBuffer)
        );

        D3D12_RANGE readRange = { 0, 0 };
        constantBuffer->Map(0, &readRange, (void**)&mappedData);
    }

    void update(const std::string& name, void* data) {
        ConstantBufferVariable cbVariable = cbData[name];
        unsigned int offset = offsetIndex * cbSizeInBytes;
        memcpy(&mappedData[offset + cbVariable.offset], data, cbVariable.size);
    }

    D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const {
        return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
    }

    void next() {
        offsetIndex++;
        if (offsetIndex >= numInstances) {
            offsetIndex = 0;
        }
    }
};

class Shader {
public:
    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;

    ShaderCompiler compiler;

    std::vector<ConstantBuffer> vsCBs;
    std::vector<ConstantBuffer> psCBs;
    std::map<std::string, int> textureBindPoints;
    std::map<std::string, int> textureRootIndices;

    void load(ID3D12Device5* device, const std::string& vsPath, const std::string& psPath) {
        vs = compiler.compileOrLoad(vsPath, "VS", "vs_5_0");
        ps = compiler.compileOrLoad(psPath, "PS", "ps_5_0");

        reflect(device, vs.Get(), vsCBs);
        reflect(device, ps.Get(), psCBs);
    }

    void apply(ID3D12GraphicsCommandList4* cmd) {
        for (int i = 0; i < vsCBs.size(); i++) {
            cmd->SetGraphicsRootConstantBufferView(0, vsCBs[i].getGPUAddress());

            vsCBs[i].next();
        }
        for (int i = 0; i < psCBs.size(); i++) {
            cmd->SetGraphicsRootConstantBufferView(1, psCBs[i].getGPUAddress());

            psCBs[i].next();
        }
    }

    void updateVS(const std::string& cbName, const std::string& var, void* data) {
        update(cbName, var, data, vsCBs);
    }

    void updatePS(const std::string& cbName, const std::string& var, void* data) {
        update(cbName, var, data, psCBs);
    }


private:
    void reflect(ID3D12Device5* device, ID3DBlob* blob, std::vector<ConstantBuffer>& buffers) {

        ComPtr<ID3D12ShaderReflection> reflection;
        D3DReflect(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            IID_PPV_ARGS(&reflection)
        );

        D3D12_SHADER_DESC desc;
        reflection->GetDesc(&desc);

        for (UINT i = 0; i < desc.ConstantBuffers; i++) {
            auto* cbReflection = reflection->GetConstantBufferByIndex(i);

            D3D12_SHADER_BUFFER_DESC cbDesc;
            cbReflection->GetDesc(&cbDesc);

            ConstantBuffer cb;
            cb.name = cbDesc.Name;
            UINT totalSize = 0;

            for (UINT n = 0; n < cbDesc.Variables; ++n) {
                auto* var = cbReflection->GetVariableByIndex(n);
                D3D12_SHADER_VARIABLE_DESC vDesc;
                var->GetDesc(&vDesc);

                ConstantBufferVariable cbVariable;
                cbVariable.offset = vDesc.StartOffset;
                cbVariable.size = vDesc.Size;
                cb.cbData.insert({ vDesc.Name, cbVariable });
                totalSize += vDesc.Size;
            }

            cb.create(device, totalSize);
            buffers.push_back(cb);
        }

        for (UINT i = 0; i < desc.BoundResources; i++) {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            reflection->GetResourceBindingDesc(i, &bindDesc);
            if (bindDesc.Type == D3D_SIT_TEXTURE) {
                textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });
            }
        }
    }

    void update(const std::string& cbName, const std::string& var, void* data, std::vector<ConstantBuffer>& buffers) {
        for (auto& cb : buffers) {
            if (cb.name == cbName) {
                cb.update(var, data);
                return;
            }
        }
    }
};
