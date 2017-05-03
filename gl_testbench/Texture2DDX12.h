#ifndef TEXTURE_2D_DX_12_H
#define TEXTURE_2D_DX_12_H

#include "Texture2D.h"
#include "DXTK12\WICTextureLoader.h"
#include "DDSTextureLoaderLuna.h"

class RendererDX12;

class Texture2DDX12 : public Texture2D
{
public:
	Texture2DDX12(ID3D12Device* devicePtr = nullptr, RendererDX12* daBoss = nullptr);
	~Texture2DDX12();
	// returns 0 if texture was loaded.
	int loadFromFile(std::string filename);

	// bind texture to be used in the pipeline, binding to
	// slot "slot" in the active fragment shader.
	// slot can have different interpretation depending on the API.
	void bind(unsigned int slot);

	// if no sampler is set here, a default sampler should be used.
	
	ID3D12Resource* textureResource = nullptr;
	ID3D12Resource* textureResource2 = nullptr;
	ID3D12Resource* textureHeap = nullptr;
	ID3D12Device* devicePtr = nullptr;
	RendererDX12* renderer = nullptr;

	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	UINT64 sizeOfTexture;
	std::unique_ptr<uint8_t[]> decodedData;
	D3D12_SUBRESOURCE_DATA subresource;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureCom = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureHeapCom = nullptr;

};






#endif
