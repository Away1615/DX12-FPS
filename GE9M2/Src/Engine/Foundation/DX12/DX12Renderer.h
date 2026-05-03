#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "DX12Resources.h"
#include "DX12RootSignature.h"
#include "../../Graphics/PSOManager.h"
#include "../../Graphics/Shader/ShaderManager.h"
#include "../../Graphics/Texture/TextureManager.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

class DX12FrameResource {

private:
    ComPtr<ID3D12CommandAllocator>          _allocator;
    ComPtr<ID3D12GraphicsCommandList4>      _commandList;
    DX12Fence                               _fence;

public:

    void create(ID3D12Device5* device) {

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));

        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&_commandList));

        _fence.create(device);
    }

    void reset() {
        _fence.wait();
        _allocator->Reset();
        _commandList->Reset(_allocator.Get(), NULL);
    }

    void exec(ID3D12CommandQueue* queue) {
        _commandList->Close();
        ID3D12CommandList* lists[] = { _commandList.Get() };
        queue->ExecuteCommandLists(1, lists);
        _fence.signal(queue);
    }

    ID3D12GraphicsCommandList4* commandList() {
        return _commandList.Get();
    }

    ID3D12CommandAllocator* allocator() {
        return _allocator.Get();
    }

    DX12Fence& fence() {
        return _fence;
    }

    void flushQueue(ID3D12CommandQueue* queue) {
        _fence.signal(queue);
        _fence.wait();
    }

public:
    DX12FrameResource() = default;

    DX12FrameResource(const DX12FrameResource&) = delete;
    DX12FrameResource& operator=(const DX12FrameResource&) = delete;

    DX12FrameResource(DX12FrameResource&&) = default;
    DX12FrameResource& operator=(DX12FrameResource&&) = default;
};

class DX12Renderer {

private:
    ComPtr<ID3D12Device5>           _device;
    ComPtr<ID3D12CommandQueue>      _graphicsQueue;

    DX12Swapchain*                  _swapchain         = nullptr;
    DX12RenderTargets*              _targets           = nullptr;
    DX12CBVSRVUAVHeap*              _srvHeap           = nullptr;
    DX12RootSignature*              _rootSignature     = nullptr;
    ShaderManager*		            _shaderManager     = nullptr;
	PSOManager*                     _psoManager        = nullptr;
    TextureManager*                 _textureManager    = nullptr;

    D3D12_VIEWPORT                  _viewport          = {};
    D3D12_RECT                      _scissor           = {};

    std::vector<DX12FrameResource>  _frameResources;

public:

    void create(
        ID3D12Device5* device,
        ID3D12CommandQueue* graphicsQueue,
        DX12Swapchain& swapchain,
        DX12RenderTargets& targets,
        DX12RootSignature& rootSignature,
        DX12CBVSRVUAVHeap& srvHeap,
        ShaderManager& shaderManager,
        PSOManager& psoManager,
        TextureManager& textureManager,
        int width,
        int height
    ) {
        _device = device;
        _graphicsQueue = graphicsQueue;

        _swapchain = &swapchain;
        _targets = &targets;
        _rootSignature = &rootSignature;
        _srvHeap = &srvHeap;
		_shaderManager = &shaderManager;
        _psoManager = &psoManager;
		_textureManager = &textureManager;

        _viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
        _scissor = { 0, 0, width, height };

        _frameResources.resize(_swapchain->bufferCount());
        for (auto& frameResource : _frameResources) {
            frameResource.create(_device.Get());
        }
    }

    void beginFrame() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Fence& fence = frameResource.fence();
        ID3D12GraphicsCommandList4* commandList = frameResource.commandList();
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = _targets->rtvHandle(frameIndex);
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = _targets->dsvHandle();

        frameResource.reset();

        DX12Barrier::add(
            _targets->backBufferResource(frameIndex),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            commandList
        );

        float color[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
        commandList->ClearRenderTargetView(_targets->rtvHandle(frameIndex), color, 0, nullptr);
        commandList->ClearDepthStencilView(_targets->dsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    
        ID3D12DescriptorHeap* heaps[] = { _srvHeap->heap.Get() };
        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_scissor);
        commandList->SetDescriptorHeaps(1, heaps);
        commandList->SetGraphicsRootSignature(_rootSignature->rootSignature());
    }

    void endFrame() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        DX12FrameResource& frameResource = _frameResources[frameIndex];

        DX12Barrier::add(
            _targets->backBufferResource(frameIndex),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            frameResource.commandList()
        );

        frameResource.exec(_graphicsQueue.Get());
        _swapchain->present();
    }

    ID3D12GraphicsCommandList4* commandList() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        return _frameResources[frameIndex].commandList();
    }

    void flushGraphicsQueue() {
        UINT frameIndex = _swapchain->getCurrentBackBufferIndex();
        _frameResources[frameIndex].flushQueue(_graphicsQueue.Get());
    }

public:

    DX12Renderer() = default;
    
    DX12Renderer(const DX12Renderer&) = delete;
    DX12Renderer& operator=(const DX12Renderer&) = delete;
    
    DX12Renderer(DX12Renderer&&) = default;
    DX12Renderer& operator=(DX12Renderer&&) = default;
};
