#include "ConstantBufferDX12.h"

ConstantBufferDX12::ConstantBufferDX12(std::string NAME, unsigned int location)
{
	this->data = nullptr;
}

ConstantBufferDX12::~ConstantBufferDX12()
{
	delete data;
}

void ConstantBufferDX12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	if (this->data)
	{
		delete this->data;
		this->data = nullptr;
	}
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->dataSize = size;
	/*BEEP the rest*/
}

void ConstantBufferDX12::bind(Material * material)
{
	/*PFT*/
}
