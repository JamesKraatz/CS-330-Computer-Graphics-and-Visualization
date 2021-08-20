#pragma once
#include "ShapeData.h"
typedef unsigned int uint;

class CubeGenerator
{
	static ShapeData makeCubeVerts();
	static ShapeData makeCubeIndices();


public:

	static ShapeData makeCube();

};
