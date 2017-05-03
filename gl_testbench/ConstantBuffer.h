#pragma once

#include "Material.h"

class ConstantBuffer
{
public:
	ConstantBuffer(std::string NAME, unsigned int location) {};
	~ConstantBuffer();
	// set data will update the buffer associated, including whatever is necessary to
	// update the GPU memory.
	virtual void setData(const void* data, size_t size, Material* m, unsigned int location) = 0;
	virtual void bind(Material*) = 0;
	virtual void* getData(size_t &size) { size = -1; return nullptr; };
protected:
	// don't use this.
	ConstantBuffer() {};
};

