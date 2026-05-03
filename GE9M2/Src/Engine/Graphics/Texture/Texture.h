#pragma once
#include <d3d12.h>
#include <string>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class ID3D12Device5;
class DX12Upload;
class DX12CBVSRVUAVHeap;

enum class TextureUsage {
	Color,
	Data
};


class Texture {

public:
	ComPtr<ID3D12Resource> texResource;
	int heapOffset = -1;
	DXGI_FORMAT format;

	void init(
		ID3D12Device5* device,
		DX12Upload& uploader,
		DX12CBVSRVUAVHeap& srvHeap,
		const std::string& filename,
		TextureUsage usage);
};