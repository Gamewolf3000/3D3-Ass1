#include "OpenGLRenderer.h"
#include "RendererDX12.h"
#include "Renderer.h"


Renderer* Renderer::makeRenderer(BACKEND option)
{
	if (option == BACKEND::GL45)
		return new OpenGLRenderer();
	else if (option == BACKEND::DX12)
		return new RendererDX12();
}

/*
Technique* Renderer::createTechnique() {
	int newId = 0;
	while (techniques.count(newId) != 0)
		newId++;
	techniques[newId] = new Technique();

	return techniques[newId];
}

Technique* Renderer::getTechnique(unsigned int techniqueId) {
	if (techniques.count(techniqueId) == 1)
		return techniques[techniqueId];
	else
		return nullptr;
}
*/

