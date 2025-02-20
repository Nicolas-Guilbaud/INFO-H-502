// standard C++ include files
#include <stdio.h>
#include <math.h>  // Required for sqrt

//include glad before GLFW to avoid header conflict or define "#define GLFW_INCLUDE_NONE"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
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
void processKeyInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void DetectCollisions(btDiscreteDynamicsWorld* dWorld);
void addGround(btDynamicsWorld* d_world);

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
	glm::mat3 permutation = glm::mat3(1.0);
	glm::vec3 temp = permutation[1];
	permutation[1] = permutation[2];
	permutation[2] = temp;
	Object ball(PATH_TO_MESHES "/Bowling_Ball_Clean.obj");
	ball.makeObject(shader, false);
	glm::mat4 model = glm::mat4(1.0);
	model = glm::scale(glm::translate(model, permutation*glm::vec3(-150.0,0.0,0.0)),glm::vec3(1.0));
	ball.setRigidBody(model, 10.0);
	ball.getRigidBody()->setDamping(0.0f, 0.0f);
	ball.getRigidBody()->setFriction(0.0f);

	ball.setVelocity(permutation * glm::vec3(50.0, 0.0, 0.0));

	std::vector<Object> objects;
	objects.push_back(ball);

	float sqrt3 = sqrtf(3.0f);  // Compute sqrt(3) once as a float

	std::vector<glm::vec3> pin_positions = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, sqrt3 / 2.0f, 0.0f), glm::vec3(1.5f, sqrt3 / 2.0f, 0.0f),
		glm::vec3(-0.5f, sqrt3 / 2.0f, 0.0f), glm::vec3(-1.5f, sqrt3 / 2.0f, 0.0f),
		glm::vec3(0.5f, -sqrt3 / 2.0f, 0.0f), glm::vec3(-0.5f, -sqrt3 / 2.0f, 0.0f),
		glm::vec3(0.0f, -sqrt3, 0.0f)
	};

	for (auto& pos : pin_positions) {
		Object pin(PATH_TO_MESHES "/PinSmooth.obj");
		pin.makeObject(shader, false);
		pin.setRigidBody(glm::scale(glm::translate(glm::mat4(1.0), permutation*pos), glm::vec3(1.5, 1.5, 1.5)),1.0);
		pin.getRigidBody()->setRestitution(0.1f);
		pin.getRigidBody()->setFriction(0.3f);
		pin.getRigidBody()->setDamping(0.1f, 0.1f);
		objects.push_back(pin);
	}

	//Initializing the dynamic collision world
	btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	int numSolvers = 1;
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	//dynamicsWorld->setGravity(btVector3(0.0,0.0,0.0));
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0)); // Set gravity

	for (auto& obj: objects) {
		dynamicsWorld->addRigidBody(obj.getRigidBody());
	}

	addGround( dynamicsWorld);

	const glm::vec3 light_pos = glm::vec3(1.0, 2.0, 2.0);
	
	double prev = 0;
	int deltaFrame = 0;
	//fps function
	auto fps = [&](double now) -> double {
		double deltaTime = now - prev;
		deltaFrame++;
		if (deltaTime > 0.5) {
			prev = now;
			const double fpsCount = static_cast<double>(deltaFrame) / deltaTime;
			deltaFrame = 0;
			std::cout << "\r FPS: " << fpsCount;
			std::cout.flush();
			return fpsCount;
		}
		return -1.0; // Return a sentinel value when FPS is not updated
		};

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

		processKeyInput(window);

		// draw objects
		for (Object o : objects) {
			o.nextFrame();
			shader.setMatrix4("M", o.getModel());
			shader.setMatrix4("itM", o.getInverseTranspose());
			if(o.hasTexture()){
				shader.setTexture("img",0);
			}
			o.draw();
		}

		glfwSwapBuffers(window);

		float fpsCount = (float)fps(now);
		// Step simulation
		float timeStep = 1.0f / 60.0; // 60 FPS
		int maxSubSteps = 5; // More substeps = better physics accuracy
		dynamicsWorld->stepSimulation(timeStep, maxSubSteps);

		//DetectCollisions(dynamicsWorld); // high cost, slows down computations significantly
	}

	//clean up ressource
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}

void processKeyInput(GLFWwindow* window) {
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

void DetectCollisions(btDiscreteDynamicsWorld* d_world) {
	/*
	c-world->performDiscreteCollisionDetection();
	int numManifolds = c_world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold* manifold = c_world->getDispatcher()->getManifoldByIndexInternal(i);
		// Process each contact point in the manifold
	}*/

	btCollisionObjectArray c_obj_arr = d_world->getCollisionObjectArray();
	MyContactResultCallback resultCallback;
	int arr_size = c_obj_arr.size();
	for (int i = 0; i < arr_size-1; i++) {
		for (int j = i + 1; j < arr_size; j++) {
			d_world->contactPairTest(c_obj_arr[i],c_obj_arr[j], resultCallback);
		}
	}
}

void addGround(btDynamicsWorld* d_world) {
	// Create a box shape for the ground (size 500x1x500)
	btCollisionShape* groundShape = new btBoxShape(btVector3(500, 1, 500));

	// Set up transformation (position it at y = -1 to align top at y = 0)
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -1, 0));

	// Create a motion state
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);

	// Set mass to 0 (static object)
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(0.1f);

	// Add to the dynamics world
	d_world->addRigidBody(groundRigidBody);
}