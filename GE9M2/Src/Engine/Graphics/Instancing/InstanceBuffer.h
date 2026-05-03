#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../../Foundation/Base/Maths.h"
#include "InstanceData.h"

using Microsoft::WRL::ComPtr;

class InstanceBuffer {
private:
    ComPtr<ID3D12Resource> _buffer;
    D3D12_VERTEX_BUFFER_VIEW _vbv = {};

    int _count = 0;
    int _maxCount = 0;
    ID3D12Device* _device = nullptr;

public:

    void create(ID3D12Device* device, int maxCount);
    void upload(const InstanceData* data, int count);

    const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView() const {
        return _vbv;
    }

    const int count() const {
        return _count;
    }


};
