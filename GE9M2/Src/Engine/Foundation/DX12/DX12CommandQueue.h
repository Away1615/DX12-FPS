#pragma once
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// Only have one queue: GraphicesQueue
class DX12Fence {

public:
    ComPtr<ID3D12Fence>     fence;
    HANDLE                  eventHandle = nullptr;
    UINT64                  value = 0;

    void create(ID3D12Device5* device) {
        device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void signal(ID3D12CommandQueue* queue) {
        queue->Signal(fence.Get(), ++value);
    }

    void wait() {
        if (fence->GetCompletedValue() < value) {
            fence->SetEventOnCompletion(value, eventHandle);
            WaitForSingleObject(eventHandle, INFINITE);
        }
    }

    ~DX12Fence() {
        if (eventHandle)
            CloseHandle(eventHandle);
    }

public:

    DX12Fence() = default;

    DX12Fence(const DX12Fence&) = delete;
    DX12Fence& operator=(const DX12Fence&) = delete;

    DX12Fence(DX12Fence&&) = default;
    DX12Fence& operator=(DX12Fence&&) = default;

};

class DX12CommandQueues {

private:
    ComPtr<ID3D12CommandQueue>        _graphics;

    ComPtr<ID3D12CommandQueue>        _compute;

    ComPtr<ID3D12CommandQueue>        _copy;

public:

    ID3D12CommandQueue* graphicsQueue() { return _graphics.Get(); }
    ComPtr<ID3D12CommandQueue>& copyQueue() { return _copy; }
    ComPtr<ID3D12CommandQueue>& computeQueue() { return _compute; }

    void create(ID3D12Device5* device) {

        D3D12_COMMAND_QUEUE_DESC desc = {};

        // Graphics
        D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
        graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&_graphics));

        //// Copy
        D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
        copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&_copy));

        //// Compute
        D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
        computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&_compute));
    }

public:

    DX12CommandQueues() = default;

    DX12CommandQueues(const DX12CommandQueues&) = delete;
    DX12CommandQueues& operator=(const DX12CommandQueues&) = delete;

    DX12CommandQueues(DX12CommandQueues&&) = default;
    DX12CommandQueues& operator=(DX12CommandQueues&&) = default;

};