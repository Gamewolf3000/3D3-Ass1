#ifndef SAMPLER_2D_DX_12_H
#define SAMPLER_2D_DX_12_H

#include "Sampler2D.h"

#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <d3dcompiler.h>

#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")

class Sampler2DDX12 : public Sampler2D
{
public:
	Sampler2DDX12();
	~Sampler2DDX12();
	void setMagFilter(FILTER filter);
	void setMinFilter(FILTER filter);
	void setWrap(WRAPPING s, WRAPPING t);

	D3D12_SAMPLER_DESC description = {};
	FILTER mag = FILTER::LINEAR;
	FILTER min = FILTER::LINEAR;

	

};


#endif
