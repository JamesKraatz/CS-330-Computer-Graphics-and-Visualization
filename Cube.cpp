// STL
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Project
#include "cube.h"
#include <iostream>




namespace static_meshes_3D {

	Cube::Cube(float height, float width, float depth, bool withPositions, bool withTextureCoordinates, bool withNormals)
		: StaticMesh3D(withPositions, withTextureCoordinates, withNormals)
		, _height(height)
		, _width(width)
		, _depth(depth)
	{
		initializeData();
	}

	float Cube::getHeight() const
	{
		return _height;
	}

	float Cube::getWidth() const
	{
		return _width;
	}

	float Cube::getDepth() const
	{
		return _depth;
	}

	void Cube::initializeData()
	{
		if (_isInitialized) {
			return;
		}

		// Generate VAO and VBO for vertex attributes
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		_vbo.createVBO(getVertexByteSize() * _numVerticesTotal);

		if (hasPositions())
		{
			float nWidth = _width / 2.0f;
			float nHeight = _height / 2.0f; float nDepth = _depth / 2.0f;
			glm::vec3 vertices[] = {
				// front face
				glm::vec3(-nWidth,  nHeight,  nDepth),
				glm::vec3(-nWidth, -nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight,  nDepth),
				glm::vec3(-nWidth,  nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight,  nDepth),
				glm::vec3(nWidth,  nHeight,  nDepth),
				// right face
				glm::vec3(nWidth,  nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight, -nDepth),
				glm::vec3(nWidth,  nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight, -nDepth),
				glm::vec3(nWidth,  nHeight, -nDepth),
				// rear face
				glm::vec3(nWidth,  nHeight, -nDepth),
				glm::vec3(nWidth, -nHeight, -nDepth),
				glm::vec3(-nWidth, -nHeight, -nDepth),
				glm::vec3(nWidth,  nHeight, -nDepth),
				glm::vec3(-nWidth, -nHeight, -nDepth),
				glm::vec3(-nWidth,  nHeight, -nDepth),
				// left face
				glm::vec3(-nWidth, -nHeight, -nDepth),
				glm::vec3(-nWidth,  nHeight, -nDepth),
				glm::vec3(-nWidth, -nHeight,  nDepth),
				glm::vec3(-nWidth,  nHeight, -nDepth),
				glm::vec3(-nWidth, -nHeight,  nDepth),
				glm::vec3(-nWidth,  nHeight,  nDepth),
				// top face
				glm::vec3(-nWidth,  nHeight, -nDepth),
				glm::vec3(-nWidth,  nHeight,  nDepth),
				glm::vec3(nWidth,  nHeight,  nDepth),
				glm::vec3(-nWidth,  nHeight, -nDepth),
				glm::vec3(nWidth,  nHeight,  nDepth),
				glm::vec3(nWidth,  nHeight, -nDepth),
				// bottom face
				glm::vec3(-nWidth, -nHeight,  nDepth),
				glm::vec3(-nWidth, -nHeight, -nDepth),
				glm::vec3(nWidth, -nHeight, -nDepth),
				glm::vec3(-nWidth, -nHeight,  nDepth),
				glm::vec3(nWidth, -nHeight, -nDepth),
				glm::vec3(nWidth, -nHeight,  nDepth)
			};

			_vbo.addRawData(vertices, sizeof(vertices));
		}

		if (hasTextureCoordinates())
		{
			int offset = 0;
			glm::vec2 texCoords[36];
			for (int i = 0; i < 6; i++)
			{
				texCoords[offset++] = glm::vec2(0.0f, 1.0f);
				texCoords[offset++] = glm::vec2(0.0f, 0.0f);
				texCoords[offset++] = glm::vec2(1.0f, 0.0f);
				texCoords[offset++] = glm::vec2(0.0f, 1.0f);
				texCoords[offset++] = glm::vec2(1.0f, 0.0f);
				texCoords[offset++] = glm::vec2(1.0f, 1.0f);
			}

			_vbo.addRawData(texCoords, sizeof(texCoords));
		}

		if (hasNormals())
		{
			int offset;
			glm::vec3 normals[36];
			for (int i = 0; i < 6; i++) {
				offset = 0;
				normals[i * offset++] = glm::vec3( 0.0f,  0.0f,  1.0f);
				normals[i * offset++] = glm::vec3( 1.0f,  0.0f,  0.0f);
				normals[i * offset++] = glm::vec3( 0.0f,  0.0f, -1.0f);
				normals[i * offset++] = glm::vec3(-1.0f,  0.0f,  0.0f);
				normals[i * offset++] = glm::vec3( 0.0f,  1.0f,  0.0f);
				normals[i * offset++] = glm::vec3( 0.0f, -1.0f,  0.0f);
			}

			_vbo.addRawData(normals, sizeof(normals));
		}

		// Finally upload data to the GPU
		_vbo.bindVBO();
		_vbo.uploadDataToGPU(GL_STATIC_DRAW);
		setVertexAttributesPointers(_numVerticesTotal);

		_isInitialized = true;
	}

	void Cube::render() const
	{
		if (!_isInitialized) {
			return;
		}

		glBindVertexArray(_vao);

		// Render cylinder side first
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Cube::renderPoints() const
	{
		if (!_isInitialized) {
			return;
		}

		// Just render all points as they are stored in the VBO
		glBindVertexArray(_vao);
		glDrawArrays(GL_POINTS, 0, _numVerticesTotal);
	}

} // namespace static_meshes_3D