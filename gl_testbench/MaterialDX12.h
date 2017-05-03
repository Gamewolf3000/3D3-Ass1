#ifndef MATERIAL_DX_12
#define MATERIAL_DX_12

#include "Material.h"
#include "ConstantBufferDX12.h"

#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

class MaterialDX12 : public Material /*PSO?*/
{
private:

	ID3D12Device* _device = nullptr;
	ID3D12GraphicsCommandList* _commandList = nullptr;
	ID3D12RootSignature* _rootSignature = nullptr;
	ID3D12PipelineState* _pipelineState = nullptr;
	ID3D12PipelineState* _pipelineStateWire = nullptr;
	bool _wire = false;

	std::map<unsigned int, ConstantBufferDX12*> _constantBuffer;


	void CreateRootSignature();
	void CreateShadersAndPiplelineState();


	/*Stuff needed here*/
public:
	MaterialDX12(ID3D12Device* device = nullptr, ID3D12GraphicsCommandList* commandList = nullptr);
	~MaterialDX12();
	void setShader(const std::string& shaderFileName, ShaderType type);

	// removes any resource linked to shader type
	void removeShader(ShaderType type);

	void setDiffuse(Color c);

	/*
	* Compile and link all shaders
	* Returns 0  if compilation/linking succeeded.
	* Returns -1 if compilation/linking fails.
	* Error is returned in errString
	* A Vertex and a Fragment shader MUST be defined.
	* If compileMaterial is called again, it should RE-COMPILE the shader
	* In principle, it should only be necessary to re-compile if the defines set
	* has changed.
	*/
	int compileMaterial(std::string& errString);

	// this constant buffer will be bound every time we bind the material
	void addConstantBuffer(std::string name, unsigned int location);

	// location identifies the constant buffer in a unique way
	void updateConstantBuffer(const void* data, size_t size, unsigned int location);

	// activate the material for use.
	int enable();

	void setWireFrameMode(bool active);

	// disable material
	void disable();

	/*Constant buffers needed*/
	ConstantBufferDX12 *getConstantBuffer(std::string name, unsigned int location) {
		return _constantBuffer[location];
	}

};


#endif