#ifndef TRANSFORM_DX_12_H
#define TRANSFORM_DX_12_H

#include "Transform.h"

class TransformDX12 : public Transform
{
	TransformDX12() {};
	~TransformDX12() {};
	void translate(float x, float y, float z)
	{
		//transform[3][0] += x;
		//transform[3][1] += y;
		//transform[3][2] += z;
	}

	void rotate(float radians, float x, float y, float z)
	{
		//transform = glm::rotate(transform, radians, glm::vec3(x, y, z));
	}

	//glm::mat4 transform;
};



#endif
