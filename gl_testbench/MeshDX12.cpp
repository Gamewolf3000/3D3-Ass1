#include "MeshDX12.h"

MeshDX12::MeshDX12()
{

}
MeshDX12::~MeshDX12()
{
	for (auto &t : textures)
		delete t.second;
}