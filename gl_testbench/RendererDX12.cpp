#include "RendererDX12.h"

#include <SDL_syswm.h>
/*

	DX12 (own) includes

*/
#define SIZE_OF_HEAP sizeof(float)*4*3*2 + sizeof(float)*2*3 //Size of float4[3] (2, normal and position), float2[3] (UV)

#include "ConstantBufferDX12.h"
#include "MaterialDX12.h"
#include "MeshDX12.h"
#include "RenderStateDX12.h"
#include "ResourceBindingDX12.h"
#include "Sampler2DDX12.h"
#include "Texture2DDX12.h"
#include "TransformDX12.h"
#include "VertexBufferDX12.h"



RendererDX12::RendererDX12()
{
	/*Denna funktion är gjord men ska inte ha något i sig. Per defintion har jag gjort en funktion nu.*/
	CoInitialize(NULL);
}

RendererDX12::~RendererDX12()
{
	shutdown();
}

Material * RendererDX12::makeMaterial()
{
	return new MaterialDX12(device, commandList);
}

Mesh * RendererDX12::makeMesh()
{
	return new MeshDX12();
}

VertexBuffer * RendererDX12::makeVertexBuffer()
{
	return new VertexBufferDX12(device, commandList, bufferResource);
}

Texture2D * RendererDX12::makeTexture2D()
{
	return new Texture2DDX12(device, this);
}

Sampler2D * RendererDX12::makeSampler2D()
{
	return new Sampler2DDX12();
}

ResourceBinding * RendererDX12::makeResourceBinding()
{
	return new ResourceBindingDX12();
}

RenderState * RendererDX12::makeRenderState()
{
	auto rs = new RenderStateDX12(device, commandList, this);
	rs->setWireFrame(false);
	return rs;
}

std::string RendererDX12::getShaderPath()
{
	return std::string("..\\assets\\DX12\\");
}

std::string RendererDX12::getShaderExtension()
{
	return std::string(".hlsl");
}

ConstantBuffer * RendererDX12::makeConstantBuffer(std::string NAME, unsigned int location)
{
	return new ConstantBufferDX12(NAME, location);
}

void RendererDX12::InitializeWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("DirectX12 > OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	
	
}

LRESULT CALLBACK RendererDX12::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void RendererDX12::CreateDirect3DDevice()
{
#ifdef _DEBUG
	//Enable the D3D12 debug layer.
	ID3D12Debug* debugController = nullptr;

#ifdef STATIC_LINK_DEBUGSTUFF
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	SafeRelease(debugController);
#else
	HMODULE mD3D12 = GetModuleHandle(L"D3D12.dll");
	PFN_D3D12_GET_DEBUG_INTERFACE f = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(mD3D12, "D3D12GetDebugInterface");
	if (SUCCEEDED(f(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
	SafeRelease(&debugController);
#endif
#endif

	//dxgi1_5 is only needed for the initialization process using the adapter.
	IDXGIFactory4* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	//First a factory is created to iterate through the adapters available.

	HRESULT hr;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));
	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
		{
			break; //No more adapters to enumerate.
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_1, __uuidof(ID3D12Device), nullptr)))
		{
			break;
		}

		SafeRelease(&adapter);
	}
	if (adapter)
	{
		//Create the actual device.
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&device));

		SafeRelease(&adapter);
	}
	else
	{

		//Create warp device if no adapter was found.
		factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	}

	


	SafeRelease(&factory);
}

void RendererDX12::CreateCommandInterfacesAndSwapChain()
{

	//Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC cqd = {};
	device->CreateCommandQueue(&cqd, IID_PPV_ARGS(&commandQueue));

	//Create command allocator. The command allocator object corresponds
	//to the underlying allocations in which GPU commands are stored.
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

	//Create command list.
	device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator,
		nullptr,
		IID_PPV_ARGS(&commandList));

	//Command lists are created in the recording state. Since there is nothing to
	//record right now and the main loop expects it to be closed, we close it.
	commandList->Close();

	IDXGIFactory4*	factory = nullptr;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));

	

	//Create swap chain.
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = 0;
	scDesc.Height = 0;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo = FALSE;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = NUM_SWAP_BUFFERS;
	scDesc.Scaling = DXGI_SCALING_NONE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	SDL_SysWMinfo HWNDFetcher;
	SDL_VERSION(&HWNDFetcher.version);
	SDL_GetWindowWMInfo(window, &HWNDFetcher);

	HRESULT hr = factory->CreateSwapChainForHwnd(
		commandQueue,
		HWNDFetcher.info.win.window,
		&scDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(&swapChain)
	);

	SafeRelease(&factory);
}

void RendererDX12::CreateFenceAndEventHandle()
{
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	fenceValue = 1;
	//Create an event handle to use for GPU synchronization.
	eventHandle = CreateEvent(0, false, false, 0);
}

void RendererDX12::CreateRenderTargets()
{
	//Create descriptor heap for render target views.
	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.NumDescriptors = NUM_SWAP_BUFFERS;
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	HRESULT hr = device->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&renderTargetsHeap));

	//Create resources for the render targets.
	renderTargetDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();

	//One RTV for each frame.
	for (UINT n = 0; n < NUM_SWAP_BUFFERS; n++)
	{
		hr = swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
		device->CreateRenderTargetView(renderTargets[n], nullptr, cdh);
		cdh.ptr += renderTargetDescriptorSize;
	}

	
}

void RendererDX12::CreateViewportAndScissorRect()
{
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = (float)windowWidth;
	vp.Height = (float)windowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	scissorRect.left = (long)0;
	scissorRect.right = (long)windowWidth;
	scissorRect.top = (long)0;
	scissorRect.bottom = (long)windowHeight;
}

void RendererDX12::CreateRenderHeap()
{
	if (bufferResource)
		bufferResource->Release();

	//Note: using upload heaps to transfer static data like vert buffers is not 
	//recommended. Every time the GPU needs it, the upload heap will be marshalled 
	//over. Please read up on Default Heap usage. An upload heap is used here for 
	//code simplicity and because there are very few vertices to actually transfer.
	D3D12_HEAP_PROPERTIES hp = {};
	hp.Type = D3D12_HEAP_TYPE_UPLOAD;
	hp.CreationNodeMask = 1;
	hp.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC rd = {};
	rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Width = SIZE_OF_HEAP;
	rd.Height = 1;
	rd.DepthOrArraySize = 1;
	rd.MipLevels = 1;
	rd.SampleDesc.Count = 1;
	rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//Creates both a resource and an implicit heap, such that the heap is big enough
	//to contain the entire resource and the resource is mapped to the heap. 
	device->CreateCommittedResource(
		&hp,
		D3D12_HEAP_FLAG_NONE,
		&rd,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&bufferResource));

	bufferResource->SetName(L"vb heap");
}

void RendererDX12::CreateConstantBufferHeap()
{
	
	//D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorDesc = {};
	//heapDescriptorDesc.NumDescriptors = NR_OF_OBJECTS;
	//heapDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//heapDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//device->CreateDescriptorHeap(&heapDescriptorDesc, IID_PPV_ARGS(&CBDescriptorHeap));
	//

	//D3D12_HEAP_PROPERTIES heapProperties = {};
	//heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//heapProperties.CreationNodeMask = 1; //used when multi-gpu
	//heapProperties.VisibleNodeMask = 1; //used when multi-gpu
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	//D3D12_RESOURCE_DESC resourceDesc = {};
	//resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	//resourceDesc.Width = 1;
	//resourceDesc.Height = 1;
	//resourceDesc.DepthOrArraySize = 1;
	//resourceDesc.MipLevels = 1;
	//resourceDesc.SampleDesc.Count = 1;
	//resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//

	////Create a resource heap, descriptor heap, and pointer to cbv for each frame
	//
	//HRESULT hr = device->CreateCommittedResource(
	//	&heapProperties,
	//	D3D12_HEAP_FLAG_NONE,
	//	&resourceDesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&constantBufferResource)
	//);

	//constantBufferResource->SetName(L"cb heap");

	//D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	//cbvDesc.BufferLocation = constantBufferResource->GetGPUVirtualAddress();
	//cbvDesc.SizeInBytes = (sizeof(float) * 4 +255) & ~255;	// 256-byte aligned CB.
	//device->CreateConstantBufferView(&cbvDesc, CBDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	//constantBufferResource->Map(0, &range, &constantBufferCPU_mappedPtr);

	////gVertexBufferResource->Unmap(0, nullptr);	//we don't unmap here, want to update every frame

	D3D12_DESCRIPTOR_HEAP_DESC heapDescriptorDesc = {};
	heapDescriptorDesc.NumDescriptors = NR_OF_OBJECTS;
	heapDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	device->CreateDescriptorHeap(&heapDescriptorDesc, IID_PPV_ARGS(&CBDescriptorHeap));
	device->CreateDescriptorHeap(&heapDescriptorDesc, IID_PPV_ARGS(&DiffuseDescriptorHeap));

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.CreationNodeMask = 1; //used when multi-gpu
	heapProperties.VisibleNodeMask = 1; //used when multi-gpu
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Width = 1;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	//Create a resource heap, descriptor heap, and pointer to cbv for each frame

	HRESULT hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(NR_OF_OBJECTS * ((sizeof(float) * 4 + 255) & ~255)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBufferResource)
	);
	
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(NR_OF_OBJECTS * ((sizeof(float) * 4 + 255) & ~255)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantDiffuseBufferResource)
	);
	constantDiffuseBufferResource->SetName(L"Diffuse Heap");
	constantBufferResource->SetName(L"cb heap");

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc[NR_OF_OBJECTS] = {};
	D3D12_CONSTANT_BUFFER_VIEW_DESC diffuseCBDesc[NR_OF_OBJECTS] = {};
	auto CPUHandle = CBDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto CPUHandleDiffuse = DiffuseDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (int i = 0; i < NR_OF_OBJECTS; i++)
	{
		cbvDesc[i].BufferLocation = constantBufferResource->GetGPUVirtualAddress();
		cbvDesc[i].BufferLocation += i*((sizeof(float) * 4 + 255) & ~255);
		cbvDesc[i].SizeInBytes = (sizeof(float) * 4 + 255) & ~255;	// 256-byte aligned CB.
		device->CreateConstantBufferView(&cbvDesc[i], CPUHandle);
		CPUHandle.ptr += incrementSize;


		diffuseCBDesc[i].BufferLocation = constantDiffuseBufferResource->GetGPUVirtualAddress();
		diffuseCBDesc[i].BufferLocation += i*((sizeof(float) * 4 + 255) & ~255);
		diffuseCBDesc[i].SizeInBytes = (sizeof(float) * 4 + 255) & ~255;	// 256-byte aligned CB.
		device->CreateConstantBufferView(&diffuseCBDesc[i], CPUHandleDiffuse);
		CPUHandleDiffuse.ptr += incrementSize;



	}

	D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	constantBufferResource->Map(0, &range, &constantBufferCPU_mappedPtr);
	constantDiffuseBufferResource->Map(0, &range, &constantDiffuseBufferCPU_mappedPtr);

	//gVertexBufferResource->Unmap(0, nullptr);	//we don't unmap here, want to update every frame
	
	D3D12_DESCRIPTOR_HEAP_DESC samplerDesc = {};
	samplerDesc.NumDescriptors = 1;
	samplerDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samplerDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	hr = device->CreateDescriptorHeap(&samplerDesc, IID_PPV_ARGS(&samplerHeap));

	D3D12_DESCRIPTOR_HEAP_DESC textureDesc = {};
	textureDesc.NumDescriptors = 1;
	textureDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	textureDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	
	hr = device->CreateDescriptorHeap(&textureDesc, IID_PPV_ARGS(&textureHeap));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&depthStencileHeap));


}

void RendererDX12::CreateDepthStencil()
{
	HRESULT hr;

	D3D12_RESOURCE_DESC depthDesc = {};

	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthDesc.Alignment = 0;
	depthDesc.Width = windowWidth;
	depthDesc.Height = windowHeight;
	depthDesc.DepthOrArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.MipLevels = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D32_FLOAT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthDesc, D3D12_RESOURCE_STATE_COMMON,
		&optClear, IID_PPV_ARGS(&depthstencil));

	device->CreateDepthStencilView(depthstencil, nullptr, depthStencileHeap->GetCPUDescriptorHandleForHeapStart());

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depthstencil, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void RendererDX12::WaitForGPU()
{
	//WAITING FOR EACH FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	//This is code implemented as such for simplicity. The cpu could for example be used
	//for other tasks to prepare the next frame while the current one is being rendered.

	//Signal and increment the fence value.
	const UINT64 fence = fenceValue;
	commandQueue->Signal(this->fence, fence);
	fenceValue++;

	//Wait until command queue is done.
	if (this->fence->GetCompletedValue() < fence)
	{
		this->fence->SetEventOnCompletion(fence, eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
	}
}

void RendererDX12::SetResourceTransitionBarrier(ID3D12GraphicsCommandList * commandList, ID3D12Resource * resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{

	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = StateBefore;
	barrierDesc.Transition.StateAfter = StateAfter;

	commandList->ResourceBarrier(1, &barrierDesc);

}

int RendererDX12::initialize(unsigned int width, unsigned int height)
{
	windowHeight = height;
	windowWidth = width;
	InitializeWindow();					//1. Create Window

	CreateDirect3DDevice();					//2. Create Device
	CreateCommandInterfacesAndSwapChain();	//3. Create CommandQueue and SwapChain
	commandAllocator->Reset();
	HRESULT hr = commandList->Reset(commandAllocator, nullptr);
	CreateFenceAndEventHandle();			//4. Create Fence and Event handle
	CreateRenderTargets();					//5. Create render targets for backbuffer
	CreateViewportAndScissorRect();
	CreateRenderHeap();
	CreateConstantBufferHeap();
	CreateDepthStencil();

	commandList->Close();
	ID3D12CommandList* listsToExecute[] = { commandList };
	commandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);

	WaitForGPU();
	commandAllocator->Reset();
	hr = commandList->Reset(commandAllocator, nullptr);

	

	return 0;
}

void RendererDX12::present()
{

	//Execute the command list.
	ID3D12CommandList* listsToExecute[] = { commandList };
	commandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);

	//Present the frame.
	swapChain->Present(0, 0);
	WaitForGPU();

	//Swap frame index for next frame.
	frameIndex = (frameIndex + 1) % NUM_SWAP_BUFFERS;

	
}

int RendererDX12::shutdown()
{
	WaitForGPU();
	

	CloseHandle(eventHandle);
	SafeRelease(&device);
	SafeRelease(&commandQueue);
	SafeRelease(&commandAllocator);
	SafeRelease(&commandList);
	SafeRelease(&swapChain);

	SafeRelease(&fence);

	SafeRelease(&renderTargetsHeap);
	SafeRelease(&samplerHeap);
	SafeRelease(&textureHeap);
	for (int i = 0; i < NUM_SWAP_BUFFERS; i++)
	{
		//SafeRelease(&descriptorHeap[i]);
		//SafeRelease(&constantBufferResource[i]);
		SafeRelease(&renderTargets[i]);
	}

	//SafeRelease(&rootSignature);
	//SafeRelease(&pipeLineState);

	//SafeRelease(&vertexBufferResource);

	SafeRelease(&bufferResource);
	SafeRelease(&textureResource);
	
	SafeRelease(&constantBufferResource);
	SafeRelease(&CBDescriptorHeap);

	SafeRelease(&constantDiffuseBufferResource);
	SafeRelease(&DiffuseDescriptorHeap);

	SafeRelease(&depthStencileHeap);
	SafeRelease(&depthstencil);

	return 0;
}

void RendererDX12::setClearColor(float r, float g, float b, float a)
{
	clearColor[0] = r;	
	clearColor[1] = g;
	clearColor[2] = b;
	clearColor[3] = a;
}

void RendererDX12::clearBuffer(unsigned int)
{
	/*First to be called*/
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += renderTargetDescriptorSize * frameIndex;

	commandAllocator->Reset();
	HRESULT hr = commandList->Reset(commandAllocator, nullptr);

	commandList->ClearRenderTargetView(cdh, clearColor, 0, nullptr);

}

void RendererDX12::setRenderTarget(RenderTarget * rt)
{
	/*Empty?*/

}

void RendererDX12::setRenderState(RenderState * ps)
{
	ps->set();
	

}

void RendererDX12::setWireFrame(bool active)
{
	currentMaterial->setWireFrameMode(active);	
}

void RendererDX12::submit(Mesh * mesh)
{
	/*Second to be called, in a loop*/

	drawList.push_back(mesh);
}

void RendererDX12::frame()
{
	/*Third to be called*/
	D3D12_CPU_DESCRIPTOR_HANDLE cdh = renderTargetsHeap->GetCPUDescriptorHandleForHeapStart();
	cdh.ptr += renderTargetDescriptorSize * frameIndex;

		//Set necessary states.
	commandList->RSSetViewports(1, &vp);
	commandList->RSSetScissorRects(1, &scissorRect);

	//Indicate that the back buffer will be used as render target.
	SetResourceTransitionBarrier(commandList,
		renderTargets[frameIndex],
		D3D12_RESOURCE_STATE_PRESENT,		//state before
		D3D12_RESOURCE_STATE_RENDER_TARGET	//state after
	);

	//Record commands.
	commandList->OMSetRenderTargets(1, &cdh, true, &depthStencileHeap->GetCPUDescriptorHandleForHeapStart());
	commandList->ClearRenderTargetView(cdh, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencileHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	
	

	D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	int offset = (sizeof(float) * 4 + 255) & ~255;
	int index = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = CBDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandleDiffuse = DiffuseDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	//ID3D12DescriptorHeap *heaps[2] = { /*CBDescriptorHeap,*/ textureHeap, samplerHeap };

	

	

	auto incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	float firstDepth = 0.0f;

	if (drawList.size())
	{
		size_t size;
		auto mesh = drawList[0];
		void* constData = mesh->txBuffer->getData(size);

		firstDepth = ((float*)constData)[2];
	}

	for (auto &mesh : drawList)
	{
		
		
		Technique* t = mesh->technique;
		currentMaterial = (MaterialDX12*) t->material;
		t->enable(this);
		size_t sizeOfDiffuseBuffer;
		void* diffuseBufferDataDeluxe = currentMaterial->getConstantBuffer(DIFFUSE_TINT_NAME, DIFFUSE_TINT)->getData(sizeOfDiffuseBuffer);
		//commandList->SetDescriptorHeaps(2, heaps);

		size_t size;

		//

		void* constData = mesh->txBuffer->getData(size);

		((float*)constData)[2] += 0.00005*drawList.size()/(index+1);

		//ZeroMemory((char*)constantBufferCPU_mappedPtr + index*offset, (sizeof(float) * 4 + 255) & ~255);
		memcpy(((char*)constantBufferCPU_mappedPtr + index*offset), constData, size);
		CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(CBDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		cbv.Offset(index, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		//commandList->SetGraphicsRootDescriptorTable(0, cbv);
		

		commandList->SetDescriptorHeaps(1, &CBDescriptorHeap);
		commandList->SetGraphicsRootDescriptorTable(0, GPUHandle);

	/*	commandList->SetGraphicsRootDescriptorTable(0,
			CBDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
*/
		// bind buffers for this mesh.
		// this implementation only has buffers in the Vertex Shader!
		// bind them all before drawing.
		size_t combSize;
		void* bufferData = mesh->CombineBufferData(combSize);
		void* dataBegin;
		bufferResource->Map(0, &range, &dataBegin);
		memcpy(dataBegin, bufferData, combSize);
		bufferResource->Unmap(0, nullptr);

		D3D12_VERTEX_BUFFER_VIEW view;
		view.BufferLocation = bufferResource->GetGPUVirtualAddress();
		view.SizeInBytes = SIZE_OF_HEAP;
		view.StrideInBytes = 0;

		commandList->IASetVertexBuffers(0, 1, &view);
		//
		//

		delete bufferData;

		////size_t numberElements = 3;
		////for (auto element : mesh->geometryBuffers) {
		////	mesh->bindIAVertexBuffer(element.first);
		////	numberElements = element.second.numElements;
		////}

		//mesh->txBuffer->bind(mesh->technique->material);

		//// everything is bound!
		//// always 0 because we are just generating gl_VertexId
		////glDrawArrays(GL_TRIANGLES, 0, numberElements);

		//glBindTexture(GL_TEXTURE_2D, 0);
		for (auto &t : mesh->textures)
		{
			// we do not really know here if the sampler has been
			// defined in the shader.
			((float*)diffuseBufferDataDeluxe)[3] = 0.0f;



			t.second->bind(t.first);
		}
		
		memcpy(((char*)constantDiffuseBufferCPU_mappedPtr + index*offset), diffuseBufferDataDeluxe, sizeOfDiffuseBuffer);
		CD3DX12_GPU_DESCRIPTOR_HANDLE diffuseCBV(DiffuseDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		diffuseCBV.Offset(index, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		//commandList->SetGraphicsRootDescriptorTable(0, cbv);


		commandList->SetDescriptorHeaps(1, &DiffuseDescriptorHeap);
		commandList->SetGraphicsRootDescriptorTable(3, GPUHandleDiffuse);
		
		commandList->DrawInstanced(3, 1, 0, 0);

		
		index++;
		GPUHandle.ptr += incrementSize;
		GPUHandleDiffuse.ptr += incrementSize;
	}


	drawList.clear();


	//Indicate that the back buffer will now be used to present.
	SetResourceTransitionBarrier(commandList,
		renderTargets[frameIndex],
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//state before
		D3D12_RESOURCE_STATE_PRESENT		//state after
	);

	commandList->Close();
}

void RendererDX12::CreateTextureHeap(Texture2DDX12 * texture)
{
	//D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	//viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//viewDesc.Texture2D.MostDetailedMip = 0;
	//viewDesc.Texture2D.MipLevels = 1;
	//viewDesc.Texture2D.PlaneSlice = 0;
	//viewDesc.Texture2D.ResourceMinLODClamp = 0;

	//D3D12_RESOURCE_DESC resourceDesc = {};
	//resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//resourceDesc.Alignment = 0;
	//resourceDesc.DepthOrArraySize = 1;
	//resourceDesc.Height = 640;
	//resourceDesc.Width = 640;
	//resourceDesc.Format = texture->desc.Format;
	//resourceDesc.MipLevels = 0;
	//resourceDesc.SampleDesc.Count = 1;
	//resourceDesc.SampleDesc.Quality = 0;
	//HRESULT hr;
	// hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
	//	D3D12_HEAP_FLAG_NONE,
	//	&CD3DX12_RESOURCE_DESC::Buffer(texture->sizeOfTexture),
	//	 D3D12_RESOURCE_STATE_COPY_DEST,
	//	nullptr,
	//	IID_PPV_ARGS(&textureResource)
	//);

	/*THIS API CANNOT BE CALLED, ÖPPNA LISTAN FÖR FAN*/
	
	commandList->Close();
	ID3D12CommandList* listsToExecute[] = { commandList };
	commandQueue->ExecuteCommandLists(ARRAYSIZE(listsToExecute), listsToExecute);

	WaitForGPU();

	commandAllocator->Reset();
	HRESULT hr = commandList->Reset(commandAllocator, nullptr);
	WaitForGPU();

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(textureHeap->GetCPUDescriptorHandleForHeapStart());

	device->CreateShaderResourceView(texture->textureResource2, &texture->desc, hDescriptor);

	commandList->Close();
	
	WaitForGPU();
	
}

void RendererDX12::UploadTexture(Texture2DDX12 * texture)
{
	//void* ptr;
	//commandList->SetDescriptorHeaps(1, &textureHeap);
	//D3D12_PLACED_SUBRESOURCE_FOOTPRINT temp = {0};
	//device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &temp, NULL, NULL, NULL);

	//D3D12_RANGE range = { 0, 0 };
	//commandList->CopyTextureRegion(&CD3DX12_TEXTURE_COPY_LOCATION(texture->textureResource, 0),
	//	0, 0, 0,
	//	&CD3DX12_TEXTURE_COPY_LOCATION(texture->textureResource, temp), nullptr);

	CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(textureHeap->GetGPUDescriptorHandleForHeapStart());
	//texHandle.Offset()
	if (texture->textureResource2)
	{
		commandList->SetDescriptorHeaps(1, &textureHeap);
	}
	else
	{
		commandList->SetDescriptorHeaps(1, nullptr);
	}
	commandList->SetGraphicsRootDescriptorTable(1, texHandle);
	

	

	if (texture->sampler)
	{
		Sampler2DDX12* sampler = (Sampler2DDX12*)texture->sampler;
		commandList->SetDescriptorHeaps(1, &samplerHeap);
		device->CreateSampler(&sampler->description, samplerHeap->GetCPUDescriptorHandleForHeapStart());
		commandList->SetGraphicsRootDescriptorTable(2, samplerHeap->GetGPUDescriptorHandleForHeapStart());
	}
}
