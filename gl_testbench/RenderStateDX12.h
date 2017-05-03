#ifndef RENDER_STATE_DX_12_H
#define RENDER_STATE_DX_12_H

#include "RenderState.h"

#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

class RendererDX12;

class RenderStateDX12 : public RenderState
{
public:
	RenderStateDX12(ID3D12Device* device = nullptr, ID3D12GraphicsCommandList* commandList = nullptr, RendererDX12* renderer = nullptr);
	~RenderStateDX12();

	void setWireFrame(bool);

	// activate all options in this render state.
	void set();

private:
	bool _wireframe;
	bool* globalWireFrame;

	ID3D12Device* _device = nullptr;
	ID3D12GraphicsCommandList* _commandList = nullptr;
	RendererDX12* _renderer;
};



#endif
