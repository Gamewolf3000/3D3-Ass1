#ifndef VERTEX_BUFFER_DX_12_H
#define VERTEX_BUFFER_DX_12_H

#include "VertexBuffer.h"

#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

class VertexBufferDX12 : public VertexBuffer
{
public:
	VertexBufferDX12(ID3D12Device* device = nullptr, ID3D12GraphicsCommandList* commandList = nullptr, ID3D12Resource* bufferResource = nullptr);

	~VertexBufferDX12();
	void setData(const void* data, size_t size, DATA_USAGE usage);
	void bind(size_t offset, size_t size, unsigned int location);
	void* getData() { return data; }
	void* getBufferView() { return &_bufferView; }
	void unbind();
	size_t getSize();

private:
	ID3D12Device* _device;
	ID3D12GraphicsCommandList* _commandList;
	D3D12_VERTEX_BUFFER_VIEW _bufferView;

	void* data;
	size_t totalSize;
	
};


#endif
