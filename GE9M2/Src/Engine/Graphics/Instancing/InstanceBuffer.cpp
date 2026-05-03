#include "InstanceBuffer.h"
#include <d3d12.h>
#include <cassert>

void InstanceBuffer::create(ID3D12Device* device, int maxCount) {
    assert(device);
    _count = 0;
    _maxCount = maxCount;

    D3D12_HEAP_PROPERTIES heap = {};
    heap.Type = D3D12_HEAP_TYPE_UPLOAD;

    int bufferSize = sizeof(InstanceData) * maxCount;

    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = bufferSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_buffer)
    );

    _vbv.BufferLocation = _buffer->GetGPUVirtualAddress();
    _vbv.StrideInBytes = sizeof(InstanceData);
    _vbv.SizeInBytes = desc.Width;
}

void InstanceBuffer::upload(const InstanceData* data, int count) {
    assert(_buffer);
    _count = count;
    if (count == 0 || count > _maxCount)
        return;

    void* mapped = nullptr;
    _buffer->Map(0, nullptr, &mapped);
    memcpy(mapped, data, sizeof(InstanceData) * count);
    _buffer->Unmap(0, nullptr);
}
