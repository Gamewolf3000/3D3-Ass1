#include "MaterialDX12.h"
#include <vector>
#include "CD3DX12Helper.h"

void splitDX(std::string text, std::vector<std::string>* temp, const char delim = ' ')
{
	int pos = text.find(delim, 0);
	if (pos == -1)
		temp->push_back(text);
	else {
		temp->push_back(text.substr(0, pos));
		splitDX(text.substr(pos + 1, text.length() - pos - 1), temp, delim);
	}
}

void MaterialDX12::CreateRootSignature()
{


	////define descriptor range(s)
	//D3D12_DESCRIPTOR_RANGE  dtConstant[1];
	//dtConstant[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	//dtConstant[0].NumDescriptors = 1; //only one CB in this example
	//dtConstant[0].BaseShaderRegister = 0; //register b0
	//dtConstant[0].RegisterSpace = 0; //register(b0,space0);
	//dtConstant[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//D3D12_DESCRIPTOR_RANGE  dtTexture[1];
	//dtTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//dtTexture[0].NumDescriptors = 1; //only one texture in this example
	//dtTexture[0].BaseShaderRegister = 0; //register t0
	//dtTexture[0].RegisterSpace = 0; //register(t0,space0);
	//dtTexture[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//D3D12_DESCRIPTOR_RANGE  dtSampler[1];
	//dtSampler[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	//dtSampler[1].NumDescriptors = 1; //only one sampler in this example
	//dtSampler[1].BaseShaderRegister = 0; //register s0
	//dtSampler[1].RegisterSpace = 0; //register(s0,space0);
	//dtSampler[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	////create a descriptor table
	//D3D12_ROOT_DESCRIPTOR_TABLE dt1;
	//dt1.NumDescriptorRanges = ARRAYSIZE(dtConstant);
	//dt1.pDescriptorRanges = dtConstant;

	//D3D12_ROOT_DESCRIPTOR_TABLE dt2;
	//dt2.NumDescriptorRanges = ARRAYSIZE(dtTexture);
	//dt2.pDescriptorRanges = dtTexture;

	//D3D12_ROOT_DESCRIPTOR_TABLE dt3;
	//dt3.NumDescriptorRanges = ARRAYSIZE(dtSampler);
	//dt3.pDescriptorRanges = dtSampler;

	////create root parameter
	//D3D12_ROOT_PARAMETER  rootParam[3];
	//rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParam[0].DescriptorTable = dt1;
	//rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	//rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParam[1].DescriptorTable = dt2;
	//rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//rootParam[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParam[2].DescriptorTable = dt3;
	//rootParam[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//D3D12_STATIC_SAMPLER_DESC sampler = {};
	//sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//sampler.ShaderRegister = 0;



	//D3D12_ROOT_SIGNATURE_DESC rsDesc;
	//rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//rsDesc.NumParameters = ARRAYSIZE(rootParam);
	//rsDesc.pParameters = rootParam;
	//rsDesc.NumStaticSamplers = 1;
	//rsDesc.pStaticSamplers = &sampler;


	CD3DX12_DESCRIPTOR_RANGE descRange[4];
	descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
	descRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);


	CD3DX12_ROOT_PARAMETER rootParameters[4];
	rootParameters[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[2].InitAsDescriptorTable(1, &descRange[2], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[3].InitAsDescriptorTable(1, &descRange[3], D3D12_SHADER_VISIBILITY_PIXEL);


	CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Init(4, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* sBlob;
	HRESULT hr = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&sBlob,
		nullptr);
	auto a = sBlob->GetBufferSize();

 	hr = _device->CreateRootSignature(
		0,
		sBlob->GetBufferPointer(),
		sBlob->GetBufferSize(),
		IID_PPV_ARGS(&_rootSignature));

	sBlob->Release();
	
}

void MaterialDX12::CreateShadersAndPiplelineState()
{
	std::wstring temp(shaderFileNames[Material::ShaderType::VS].begin(), shaderFileNames[Material::ShaderType::VS].end());
	LPCWSTR fileName = temp.c_str();
	////// Shader Compiles //////
	ID3DBlob* vertexBlob;
	HRESULT hr = D3DCompileFromFile(
		fileName, // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"vs_5_1",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&vertexBlob,	// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	ID3DBlob* pixelBlob;
	temp = std::wstring(shaderFileNames[Material::ShaderType::PS].begin(), shaderFileNames[Material::ShaderType::PS].end());
	fileName = temp.c_str();
	hr = D3DCompileFromFile(
		fileName, // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"ps_5_1",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pixelBlob,		// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	////// Input Layout //////
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "POSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16 ,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "POSITION", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32 ,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 80 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, 96 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "UV" , 1, DXGI_FORMAT_R32G32_FLOAT, 0, 104 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV" , 2, DXGI_FORMAT_R32G32_FLOAT, 0, 112 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = ARRAYSIZE(inputElementDesc);

	////// Pipline State //////
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsd = {};

	//Specify pipeline stages:
	gpsd.pRootSignature = _rootSignature;
	gpsd.InputLayout = inputLayoutDesc;
	gpsd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsd.VS.pShaderBytecode = reinterpret_cast<void*>(vertexBlob->GetBufferPointer());
	gpsd.VS.BytecodeLength = vertexBlob->GetBufferSize();
	gpsd.PS.pShaderBytecode = reinterpret_cast<void*>(pixelBlob->GetBufferPointer());
	gpsd.PS.BytecodeLength = pixelBlob->GetBufferSize();

	//Specify render target and depthstencil usage.
	gpsd.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsd.NumRenderTargets = 1;

	gpsd.SampleDesc.Count = 1;
	gpsd.SampleMask = UINT_MAX;

	//Specify rasterizer behaviour.
	gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsd.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	//DEPTH STENCIL CHEAT!
	gpsd.DepthStencilState.DepthEnable = true;
	gpsd.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpsd.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsd.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	

	//Specify blend descriptions.
	D3D12_RENDER_TARGET_BLEND_DESC defaultRTdesc = {
		false, false,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL };
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		gpsd.BlendState.RenderTarget[i] = defaultRTdesc;

	hr = _device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&_pipelineState));
	gpsd.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	hr = _device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(&_pipelineStateWire));
	
	vertexBlob->Release();
	pixelBlob->Release();
}

MaterialDX12::MaterialDX12(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	_device = device;
	_commandList = commandList;
}

MaterialDX12::~MaterialDX12()
{
	for (auto &cb : _constantBuffer)
		delete cb.second;
	_rootSignature->Release();
	
	_pipelineState->Release();
	_pipelineStateWire->Release();
}

void MaterialDX12::setShader(const std::string & shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		// removeShader is implemented in a concrete class
		removeShader(type);
	}
	shaderFileNames[type] = shaderFileName;
}

void MaterialDX12::removeShader(ShaderType type)
{
	//GLuint shader = shaderObjects[(GLuint)type];

	//// check if name exists (if it doesn't there should not be a shader here.
	//if (shaderFileNames.find(type) == shaderFileNames.end())
	//{
	//	assert(shader == 0);
	//	return;
	//}
	//else if (shader != 0 && program != 0) {
	//	glDetachShader(program, shader);
	//	glDeleteShader(shader);
	//};

}

void MaterialDX12::setDiffuse(Color c)
{

}

int MaterialDX12::compileMaterial(std::string & errString)
{
	CreateRootSignature();
	CreateShadersAndPiplelineState();



	return 0;
}

void MaterialDX12::addConstantBuffer(std::string name, unsigned int location)
{
	if (_constantBuffer[location] != nullptr)
	{
		delete _constantBuffer[location];
		_constantBuffer[location] = nullptr;
	}
	_constantBuffer[location] = new ConstantBufferDX12(name, location);
}

void MaterialDX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
	_constantBuffer[location]->setData(data, size, this, location);
}

int MaterialDX12::enable()
{
	_commandList->SetGraphicsRootSignature(_rootSignature);
	if (_wire)
		_commandList->SetPipelineState(_pipelineStateWire);
	else
		_commandList->SetPipelineState(_pipelineState);
	return 0;
}

void MaterialDX12::setWireFrameMode(bool active)
{
	_wire = active;
}

void MaterialDX12::disable()
{

}
