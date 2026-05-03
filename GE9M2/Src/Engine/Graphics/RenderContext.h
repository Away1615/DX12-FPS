#pragma once
#include "PSOManager.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureManager.h"
#include "Texture/Texture.h"
#include "../Foundation/DX12/DX12Device.h"
#include "../Foundation/DX12/DX12CommandQueue.h"
#include "../Foundation/DX12/DX12Resources.h"
#include "../Foundation/DX12/DX12RootSignature.h"
#include "../Foundation/DX12/DX12Renderer.h"
#include "Material/MaterialManager.h"
#include "Instancing/InstanceBuffer.h"

class RenderContext {
private:
    const int bufferCount = 2;
    const int srvHeapSize = 16384;

    float                   _aspectRatio = 0;
    DX12Device              _device;
    DX12CommandQueues       _queues;
    DX12Swapchain           _swapchain;
    DX12RenderTargets       _targets;
    DX12CBVSRVUAVHeap       _srvHeap;
    DX12RootSignature       _rootSignature;
    DX12Renderer            _renderer;
    DX12Upload              _uploader;
    InstanceBuffer          _instanceBuffer;

    PSOManager              _psoManager;
	ShaderManager		    _shaderManager;
	TextureManager          _textureManager;
    MaterialManager         _materialManager;

public:

	DX12Device& device() { return _device; }
    DX12Renderer& renderer() { return _renderer; }
    DX12CBVSRVUAVHeap& srvHeap() { return _srvHeap; }
    DX12Upload& uploader() { return _uploader; }
    DX12RootSignature& rootSignature() { return _rootSignature; }
    InstanceBuffer& instanceBuffer() { return _instanceBuffer; }
	ShaderManager& shaderManager() { return _shaderManager; }
	PSOManager& psoManager() { return _psoManager; }
	TextureManager& textureManager() { return _textureManager; }
    MaterialManager& materialManager() { return _materialManager; }

    float aspectRatio() { return _aspectRatio; }

    RenderContext(HWND hwnd, int width, int height) {

        _aspectRatio = (float)width / (float)height;

        _device.create();

        _queues.create(_device.dxDevice());

        _rootSignature.create(_device.dxDevice());

        _instanceBuffer.create(_device.dxDevice(), 100);
        _swapchain.create(_queues.graphicsQueue(), hwnd, width, height, bufferCount);

        _targets.create(_device.dxDevice(), _swapchain, width, height);

        _srvHeap.create(_device.dxDevice(), srvHeapSize);

        _uploader.create(_device.dxDevice(), _queues.graphicsQueue());

        _renderer.create(
            _device.dxDevice(),
            _queues.graphicsQueue(),
            _swapchain,
            _targets,
            _rootSignature,
            _srvHeap,
            _shaderManager,
            _psoManager,
            _textureManager,
            width,
            height
        );
        
        registerDefaultResource();
    }

public:
       
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&) = default;
    RenderContext& operator=(RenderContext&&) = default;

private:
    void registerDefaultResource() {
        // load default texture
        _textureManager.loadTexture(
            _device.dxDevice(),
            _uploader,
            _srvHeap,
            "__default",
            "Src/Assets/Textures/__default.png",
            TextureUsage::Color
        );

        Shader* debugShader = _shaderManager.loadShader(
            "debugShader",
            "Src/Assets/Shaders/debug_vs.hlsl",
            "Src/Assets/Shaders/debug_ps.hlsl",
            _device.dxDevice(),
            _rootSignature
        );

        PSOParam debugParam;
        debugParam.psoName = "debugLinePSO";
        debugParam.psBlob = debugShader->ps.Get();
        debugParam.vsBlob = debugShader->vs.Get();
        debugParam.depthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        debugParam.depthFunc = D3D12_COMPARISON_FUNC_LESS;
        debugParam.layout = DX12VertexLayoutCache::getStaticLayout();
        debugParam.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        _psoManager.createPSO(
            _device.dxDevice(),
            _rootSignature.rootSignature(),
            debugParam
        );
    }
};
