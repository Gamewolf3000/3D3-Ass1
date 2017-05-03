#ifndef RESOURCE_BINDING_DX_12_H
#define RESOURCE_BINDING_DX_12_H

#include "ResourceBinding.h"

class ResourceBindingDX12 : public ResourceBinding
{
public:
	ResourceBindingDX12();
	~ResourceBindingDX12();
	// update by hand whenever you want
	// id is a unique Id of a resource on the shader.
	template<typename T, typename V> void updateValue(T id, V& value);
	// can assume that ptr will be updated every frame.
	template<typename T, typename V> void mapBufferToAddr(T name, V ptr);

};




#endif

template<typename T, typename V>
inline void ResourceBindingDX12::updateValue(T id, V & value)
{
}

template<typename T, typename V>
inline void ResourceBindingDX12::mapBufferToAddr(T name, V ptr)
{
}
