#pragma once

#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>
#include <string>

using Microsoft::WRL::ComPtr;

class DX12RootSignature {

private:
    ComPtr<ID3D12RootSignature>         _rootSignature;

public:

    int rpVS_CBV = -1;      // b0 (VS)
    int rpPS_CBV = -1;      // b0 (PS)
    int rpSRV_Albedo = -1;  // t0
    int rpSRV_Normal = -1;  // t1

    ID3D12RootSignature* rootSignature() const { return _rootSignature.Get(); }

    void create(ID3D12Device5* device) {
        std::vector<D3D12_ROOT_PARAMETER> params;

        // RootParam 0: VS CBV(b0)
        rpVS_CBV = params.size();
        D3D12_ROOT_PARAMETER rootCBVS{};
        rootCBVS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootCBVS.Descriptor.ShaderRegister = 0;
        rootCBVS.Descriptor.RegisterSpace = 0;
        rootCBVS.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        params.push_back(rootCBVS);

        // RootParam 1: PS CBV(b0)
        rpPS_CBV = params.size();
        D3D12_ROOT_PARAMETER rootCBPS{};
        rootCBPS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootCBPS.Descriptor.ShaderRegister = 0;
        rootCBPS.Descriptor.RegisterSpace = 0;
        rootCBPS.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params.push_back(rootCBPS);

        // RootParam 2: Albedo SRV(t0)
        rpSRV_Albedo = params.size();
        D3D12_DESCRIPTOR_RANGE albedoRange{};
        albedoRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        albedoRange.NumDescriptors = 1;
        albedoRange.BaseShaderRegister = 0; // t0
        albedoRange.RegisterSpace = 0;
        albedoRange.OffsetInDescriptorsFromTableStart =
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER rootAlbedoTex{};
        rootAlbedoTex.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootAlbedoTex.DescriptorTable.NumDescriptorRanges = 1;
        rootAlbedoTex.DescriptorTable.pDescriptorRanges = &albedoRange;
        rootAlbedoTex.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params.push_back(rootAlbedoTex);

        // RootParam 3: Normal SRV(t1)
        rpSRV_Normal = params.size();
        D3D12_DESCRIPTOR_RANGE normalRange{};
        normalRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        normalRange.NumDescriptors = 1;
        normalRange.BaseShaderRegister = 1; // t1
        normalRange.RegisterSpace = 0;
        normalRange.OffsetInDescriptorsFromTableStart =
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER rootNormalTex{};
        rootNormalTex.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootNormalTex.DescriptorTable.NumDescriptorRanges = 1;
        rootNormalTex.DescriptorTable.pDescriptorRanges = &normalRange;
        rootNormalTex.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        params.push_back(rootNormalTex);

        // Static Sampler (s0)
        D3D12_STATIC_SAMPLER_DESC staticSampler = {};
        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0;
        staticSampler.MaxAnisotropy = 1;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0;   // s0
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


        // Root Signature Desc
        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.NumParameters = params.size();
        desc.pParameters = params.data();
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        desc.NumStaticSamplers = 1;
        desc.pStaticSamplers = &staticSampler;

        // Serialize + Create
        ComPtr<ID3DBlob> serialized;
        ComPtr<ID3DBlob> errors;

        D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &serialized,
            &errors
        );

        device->CreateRootSignature(
            0,
            serialized->GetBufferPointer(),
            serialized->GetBufferSize(),
            IID_PPV_ARGS(&_rootSignature)
        );
    }

public:
    DX12RootSignature() = default;

    DX12RootSignature(const DX12RootSignature&) = delete;
    DX12RootSignature& operator=(const DX12RootSignature&) = delete;

    DX12RootSignature(DX12RootSignature&&) = default;
    DX12RootSignature& operator=(DX12RootSignature&&) = default;
};