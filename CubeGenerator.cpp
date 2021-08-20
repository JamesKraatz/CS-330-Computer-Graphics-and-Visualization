#include "CubeGenerator.h"
//#include <glm\glm.hpp>
//#include <glm\gtc\matrix_transform.hpp>
#include "Vertex.h"

#define PI 3.14159265359
using glm::vec3;
using glm::mat4;
using glm::mat3;
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

glm::vec3 randomCubeColor()
{
	glm::vec3 ret;
	ret.x = rand() / (float)RAND_MAX;
	ret.y = rand() / (float)RAND_MAX;
	ret.z = rand() / (float)RAND_MAX;
	return ret;
}


ShapeData CubeGenerator::makeCubeVerts()	// 3D cubes have 8 vertices
{
	ShapeData ret;
	uint dimensions = 2;
	ret.numVertices = 8; // 3D cubes have 8 vertices 
	int half = dimensions / 2;
	ret.vertices = new Vertex[ret.numVertices];
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				Vertex& thisVert = ret.vertices[(i * dimensions * dimensions) + (j * dimensions) + k];
				thisVert.position.x = (2 * ((j + k) %2)) - half;
				thisVert.position.y = half - (2 * j);
				thisVert.position.z = half - (2 * i);
				//thisVert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
				thisVert.normal = glm::normalize(thisVert.position);
				thisVert.color = randomCubeColor();
			}			
		}
	}
	return ret;
}

ShapeData CubeGenerator::makeCubeIndices()
{
	const uint numberOfSquares = 6;
	const uint dimensions = numberOfSquares;

	ShapeData ret;
	ret.numIndices = dimensions * 2 * 3; // 2 triangles per square, 3 indices per triangle
	ret.indices = new unsigned short[ret.numIndices];
	
	int runner = 0;

	// adjustment values to align indices to vertices on all 6 faces
	int col = 0;
	int colAdj = 0;
	int col6Adj = 0;
	int lastVert = 0;
	
	for (int i = 0; i < numberOfSquares; i++)
	{
		col = i;
		colAdj = 0;
		col6Adj = 0;

		// adjustment for bottom face of cube
		if (i == 1)
		{
			col = 4;
		}
		//  adjustment for vertical faces
		else if (i > 1) 
		{
			col = i - 2;
			colAdj = 3;
			// last index to be added each iteration needs an aditional adjustment
			// when on the side faces
			lastVert = -2;

			// adjustment for last vertical face, (vertices wrap-around)
			if(i == 5)
			{
				col6Adj = -4;
			}
		}

		// add 2 triangs at 3 vertices each for every face
		ret.indices[runner++] = col;
		ret.indices[runner++] = col + colAdj + 1;
		ret.indices[runner++] = col + colAdj + col6Adj + 2;

		ret.indices[runner++] = col;
		ret.indices[runner++] = col + colAdj + col6Adj + 2;
		ret.indices[runner++] = col + col6Adj + lastVert + 3;
	}
	assert(runner = ret.numIndices);
	return ret;
}


ShapeData CubeGenerator::makeCube()
{
	ShapeData ret = makeCubeVerts();
	ShapeData ret2 = makeCubeIndices();
	ret.numIndices = ret2.numIndices;
	ret.indices = ret2.indices;

	return ret;
}