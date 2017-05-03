#ifndef CONSTANT_BUFFER_DX12_H
#define CONSTANT_BUFFER_DX12_H

#include "ConstantBuffer.h"

class ConstantBufferDX12 : public ConstantBuffer
{
public:
	ConstantBufferDX12(std::string NAME, unsigned int location);
	~ConstantBufferDX12();
	// set data will update the buffer associated, including whatever is necessary to
	// update the GPU memory.
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material* material);
	void* getData(size_t &size) {
		size = dataSize;
		return data;
	};
protected:
	// don't use this.
	ConstantBufferDX12() = delete;

private:
	void* data;
	size_t dataSize;
};




#endif