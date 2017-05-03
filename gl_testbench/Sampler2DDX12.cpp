#include "Sampler2DDX12.h"

Sampler2DDX12::Sampler2DDX12()
{
	description.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	description.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	description.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	description.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	description.MinLOD = 0;
	description.MaxLOD = D3D12_FLOAT32_MAX;
	description.MipLODBias = 0.0f;
	description.MaxAnisotropy = 1;
	description.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	description.BorderColor[0] = 0.0f;
	description.BorderColor[1] = 0.0f;
	description.BorderColor[2] = 1.0f;
	description.BorderColor[3] = 1.0f;
}

Sampler2DDX12::~Sampler2DDX12()
{

}

void Sampler2DDX12::setMagFilter(FILTER filter)
{
	mag = filter;
	if (mag == LINEAR)
	{
		if(min == LINEAR)
			description.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		else if (min == POINT_SAMPLER)
			description.Filter = D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if (mag == POINT_SAMPLER)
	{
		if (min == LINEAR)
			description.Filter = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		else if (min == POINT_SAMPLER)
			description.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
}

void Sampler2DDX12::setMinFilter(FILTER filter)
{
	min = filter;
	if (mag == LINEAR)
	{
		if (min == LINEAR)
			description.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		else if (min == POINT_SAMPLER)
			description.Filter = D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	}
	else if (mag == POINT_SAMPLER)
	{
		if (min == LINEAR)
			description.Filter = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		else if (min == POINT_SAMPLER)
			description.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	}
}

void Sampler2DDX12::setWrap(WRAPPING s, WRAPPING t)
{
	if (s == WRAPPING::CLAMP)
		description.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	else if (s == WRAPPING::REPEAT)
		description.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	if (t == WRAPPING::CLAMP)
		description.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	else if (t == WRAPPING::REPEAT)
		description.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;


}
