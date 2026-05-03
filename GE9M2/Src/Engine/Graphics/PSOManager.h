#pragma once
#include <unordered_map>
#include <d3d12.h>
#include <string>
#include <wrl/client.h>
#include <cassert>
#include "../Foundation/DX12/DX12Resources.h"

using Microsoft::WRL::ComPtr;

class PSOParam {
public:
    PSOParam() = default;

    std::string psoName;

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;
    D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_NONE;

    bool depthEnable = true;
    D3D12_DEPTH_WRITE_MASK depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS;
    D3D12_INPUT_LAYOUT_DESC layout = DX12VertexLayoutCache::getStaticLayout();

    D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
};

class PSOManager {
private:
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> psos;

public:

    void createPSO(
        ID3D12Device5* device,
        ID3D12RootSignature* rootSignature,
        const PSOParam& param) {
        if (psos.find(param.psoName) != psos.end()) {
            return;
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
        desc.InputLayout = param.layout;
        desc.pRootSignature = rootSignature;
        desc.VS = { param.vsBlob->GetBufferPointer(), param.vsBlob->GetBufferSize() };
        desc.PS = { param.psBlob->GetBufferPointer(), param.psBlob->GetBufferSize() };

        // Rasterizer State
        D3D12_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.FillMode = param.fillMode;
        rasterDesc.CullMode = param.cullMode;
        rasterDesc.FrontCounterClockwise = FALSE;
        rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.MultisampleEnable = FALSE;
        rasterDesc.AntialiasedLineEnable = FALSE;
        rasterDesc.ForcedSampleCount = 0;
        rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        desc.RasterizerState = rasterDesc;

        // Depth Stencil State
        D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = param.depthEnable;
        depthStencilDesc.DepthWriteMask = param.depthWriteMask;
        depthStencilDesc.DepthFunc = param.depthFunc;
        depthStencilDesc.StencilEnable = FALSE;
        desc.DepthStencilState = depthStencilDesc;

        // Blend State
        D3D12_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlend = {
        FALSE, FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
        };

        for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++) {
            blendDesc.RenderTarget[i] = defaultRenderTargetBlend;
        }
        desc.BlendState = blendDesc;

        // Render Target State + Topology
        desc.SampleMask = UINT_MAX;
        desc.PrimitiveTopologyType = param.topologyType;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        desc.SampleDesc.Count = 1;

        // Create Pipeline State Object
        ComPtr<ID3D12PipelineState> pso;
        device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
        psos.insert({ param.psoName, pso });
    }

    ID3D12PipelineState* find(const std::string& name) {
        ComPtr<ID3D12PipelineState> pso = psos[name];
        assert(pso.Get());
        return pso.Get();
    }


    void bind(ID3D12GraphicsCommandList4* cmd, const std::string& psoName) {
        cmd->SetPipelineState(find(psoName));
    }
};