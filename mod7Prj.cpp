#include "ShapeGenerator.h"
#include "ShapeData.h"
#include "CubeGenerator.h"
#include "cylinderGenerator.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "shader.h"
#include "camera.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Pyramid.h"

#include <iostream>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
GLuint loadObject(unsigned int* VAO, unsigned int* VBO, ShapeData *object);
void setSceneLighting(Shader& objectShader, glm::vec3  pointLightPosition);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 12.0f));
//Camera camera(glm::vec3(1.5f, 3.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool perspective = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// offset variables for plane, sphere
const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 9;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

// projection matrix
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

// lighting
// color for direction light "warm sun"
glm::vec3 sunColor = glm::vec3(0.9529f, 0.898f, 0.7373f);
glm::vec3 sunDirection = glm::vec3(-2.0f, -1.0f, 0.5f);
// position for a point lights
glm::vec3 pointLightPosition = glm::vec3(-1.0f, 5.0f, -3.0f);
// point light color 
glm::vec3 pointLightColor = glm::vec3(1.0f, 0.9451f, 0.9176f);
// phong light type multiple
glm::vec3 ambMult = glm::vec3(0.05f, 0.05f, 0.05f);
glm::vec3 diffMult = glm::vec3(0.45f, 0.45f, 0.45f);
glm::vec3 specMult = glm::vec3(0.5f, 0.5f, 0.5f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "James Kraatz: Module 7 Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------

	Shader objectShader("shaderfiles/mod6MlStn.vs", "shaderfiles/mod6MlStn.fs");
	Shader lightShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	Shader clockFaceShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");

	// create plane from ShapeGenerator
	// --------------------------------
	ShapeData myPlane = ShapeGenerator::makePlane(2);
	unsigned int myPlaneVBO{}, myPlaneVAO;
	GLuint myPlaneIndexByteOffset = loadObject(&myPlaneVAO, &myPlaneVBO, &myPlane);

	// load textures using utility function
	unsigned int eggWhite = loadTexture("images/egg.jpg");
	unsigned int green = loadTexture("images/green.jpg");
	unsigned int magentaBoxDiffuseMap = loadTexture("images/magenta.jpg");
	unsigned int brassTexture = loadTexture("images/gold_textured_background_hd_picture_3_169654.jpg");
	unsigned int silverTexture = loadTexture("images/metal_texture_set_03_hd_picture_170837.jpg");
	unsigned int clockDial = loadTexture("images/pngkit_clock-vector-png_1190415.png");
	unsigned int tableSurface = loadTexture("images/tableTopBrown.jpg");
	unsigned int eyeglassCaseTexture = loadTexture("images/eyeglassCaseLtGryTexture.jpg");
	unsigned int hatTexture = loadTexture("images/BlackWavy.jpg");
	unsigned int tassleTexture = loadTexture("images/tassleTexture.jpg");
	unsigned int bookTexture = loadTexture("images/book.jpg");
	unsigned int clockBodyTexture = loadTexture("images/clockBodyTexture.jpg");

	// make objects
	static_meshes_3D::Cube lightCube(0.1f, 0.1f, 0.1f, true, false, false);
	static_meshes_3D::Cube book(0.5f, 3.0f, 4.0f, true, true, true);
	static_meshes_3D::Cube clockBody(1.0f, 1, 0.8f, true, true, true);
	static_meshes_3D::Cylinder clockFace(0.5f, 30, 1.0f, true, true, true);
	static_meshes_3D::Sphere hatBody(30, 15, 1.0f, 0.7f, 0.0f, true, true, true);
	static_meshes_3D::Cube hatTop(0.025f, 2.0f, 2.0f, true, true, true);
	static_meshes_3D::Cylinder hatTopCord(0.025f, 20, 1.10f, true, true, true);
	static_meshes_3D::Cube eyeglassCase(0.5f, 2.5f, 1.0f, true, true, true);
	static_meshes_3D::Pyramid tassle(0.35f, 0.07f, 0.07f, true, true, true);
	static_meshes_3D::Cylinder tassleCord(0.025f, 20, 0.123f, true, true, true);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objectShader.Activate();
		objectShader.setInt("material.diffuse", 0);
		objectShader.setInt("material.specular", 1);

		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setFloat("material.shininess", 32.0f);

		setSceneLighting(objectShader, pointLightPosition);

		// view/projection transformations
		glm::mat4 view = camera.GetViewMatrix();
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		objectShader.setMat4("model", model);

		// setup to draw myPlane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tableSurface);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tableSurface);
		glBindVertexArray(myPlaneVAO);
		model = model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, -0.0001f, 3.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(12.0f, 1.0f, 8.0f));
		objectShader.setMat4("model", model);

		// draw plane
		glDrawElements(GL_TRIANGLES, myPlane.numIndices, GL_UNSIGNED_SHORT, (void*)myPlaneIndexByteOffset);

		// draw book
		// setup to draw book
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bookTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bookTexture);
		model = model = glm::mat4(1.0f);
		// move book up to sit on table plane, and left towards left side of table
		model = glm::translate(model, glm::vec3(-3.0f, book.getHeight() /2.0f, 0.0f));
		// rotate book about y axis in xz plane
		//model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// scale book
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setMat4("model", model);
		// ------------ draw book ------------ //
		book.render();

		// draw clock body
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, clockBodyTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, clockBodyTexture);
		model = model = glm::mat4(1.0f);
		// move clock body right, up, and forward toward viewer
		model = glm::translate(model, glm::vec3(3.0f, clockBody.getHeight() / 2.0f, -1.2f));
		// rotate clock body about y axis
		model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// scale clock body
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		objectShader.setMat4("model", model);
		// ------------ draw clock body ------------ //
		clockBody.render();

		glBindVertexArray(0);


		// draw clock face
		clockFaceShader.Activate();
		clockFaceShader.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brassTexture);
		clockFaceShader.setInt("texture1", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, clockDial);
		clockFaceShader.setInt("texture2", 1);
		
		clockFaceShader.setMat4("projection", projection);
		clockFaceShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		// move clock face right, up, and forward toward viewer
		model = glm::translate(model, glm::vec3(2.975f, clockBody.getHeight() / 2.0f, -1.165f));
		// rotate clock face about y axis
		model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// scale clock face
		// rotate clock about x-axis 
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		clockFaceShader.setMat4("model", model);
		clockFace.render();

		// draw hatbody
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hatTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, hatTexture);
		model = model = glm::mat4(1.0f);
		// move hatbody right, up, and bach away from viewer
		float hatBodyBase = -hatBody.getBottom() / 2.0f;
		float hatBodyTop = hatBodyBase + (hatBody.getTop() - hatBody.getBottom());
		model = glm::translate(model, glm::vec3(0.5f, hatBodyBase, -2.0f));
		// rotate hatbody about y axis "no rotation needed"
		// scale clock body
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		objectShader.setMat4("model", model);
		// ------------ draw hat body ------------ //
		hatBody.render();

		// draw hat top
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hatTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, hatTexture);
		model = model = glm::mat4(1.0f);
		// move hat top right, up, and back away from viewer
		model = glm::translate(model, glm::vec3(0.5f, hatBodyTop, -2.0f));
		// rotate clock body about y axis
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// scale hat top
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		objectShader.setMat4("model", model);
		// ------------ draw hat top ------------ //
		hatTop.render();
		
		// draw hat top cord
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		model = model = glm::mat4(1.0f);
		// move hat top cord right, up, and back away from viewer
		float hatTopCordBase = hatBodyTop + hatTop.getHeight() + (hatTopCord.getRadius() / 2.0f);
		model = glm::translate(model, glm::vec3(0.335f, 0.0f, -0.4f));
		model = glm::translate(model, glm::vec3(0.5f, hatTopCordBase, -2.0f));
		// rotate clock body about y axis
		model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// rotate cord to be horizontal
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		// scale hat top
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		objectShader.setMat4("model", model);
		// ------------ draw hat top cord ------------ //
		hatTopCord.render();

		// draw tassle
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		model = model = glm::mat4(1.0f);
		// move tassle right, up, and back away from viewer
		float tasslePeak = hatBodyTop + hatTop.getHeight() + (hatTopCord.getRadius() / 2.0f);
		model = glm::translate(model, glm::vec3(1.2f, tasslePeak - 0.2f, -2.834f));
		// scale tassle
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setMat4("model", model);
		// ------------ draw tassle ------------ //
		tassle.render();

		// draw tassle cord
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tassleTexture);
		model = model = glm::mat4(1.0f);
		// move tassle cord right, up, and back away from viewer
		model = glm::translate(model, glm::vec3(1.198f, 0.674f, -2.835));
		// rotate tassle coord "no rotation needed"
		// scale tassle cord
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setMat4("model", model);
		// ------------ draw tassle ------------ //
		tassleCord.render();

		// draw eyeglass case
		objectShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, eyeglassCaseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, eyeglassCaseTexture);
		model = model = glm::mat4(1.0f);
		// move eyeglass case right, up, and back away from viewer
		float eyeglassCaseBase = eyeglassCase.getHeight() / 2;
		model = glm::translate(model, glm::vec3(0.75f, eyeglassCaseBase, 1.5f));
		// rotate eyeglass case about y axis
		// scale eyeglass case
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
		objectShader.setMat4("model", model);
		// ------------ draw eyeglass case ------------ //
		eyeglassCase.render();


		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	//glDeleteVertexArrays(1, &myCubeVAO);
	//glDeleteBuffers(1, &myCubeVBO);

	//glDeleteVertexArrays(1, &myCylinderVAO);
	//glDeleteBuffers(1, &myCylinderVBO);

	lightCube.deleteMesh();
	book.deleteMesh();
	clockBody.deleteMesh();
	clockFace.deleteMesh();
	hatBody.deleteMesh();
	hatTop.deleteMesh();
	eyeglassCase.deleteMesh();

	glDeleteVertexArrays(1, &myPlaneVAO);
	glDeleteBuffers(1, &myPlaneVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void setSceneLighting(Shader& shader, glm::vec3  pointLightPosition)
{
	// directional light
	shader.setVec3("dirLight.direction", sunDirection);
	shader.setVec3("dirLight.ambient", ambMult * sunColor);
	shader.setVec3("dirLight.diffuse", diffMult * sunColor);
	shader.setVec3("dirLight.specular", specMult * sunColor);
	// key light
	shader.setVec3("pointLight.position", pointLightPosition);
	shader.setVec3("pointLight.ambient", ambMult * pointLightColor);
	shader.setVec3("pointLight.diffuse", diffMult * pointLightColor);
	shader.setVec3("pointLight.specular", specMult * pointLightColor);
	shader.setFloat("pointLight.constant", 0.01f);
	shader.setFloat("pointLight.linear", 0.049);
	shader.setFloat("pointLight.quadratic", 0.0019);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	// change view between perspective and orthographics
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		const float width = (float)SCR_WIDTH / 2.0f;
		const float height = (float)SCR_HEIGHT / 2.0f;
		const float aspectRatio = width / height;

		if (perspective) {
			projection = glm::ortho(-5.0f * aspectRatio, 5.0f * aspectRatio, -5.0f, 5.0f , 0.1f, 100.0f);
			perspective = false;
		}
		else {
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			perspective = true;
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}

//void loadObject(unsigned int* VAO, unsigned int* VBO, std::vector<float>* shapeData, std::vector<int>* dataStepSize)
GLuint loadObject(unsigned int* VAO, unsigned int* VBO, ShapeData * object)
{
	const int nDataGroups = 3;
	const int nFloats = 3;

	GLsizeiptr currentOffset = 0;
	GLuint indexByteOffset;
	GLuint objectNumIndices;

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, object->vertexBufferSize() + object->indexBufferSize(), 0, GL_STATIC_DRAW);
	currentOffset = 0;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, object->vertexBufferSize(), object->vertices);
	currentOffset += object->vertexBufferSize();
	indexByteOffset = currentOffset;
	glBufferSubData(GL_ARRAY_BUFFER, currentOffset, object->indexBufferSize(), object->indices);
	objectNumIndices = object->numIndices;


	for (int i = 0; i < nDataGroups; i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, nFloats, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(sizeof(float) * (i * nFloats)));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *VBO);

	return indexByteOffset;
}
