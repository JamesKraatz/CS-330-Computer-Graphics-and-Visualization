#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID referece of Elements Buffer Object
	GLuint ID;
	// Constructor that generates an Elements Buffer Object and links it to indices
	EBO(std::vector<GLuint>& indices);

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Delets the EBO
	void Delete();
};

#endif