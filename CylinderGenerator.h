#pragma once
#include "ShapeData.h"
typedef unsigned int uint;

class CylinderGenerator
{
	static ShapeData makeCylinderVerts(int nSliceAngles, float length);
	static ShapeData makeCylinderIndices(int nSliceAngles);


public:

	static ShapeData makeCylinder(int nSliceAngles, float length);

};
