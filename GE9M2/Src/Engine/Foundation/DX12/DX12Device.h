#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX12Device {
private:
    ComPtr<IDXGIAdapter1>       _adapter;
    ComPtr<ID3D12Device5>       _device;

public:

    void create() {

        ComPtr<IDXGIAdapter1> adapterf;
        std::vector<ComPtr<IDXGIAdapter1>> adapters;
        ComPtr<IDXGIFactory4> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        int i = 0;
        while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND) {
            adapters.push_back(adapterf);
            i++;
        }

        // Find the best adapter
        long long maxVideoMemory = 0;
        int bestIndex = 0;

        for (int i = 0; i < (int) adapters.size(); ++i) {
            DXGI_ADAPTER_DESC desc;
            adapters[i]->GetDesc(&desc);

            if (desc.DedicatedVideoMemory > maxVideoMemory) {
                maxVideoMemory = desc.DedicatedVideoMemory;
                bestIndex = i;
            }
        }

        _adapter = adapters[bestIndex];

        D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&_device));
		adapters.clear();
    }

    ID3D12Device5* dxDevice() {
        return _device.Get();
    }

public:
    DX12Device() = default;

    DX12Device(const DX12Device&) = delete;
    DX12Device& operator=(const DX12Device&) = delete;

    DX12Device(DX12Device&&) = default;
    DX12Device& operator=(DX12Device&&) = default;
};
