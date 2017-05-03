#include "Texture2DDX12.h"
#include "CD3DX12Helper.h"
#include "RendererDX12.h"
#include <string>
#include <iostream>
Texture2DDX12::Texture2DDX12(ID3D12Device* devicePtr, RendererDX12* daBoss)
{
	this->devicePtr = devicePtr;
	this->renderer = daBoss;
	this->textureResource = nullptr;
	this->textureResource2 = nullptr;
}

Texture2DDX12::~Texture2DDX12()
{
	if(sampler)
		delete sampler;
	textureResource->Release();
	textureResource2->Release();
	textureHeap->Release();

}

int Texture2DDX12::loadFromFile(std::string filename)
{
	HRESULT hr;
	std::wstring convertedString = std::wstring(filename.begin(), filename.end());
	DirectX::LoadWICTextureFromFile(devicePtr, convertedString.c_str(), &textureResource, decodedData, subresource);
	textureResource->SetName(L"Texture wic");

	//sizeOfTexture = GetRequiredIntermediateSize(textureResource, 0, 1);

	//for (int i = 0; i < sizeOfTexture; i++)
	//{
	//	
	//	auto j = (char*)subresource.pData;
	//	std::cout << std::to_string((uint8_t)j[i] )<< std::endl;
	//}


	
	//hr = DirectX::CreateDDSTextureFromFile12(devicePtr, renderer->commandList, L"Green.dds", textureCom, textureHeapCom);
	//textureResource = textureCom.Get();
	//textureHeapCom = textureHeapCom.Get();
	//textureResource->SetName(L"Texture dds");
	

	sizeOfTexture = GetRequiredIntermediateSize(textureResource, 0, 1);
	desc = D3D12_SHADER_RESOURCE_VIEW_DESC() = { };
	desc.Format = textureResource->GetDesc().Format;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = textureResource->GetDesc().MipLevels;
	desc.Texture2D.ResourceMinLODClamp = 0.0f;

	//----------------------------------------------------------------------------------------------------

	D3D12_RESOURCE_DESC texDesc = textureResource->GetDesc();
	/*ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = width;
	texDesc.Height = (uint32_t)height;
	texDesc.DepthOrArraySize = (depth > 1) ? (uint16_t)depth : (uint16_t)arraySize;
	texDesc.MipLevels = (uint16_t)mipCount;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;*/

	hr = devicePtr->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&textureResource2)
	);

	if (FAILED(hr))
	{
		// THINGS GONE HORRIBLY WRONG, PLEASE SEND HELP
	}
	else
	{
		const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource2, 0, num2DSubresources);

		hr = devicePtr->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureHeap));
		if (FAILED(hr))
		{
			//SOMETHING WENT HORRIBLY WRONG HERE AS WELL, PLEASE SEND MORE HELP, OR DONUTS AT LEAST
		}
		else
		{
			renderer->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureResource2,
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

			// Use Heap-allocating UpdateSubresources implementation for variable number of subresources (which is the case for textures).
			UpdateSubresources(renderer->commandList, textureResource2, textureHeap, 0, 0, num2DSubresources, &subresource);

			renderer->commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureResource2,
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
			}
	}
	
	//----------------------------------------------------------------------------------------------------

	
	
	textureResource2->SetName(L"Texture wic");
	renderer->CreateTextureHeap(this);

	return 0;
}

void Texture2DDX12::bind(unsigned int slot)
{
	renderer->UploadTexture(this);
}


