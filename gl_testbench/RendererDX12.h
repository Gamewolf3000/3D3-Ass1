#ifndef RENDERER_DX_12_H
#define RENDERER_DX_12_H

#include <windows.h>
#include <SDL.h>
#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "CD3DX12Helper.h"
#include "Renderer.h"


#define NUM_SWAP_BUFFERS 2U
#define NR_OF_OBJECTS 2000

template<class Interface>
inline void SafeRelease(
	Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}






class MaterialDX12;
class Texture2DDX12;

class RendererDX12: public Renderer
{
private:
	SDL_Window* window;

	UINT16 windowHeight;
	UINT16 windowWidth;

	ID3D12Device* device;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;
	
	IDXGISwapChain* swapChain;

	ID3D12Fence* fence;
	UINT64 fenceValue;
	HANDLE eventHandle;

	ID3D12DescriptorHeap* renderTargetsHeap;
	ID3D12DescriptorHeap* samplerHeap;
	ID3D12DescriptorHeap* textureHeap;
	void* textureBufferCPU_mappedPtr;
	D3D12_RESOURCE_DESC textureDesc;
	UINT renderTargetDescriptorSize;
	ID3D12Resource* renderTargets[NUM_SWAP_BUFFERS];
	ID3D12DescriptorHeap* depthStencileHeap = nullptr;
	ID3D12Resource* depthstencil = nullptr;
	ID3D12Resource* bufferResource = nullptr;
	ID3D12Resource* textureResource = nullptr;

	D3D12_VIEWPORT vp;
	D3D12_RECT scissorRect;

	ID3D12DescriptorHeap* CBDescriptorHeap;
	ID3D12Resource* constantBufferResource;
	void* constantBufferCPU_mappedPtr;
	ID3D12DescriptorHeap* DiffuseDescriptorHeap;
	ID3D12Resource* constantDiffuseBufferResource;
	void* constantDiffuseBufferCPU_mappedPtr;

	float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	int frameIndex = 0;

	std::vector<Mesh*> drawList;

	void InitializeWindow();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	void CreateDirect3DDevice();
	void CreateCommandInterfacesAndSwapChain();
	void CreateFenceAndEventHandle();
	void CreateRenderTargets();
	void CreateViewportAndScissorRect();
	void CreateRenderHeap();
	void CreateConstantBufferHeap();
	void CreateDepthStencil();

	void WaitForGPU();

	void SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
		D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	

public:
	RendererDX12();
	~RendererDX12();
	Material* makeMaterial();
	Mesh* makeMesh();
	VertexBuffer* makeVertexBuffer();
	Texture2D* makeTexture2D();
	Sampler2D* makeSampler2D();
	ResourceBinding* makeResourceBinding();
	RenderState* makeRenderState();
	std::string getShaderPath();
	std::string getShaderExtension();
	ConstantBuffer* makeConstantBuffer(std::string NAME,unsigned int location);
	MaterialDX12* currentMaterial;

	ID3D12GraphicsCommandList* commandList; // Vad är en "graphics"? Vad är det för speciellt med den?
	
	int initialize(unsigned int width=800,unsigned int height=600);
	void present();
	int shutdown();

	void setClearColor(float,float,float,float);
	void clearBuffer(unsigned int);
	void setRenderTarget(RenderTarget*rt);//completeparameters
														//canbepartiallyoverridenbyaspecificTechnique.
	void setRenderState(RenderState*ps);
	void setWireFrame(bool active);
	//submitwork(torender)totherenderer.
	void submit(Mesh*mesh);
	void frame();

	void CreateTextureHeap(Texture2DDX12* texture); /*DON'T ASK! WE DON'T LIKE THIS SOLUTION EITHER*/
	void UploadTexture(Texture2DDX12* texture);
};



#endif
