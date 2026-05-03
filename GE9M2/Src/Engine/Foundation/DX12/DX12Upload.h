#pragma once
#include <d3d12.h>
#include <cassert>
#include <wrl/client.h>
#include "DX12CommandQueue.h"

using Microsoft::WRL::ComPtr;

class DX12Barrier {

public:
    static void add(ID3D12Resource* res,
        D3D12_RESOURCE_STATES first,
        D3D12_RESOURCE_STATES second,
        ID3D12GraphicsCommandList4* commandList) {

        D3D12_RESOURCE_BARRIER rb = {};
        rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        rb.Transition.pResource = res;
        rb.Transition.StateBefore = first;
        rb.Transition.StateAfter = second;
        rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &rb);
    }

public:

    DX12Barrier() = default;

    DX12Barrier(const DX12Barrier&) = delete;
    DX12Barrier& operator=(const DX12Barrier&) = delete;

    DX12Barrier(DX12Barrier&&) = default;
    DX12Barrier& operator=(DX12Barrier&&) = default;
};

class DX12Upload {
private:
    ComPtr<ID3D12Device5>                   _device;
    ComPtr<ID3D12CommandQueue>              _queue;
    ComPtr<ID3D12CommandAllocator>          _allocator;
    ComPtr<ID3D12GraphicsCommandList4>      _commandList;

    DX12Fence                               _fence;

public:

    void create(ID3D12Device5* device, ID3D12CommandQueue* queue) {
        _device = device;
        _queue = queue;

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_allocator));

        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&_commandList));

        _commandList->Close();
        _fence.create(device);
    }

    // Upload a generic buffer
    void uploadResource(
        ID3D12Resource* dstResource,
        const void* data,
        UINT size,
        D3D12_RESOURCE_STATES targetState,
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT* texFootprint = nullptr
    ) {
        ComPtr<ID3D12Resource> uploadBuffer;
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = size;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        HRESULT hr = _device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadBuffer)
        );

        if (FAILED(hr)) {
            assert(false);
        }

        void* mappeddata = nullptr;
        uploadBuffer->Map(0, nullptr, &mappeddata);
        memcpy(mappeddata, data, size);
        uploadBuffer->Unmap(0, nullptr);

        resetCommandList();

        if (texFootprint != NULL) {
            D3D12_TEXTURE_COPY_LOCATION src = {};
            src.pResource = uploadBuffer.Get();
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = *texFootprint;
            D3D12_TEXTURE_COPY_LOCATION dst = {};
            dst.pResource = dstResource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = 0;
            _commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
        }
        else {
            if (!uploadBuffer) return;
            _commandList->CopyBufferRegion(dstResource, 0, uploadBuffer.Get(), 0, size);
        }

        DX12Barrier::add(
            dstResource,
            D3D12_RESOURCE_STATE_COPY_DEST,
            targetState,
            _commandList.Get()
        );

        runCommandList();

        flushQueue();
    }

private:
    void resetCommandList() {
        _allocator->Reset();
        _commandList->Reset(_allocator.Get(), nullptr);
    }

    void runCommandList() {
        _commandList->Close();
        ID3D12CommandList* lists[] = { _commandList.Get() };
        _queue->ExecuteCommandLists(1, lists);
    }

    // though 
    void flushQueue() {
        _fence.signal(_queue.Get());
        _fence.wait();
    }

public:
    DX12Upload() = default;

    DX12Upload(const DX12Upload&) = delete;
    DX12Upload& operator=(const DX12Upload&) = delete;

    DX12Upload(DX12Upload&&) = default;
    DX12Upload& operator=(DX12Upload&&) = default;
};
