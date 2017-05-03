#include "VertexBufferDX12.h"

VertexBufferDX12::VertexBufferDX12(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12Resource* bufferResource)
{
	_device = device;
	_commandList = commandList;
	_bufferView.BufferLocation = bufferResource->GetGPUVirtualAddress();
	data = nullptr;
}

VertexBufferDX12::~VertexBufferDX12()
{
	delete data;
}

void VertexBufferDX12::setData(const void * data, size_t size, DATA_USAGE usage)
{
	if (!_device)
		return; /*Det ska vara en throw där egentligen*/
	if (this->data)
		delete data;

	this->data = new char[size];
	//Copy the triangle data to the vertex buffer.
	D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	//_bufferResource->Map(0, &range, &dataBegin); /* Ska läggas in i renderer*/
	memcpy(this->data, data, size);
	//_bufferResource->Unmap(0, nullptr); /*Ska läggas in i renderer*/

	//Initialize vertex buffer view, used in the render call.
	_bufferView.StrideInBytes = size / 3; /*size is a float4[3], but we need the size of the individual elements, thus divide by 3*/
	_bufferView.SizeInBytes = size;

	totalSize = size;
	
}

void VertexBufferDX12::bind(size_t offset, size_t size, unsigned int location)
{
	
	//_commandList->IASetVertexBuffers(offset, 1, &_bufferView);
}

void VertexBufferDX12::unbind()
{
	/*We don't? Bind over?*/

}

inline size_t VertexBufferDX12::getSize()
{
	return totalSize;
}
