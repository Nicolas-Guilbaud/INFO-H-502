// standard C++ include files
#include<iostream>

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//user-defined header files
#include "../utils/camera.h"
#include "../utils/shader.h"
#include "../utils/object.h"
#include "../collisions/MyContactResultCallback.cpp"
#include "../utils/Texture.h"

const int width = 500;
const int height = 500;

bool firstMouse = true;
float lastX = width / 2.0f;
float lastY = height / 2.0f;

Camera camera(glm::vec3(0.0, 0.0, 0.1));
glm::vec3 processKeyInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void DetectCollisions(btCollisionWorld* cWorld);

#ifndef NDEBUG
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}
#endif

int main(int argc, char* argv[]){

    //Boilerplate
	//Create the OpenGL context 
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW \n");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifndef NDEBUG
	//create a debug context to help with Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif


	//Create the window
	GLFWwindow* window = glfwCreateWindow(width, height, "Solution 01", nullptr, nullptr);
	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window\n");
	}

	glfwMakeContextCurrent(window);
	/* Motivated student can implement the rotation using the mouse */
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //GLFW_CURSOR_DISABLED
	//load openGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

#ifndef NDEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif

    glEnable(GL_DEPTH_TEST);

    //TODO: Shader
	Shader shader(PATH_TO_SHADERS "/cube.vert", PATH_TO_SHADERS "/cube.frag");

	//Creating the objects
	Object cube(PATH_TO_MESHES "/cube.obj");
	cube.makeObject(shader,false);
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(10, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 1, 0));
	model = glm::scale(model, glm::vec3(0.5, 0.5, 1.0));
	cube.setModel(model);

	Object sphere(PATH_TO_MESHES "/Bowling_Ball_Clean.obj");
	sphere.makeObject(shader, true);
	GLuint ballTex = loadTexture(PATH_TO_TEXTURES "/bowling_ball.jpg");
	sphere.changeTexture(ballTex);

	model = glm::mat4(1.0);
	model = glm::scale(model,glm::vec3(0.1));
	sphere.setModel(model);

	//TEXTURE
	Object sphere_coarse(PATH_TO_MESHES "/PinAvg.obj");
	sphere_coarse.makeObject(shader, true);
	GLuint pinTex = loadTexture(PATH_TO_TEXTURES "/bowling_pin.jpg");
	sphere_coarse.changeTexture(pinTex);

	model = glm::mat4(1.0);
	model = glm::translate(model,glm::vec3(5.0,0.0,0.0));
	model = glm::scale(model, glm::vec3(0.1));

	sphere_coarse.setModel(model);

	std::vector<Object> objects;
	objects.push_back(cube);
	objects.push_back(sphere);
	objects.push_back(sphere_coarse);

	//Initializing the collision world
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btDbvtBroadphase* broadphasePairCache = new btDbvtBroadphase();
	btCollisionWorld* collisionWorld = new btCollisionWorld(dispatcher, broadphasePairCache, collisionConfiguration);
	for (auto& obj: objects) {
		collisionWorld->addCollisionObject(obj.getCollisionObject());
	}

	const glm::vec3 light_pos = glm::vec3(1.0, 2.0, 2.0);
	
	double prev = 0;
	int deltaFrame = 0;
	//fps function
	auto fps = [&](double now) {
		double deltaTime = now - prev;
		deltaFrame++;
		if (deltaTime > 0.5) {
			prev = now;
			const double fpsCount = (double)deltaFrame / deltaTime;
			deltaFrame = 0;
			std::cout << "\r FPS: " << fpsCount;
			std::cout.flush();
		}
	};

	const btCollisionObject* cubeCObj = cube.getCollisionObject();
	btTransform cubeWT = cubeCObj->getWorldTransform();
	btMatrix3x3 cubeBasis = cubeWT.getBasis();
	btVector3 basisColumns[3] = {cubeBasis.getColumn(0),cubeBasis.getColumn(1),cubeBasis.getColumn(2)};
	btVector3 cubeOrigin = cubeWT.getOrigin();

	/*
	std::cout << "origin in the collision world: " << cubeOrigin.getX() <<" " << cubeOrigin.getY() << " " << cubeOrigin.getZ() <<"\n";
	std::cout << "first column of the rotation basis in collision world: " << basisColumns[0].getX() << " " << basisColumns[0].getY() << " " << basisColumns[0].getZ() << "\n";
	std::cout << "second column of the rotation basis in collision world: " << basisColumns[1].getX() << " " << basisColumns[1].getY() << " " << basisColumns[1].getZ() << "\n";
	std::cout << "third column of the rotation basis in collision world: " << basisColumns[2].getX() << " " << basisColumns[2].getY() << " " << basisColumns[2].getZ() << "\n";
	*/

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 perspective = camera.GetProjectionMatrix();

	//Rendering

	while (!glfwWindowShouldClose(window)) {
		
		view = camera.GetViewMatrix();
		glfwPollEvents();
		double now = glfwGetTime();
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		shader.use();

		shader.setMatrix4("V", view);
		shader.setMatrix4("P", perspective);
		shader.setVector3f("u_view_pos", camera.Position);
		shader.setVector3f("u_light_pos", light_pos);

		//glDrawArrays(GL_TRIANGLES, 0, 12);

		glm::vec3 trans = processKeyInput(window);
		objects[0].setModel(glm::translate(objects[0].getModel(), trans));

		// draw objects
		for (Object o : objects) {
			shader.setMatrix4("M", o.getModel());
			shader.setMatrix4("itM", o.getInverseTranspose());
			if(o.hasTexture()){
				shader.setTexture("img",0);
			}
			o.draw();
		}

		//fps(now);
		glfwSwapBuffers(window);

		DetectCollisions(collisionWorld);
	}

	//clean up ressource
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}

glm::vec3 processKeyInput(GLFWwindow* window) {
	//Close window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Left & Right
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(LEFT, 0.1);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(RIGHT, 0.1);

	//Front & Back
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(FORWARD, 0.1);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//std::cout << "S" << std::endl;
		camera.ProcessKeyboardMovement(BACKWARD, 0.1);
	}

	//Up & Down
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(UP, 0.1);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboardMovement(DOWN, 0.1);

	glm::vec3 trans = glm::vec3(0.0, 0.0, 0.0);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	 	trans+=glm::vec3(1.0, 0.0, 0.0) / 10.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		trans += glm::vec3(-1.0, 0.0, 0.0) / 10.0f;

	 if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		 trans += glm::vec3(0.0, 0.0, 1.0) / 10.0f;
	 if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		 trans += glm::vec3(0.0, 0.0, -1.0)/10.0f;

	 return trans;
}

/* Motivated student can implement the rotation using the mouse
* You can find a strating code in these comments
*/
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn){
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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

void DetectCollisions(btCollisionWorld* c_world) {
	/*
	c-world->performDiscreteCollisionDetection();
	int numManifolds = c_world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* manifold = c_world->getDispatcher()->getManifoldByIndexInternal(i);
		// Process each contact point in the manifold
	}*/

	btCollisionObjectArray c_obj_arr = c_world->getCollisionObjectArray();
	MyContactResultCallback resultCallback;
	int arr_size = c_obj_arr.size();
	for (int i = 0; i < arr_size-1; i++) {
		for (int j = i + 1; j < arr_size; j++) {
			c_world->contactPairTest(c_obj_arr[i],c_obj_arr[j], resultCallback);
		}
	}
}