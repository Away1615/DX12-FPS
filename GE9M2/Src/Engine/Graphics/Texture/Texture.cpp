#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"
#include <d3d12.h>
#include <string>
#include <wrl/client.h>
#include "../RenderContext.h"
#include "../../../../Third_Party/stb_image.h"

using Microsoft::WRL::ComPtr;

void Texture::init(
	ID3D12Device5* device,
	DX12Upload& uploader,
	DX12CBVSRVUAVHeap& srvHeap,
	const std::string& filename,
	TextureUsage usage
) {
	int width = 0;
	int height = 0;
	int channels = 0;

	format = (usage == TextureUsage::Color)
		? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
		: DXGI_FORMAT_R8G8B8A8_UNORM;

	unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 4);
	channels = 4;

	// Create GPU Texture
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&texResource)
	);

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 totalBytes = 0;

	device->GetCopyableFootprints(
		&textureDesc,
		0, 1, 0,
		&footprint,
		nullptr, nullptr, &totalBytes
	);

	UINT rowPitch = footprint.Footprint.RowPitch;
	UINT srcRowSize = width * channels;

	unsigned char* uploadData = new unsigned char[rowPitch * height];

	for (UINT y = 0; y < height; y++) {
		memcpy(uploadData + y * rowPitch, texels + y * srcRowSize, srcRowSize);
	}

	// upload to GPU
	uploader.uploadResource(
		texResource.Get(),
		uploadData,
		rowPitch * height,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&footprint
	);

	D3D12_CPU_DESCRIPTOR_HANDLE h = srvHeap.getNextCPUHandle();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texResource.Get(), &srvDesc, h);

	heapOffset = srvHeap.used - 1;

	delete[] uploadData;

	stbi_image_free(texels);
}
