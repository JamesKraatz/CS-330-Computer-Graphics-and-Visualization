#include "CylinderGenerator.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "Vertex.h"

#define PI 3.14159265359
using glm::vec3;
using glm::mat4;
using glm::mat3;
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

glm::vec3 randomCylinderColor()
{
	glm::vec3 ret;
	ret.x = rand() / (float)RAND_MAX;
	ret.y = rand() / (float)RAND_MAX;
	ret.z = rand() / (float)RAND_MAX;
	return ret;
}


ShapeData CylinderGenerator::makeCylinderVerts(int nSliceAngles, float length)
{
	ShapeData ret;

	// a cylinder is two circles-planes connected by orthogonal dual-triangle planes for each 4-point square.
	// two-points will be on one circle-plane 

	// calculate the number of vertices, not using stacks so the number of vertices is twice what is used for one circle-plane
	int secondCircleOffset = (nSliceAngles + 1);  // get start offset of second circle to calculate slice angle vertices
	ret.numVertices = secondCircleOffset * 2;	  // not using stacks so the number of vertices is twice what each
	float halfCylLength = length / 2.0f;				  // half of the cylinder goes above the xy-plane and half below it

	const float zPosFront = halfCylLength;
	const float zPosRear = -zPosFront;

	// get container for vertices
	ret.vertices = new Vertex[ret.numVertices];

	// angle and angle accumulator
	const float angle = 360.0f / (float)nSliceAngles;
	float angleAccumulator = 0.0f;

	// set center point on both circle-planes
	Vertex& frontVertex = ret.vertices[0];
	frontVertex.position.x = 0.0f;
	frontVertex.position.y = 0.0f;
	frontVertex.position.z = zPosFront;
	frontVertex.normal = glm::normalize(frontVertex.position);
	frontVertex.color = randomCylinderColor();
	Vertex& rearVertex = ret.vertices[secondCircleOffset];
	rearVertex.position.x = 0.0f;
	rearVertex.position.y = 0.0f;
	rearVertex.position.z = zPosRear;
	rearVertex.normal = glm::normalize(rearVertex.position);
	rearVertex.color = randomCylinderColor();

	// set vertex to first position on xy circle-plane perimeter
	const glm::vec3 frontStartPosition(1.0f, 0.0f, zPosFront);
	const glm::vec3 rearStartPosition(1.0f, 0.0f, zPosRear);

	// new position vector, z is fixed for the xy circle plane
	glm::vec3 newPosition;

	// create rotation axis, fixed to z
	glm::vec3 rotationAxis(0.0f, 0.0f, 1.0f);

	// declare rotation matrix
	glm::mat3 rotationMatrix(1.0f);
	
	Vertex frontVert;
	Vertex rearVert;

	// calculate the coordinates, start at 1 because 
	// circle-plane center is already in vector
	for (int i = 1; i <= nSliceAngles; i++)
	{
		// 1. calculate new xy position
		rotationMatrix = glm::rotate(glm::radians(angleAccumulator), rotationAxis);
		glm::vec3 newFrontPosition = rotationMatrix * frontStartPosition;
		glm::vec3 newRearPosition = rotationMatrix * rearStartPosition;

		// 2. add to front xy circle plane
		ret.vertices[i].position.x = newFrontPosition.x;
		ret.vertices[i].position.y = newFrontPosition.y;
		ret.vertices[i].position.z = newFrontPosition.z;
		ret.vertices[i].normal = glm::normalize(ret.vertices[i].position);
		ret.vertices[i].color = randomCylinderColor();


		// 3. add to rear xy circle plane
		ret.vertices[i + secondCircleOffset].position.x = newRearPosition.x;
		ret.vertices[i + secondCircleOffset].position.y = newRearPosition.y;
		ret.vertices[i + secondCircleOffset].position.z = newRearPosition.z;
		ret.vertices[i + secondCircleOffset].normal = glm::normalize(ret.vertices[i + secondCircleOffset].position);
		ret.vertices[i + secondCircleOffset].color = randomCylinderColor();

		// 4. add slice angle to angle accumulator
		angleAccumulator += angle;
	}

	return ret;
}

ShapeData CylinderGenerator::makeCylinderIndices(int nSliceAngles)
{
	const uint stacks = 1;
	ShapeData ret;

	// number of slice angles * (2 circle planes + 2 triangles per sidewall plane) * 3 pts per triangle
	// nSliceAngles * (2 + 2) * 3 = nSliceAngles * 4 * 12, there is only one stack
	ret.numIndices = nSliceAngles * (2 * (1 + stacks)) * 3;
	// create sized array and pass it to ShapeData object ret
	ret.indices = new unsigned short[ret.numIndices];
	
	
	int runner = 0;

	// array offset values
	const uint frontFaceOffset = 0;
	const uint rearFaceOffset = nSliceAngles + 1;

	uint a, b, c;

	for (int i = 0; i < nSliceAngles; i++)
	{
		a = (i + 1);
		b = (i + 2);
			
		if (b > nSliceAngles) { b = 1; }

		ret.indices[runner++] = frontFaceOffset + 0;
		ret.indices[runner++] = frontFaceOffset + a;
		ret.indices[runner++] = frontFaceOffset + b;

		ret.indices[runner++] = rearFaceOffset + 0;
		ret.indices[runner++] = rearFaceOffset + a;
		ret.indices[runner++] = rearFaceOffset + b;

		ret.indices[runner++] = frontFaceOffset + a;
		ret.indices[runner++] = rearFaceOffset + a;
		ret.indices[runner++] = rearFaceOffset + b;

		ret.indices[runner++] = frontFaceOffset + a;
		ret.indices[runner++] = rearFaceOffset + b;
		ret.indices[runner++] = frontFaceOffset + b;
	}

	assert(runner = ret.numIndices);

	ret.indices[0] = 0;

	return ret;
}


ShapeData CylinderGenerator::makeCylinder(int nSliceAngles, float length)
{
	ShapeData ret = makeCylinderVerts(nSliceAngles, length);
	ShapeData ret2 = makeCylinderIndices(nSliceAngles);
	ret.numIndices = ret2.numIndices;
	ret.indices = ret2.indices;

	return ret;
}
