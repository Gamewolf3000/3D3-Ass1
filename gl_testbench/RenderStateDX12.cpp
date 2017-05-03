#include "RenderStateDX12.h"
#include "RendererDX12.h"

RenderStateDX12::RenderStateDX12(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, RendererDX12* renderer)
{
	/*Not used*/
	_device = device;
	_commandList = commandList;
	_renderer = renderer;
}

RenderStateDX12::~RenderStateDX12()
{
}

void RenderStateDX12::setWireFrame(bool global)
{
	_wireframe = global;
}

void RenderStateDX12::set()
{
	_renderer->setWireFrame(_wireframe);
}
