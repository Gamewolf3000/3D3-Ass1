#include <iostream>

#include "Technique.h"
#include "Renderer.h"



Technique::Technique()
{
}

Technique::~Technique()
{
	std::cout << "destroyed technique" << std::endl;
	delete renderState;
}

void Technique::enable(Renderer* renderer)
{
	// better to delegate the render state to the renderer, it can be
	// more clever about changes with current render state set.
	renderer->setRenderState(renderState);
	material->enable();
}

//void Technique::setBindings(std::vector<ResourceBinding*>* rb) { bindings = rb; }
