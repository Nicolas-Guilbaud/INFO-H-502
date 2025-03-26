// standard C++ include files
#include <stdio.h>
#include <math.h>  // Required for sqrt
#include <thread>
#include <chrono>

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
#include "../objects/object.h"
#include "../objects/rigidObject.cpp"
#include "../objects/reflectableObject.cpp"
#include "../objects/refractableObject.cpp"
#include "../objects/Cubemap.cpp"
#include "../collisions/MyContactResultCallback.cpp"
#include "../utils/Texture.h"
#include "../objects/MirrorFace.cpp"
#include "../utils/Fresnel.h"

#define PRINT_VEC3(vec) "(" << vec.x << "," << vec.y << "," << vec.z << ")"

const int width = 500;
const int height = 500;

bool start_dynamics = false;
bool firstMouse = true;
float lastX = width / 2.0f;
float lastY = height / 2.0f;

std::vector<Camera> theCameras = {
	Camera(glm::vec3(0.0f, 10.0f, 2.0f),glm::vec3(0.0,0.25,-1.0),-90.0,-75.0, false),
	Camera(glm::vec3(3.75f, 8.6f, 8.4f), glm::vec3(-0.3, 0.82, -0.5), -125.0, -35.0, false),
	Camera(glm::vec3(-32.0f, 7.0f, 4.3f),glm::vec3(0.30, 0.90, -0.05), -14.0, -28.0, false),
	Camera(glm::vec3(0.0f, 0.0f, 10.0f))
};
int camIdx = 1;
Camera camera = theCameras[camIdx];
MirrorFace* mirror;

void windowResizeCallback(GLFWwindow* window, int width, int height);
void processKeyInput(GLFWwindow* window, rigidObject& obj);
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
void DetectCollisions(btDiscreteDynamicsWorld* dWorld);
void addGround(btDynamicsWorld* d_world, Object* grd);
void switchCameras();


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
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	#ifdef NDEBUG
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //GLFW_CURSOR_DISABLED
	#else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	#endif
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

	//Cubemap
	GLuint cubemapTex = loadCubemap(PATH_TO_TEXTURES "/cubemap");
	Cubemap cubemapObj(cubemapTex);

    //TODO: Shader
	Shader shader(PATH_TO_SHADERS "/cube.vert", PATH_TO_SHADERS "/cube.frag");
	GLuint ballTex = loadTexture(PATH_TO_TEXTURES "/bowling_ball.jpg");
	
	//Creating the objects
	std::vector<rigidObject> objects;
	
	Fresnel F(PATH_TO_OTHERS "/Fresnel.txt");
	std::vector<std::string> material_List = F.getMaterialList();

	#ifndef NDEBUG
	for (const auto& mat : material_List) {
		glm::vec3 fresnel = F.getFresnelValue(mat); // Avoid redundant calls

		std::cout << "Material available: " << mat
			<< " | Associated Fresnel Reflectances: ("
			<< fresnel.x << ", " << fresnel.y << ", " << fresnel.z << ")"
			<< std::endl;
	}
	#endif

	//bowling ball
	glm::mat4 model = glm::mat4(1.0);
	model = glm::scale(glm::translate(model, glm::vec3(-25.0, 0.0, 0.0)), glm::vec3(0.1));
	Mesh ballMesh(PATH_TO_MESHES "/Bowling_Ball_Clean.obj",shader,ballTex);
	rigidObject ball(ballMesh, true, F.getFresnelValue("iron"), model, 10.0);

	ball.setVelocity(glm::vec3(10.0, 0.0, 0.0));
	ball.getRigidBody()->setFriction(0.0);

	objects.push_back(ball);

	// bowling pins
	float sqrt3 = sqrtf(3.0f);  // Compute sqrt(3) once as a float

	std::vector<glm::vec3> pin_positions = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,1.0f), glm::vec3(0.0f, 0.0f,-1.0f),
		glm::vec3(sqrt3 / 2.0f,0.0f, 0.5f), glm::vec3(sqrt3 / 2.0f, 0.0f,1.5f),
		glm::vec3(sqrt3 / 2.0f,0.0f ,-0.5f), glm::vec3(sqrt3 / 2.0f,  0.0f ,-1.5f),
		glm::vec3(-sqrt3 / 2.0f,0.0f, 0.5f), glm::vec3(-sqrt3 / 2.0f,  0.0f ,-0.5f),
		glm::vec3( -sqrt3,0.0f, 0.0f)
	};

	GLuint pinTex = loadTexture(PATH_TO_TEXTURES "/bowling_pin.jpg");
	Mesh pinMesh(PATH_TO_MESHES "/PinSmooth.obj",shader,pinTex);

	for (auto& pos : pin_positions) {
		rigidObject pin(pinMesh, true, F.getFresnelValue("zinc"), glm::scale(glm::translate(glm::mat4(1.0), pos), glm::vec3(1.0, 1.0, 1.0)), 0.5);
		objects.push_back(pin);
	}

	//ground
	GLuint groundTex = loadTexture(PATH_TO_TEXTURES "/ground.jpg");
	Mesh groundMesh(PATH_TO_MESHES "/sol.obj",shader,groundTex);
	Object ground(groundMesh);
	ground.translateModel(0,-1,0);

	Mesh mirrorMesh(PATH_TO_MESHES "/mirror.obj",shader);
	mirror = new MirrorFace(width,height);
	mirror->scaleModel(5.0,5.0,5.0);
	mirror->translateModel(5.0,1.0,0.0);
	mirror->rotateModel(-90,glm::vec3(0,1,0));


	// reflecting sphere
	Shader shader_reflect(PATH_TO_SHADERS "/ref.vert", PATH_TO_SHADERS "/reflect.frag");
	Mesh reflectSphereMesh(PATH_TO_MESHES "/sphere_smooth.obj", shader_reflect, cubemapTex);
	reflectableObject reflectSphere(reflectSphereMesh);
	glm::mat4 rectSphModel = glm::mat4(1);
	rectSphModel = glm::scale(glm::translate(model, glm::vec3(-30, 25, 0)),glm::vec3(10.0));
	reflectSphere.setModel(rectSphModel);

	// refracting sphere
	Shader shader_refract(PATH_TO_SHADERS "/ref.vert", PATH_TO_SHADERS "/refract.frag");
	Mesh refractSphereMesh(PATH_TO_MESHES "/sphere_smooth.obj", shader_refract, cubemapTex);
	refractableObject refractSphere(refractSphereMesh);
	glm::mat4 ractSphModel = glm::mat4(1);
	ractSphModel = glm::scale(glm::translate(model, glm::vec3(30, 25, 0)), glm::vec3(10.0));
	refractSphere.setModel(ractSphModel);

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

	addGround( dynamicsWorld, &ground);

	const Light l(glm::vec3(1.0, 2.0, 2.0), glm::vec3(2.0));	
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

	//Rendering

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		double now = glfwGetTime();

		//Mirror texture rendering
		Camera trueState = camera; //keep true camera state
		mirror->placeCamera(&camera); //setup camera for mirror rendering
		mirror->beginMirrorRendering();

		glDepthFunc(GL_LEQUAL);
		cubemapObj.draw(camera,l);
		glDepthFunc(GL_LESS);
		// draw objects
		for (rigidObject o : objects) {
			o.draw(camera,l,start_dynamics);
		}

		ground.draw(camera,l);

		mirror->endMirrorRendering();
		camera = trueState;

		//main rendering

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processKeyInput(window, ball);

		glDepthFunc(GL_LEQUAL);
		cubemapObj.draw(camera,l);
		refractSphere.draw(camera, l, cubemapTex, 1.33f);
		reflectSphere.draw(camera, l, cubemapTex);
		glDepthFunc(GL_LESS);
		// draw objects
		for (rigidObject o : objects) {
			o.draw(camera,l,start_dynamics);
		}

		ground.draw(camera,l);
		mirror->draw(camera,l);


		glfwSwapBuffers(window);

		float fpsCount = (float)fps(now);
		// Step simulation
		float timeStep = 1.0f / 60.0; // 60 FPS
		int maxSubSteps = 5; // More substeps = better physics accuracy
		if(start_dynamics) dynamicsWorld->stepSimulation(timeStep, maxSubSteps);

		//DetectCollisions(dynamicsWorld); // high cost, slows down computations significantly
	}

	delete mirror;

	//clean up ressource
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}



void windowResizeCallback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
}

void processKeyInput(GLFWwindow* window, rigidObject& ball) {
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

	//Launch ball with enter 
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		start_dynamics = true;

	//Switch cameras with TAB
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		switchCameras();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	#ifndef NDEBUG
	//Debug MirrorFace operations on the camera
	if(glfwGetKey(window,GLFW_KEY_T) == GLFW_PRESS){
		glm::vec3 oldP = camera.Position;
		std::cout << "Reverting camera !" << std::endl
		<< "Old cam pos:" << PRINT_VEC3(camera.Position) << std::endl
		<< "\t pitch: " << camera.Pitch << ", Yaw: " << camera.Yaw << std::endl
		<< "mirror pos:" << PRINT_VEC3(mirror->getPosition()) << std::endl;

		mirror->placeCamera(&camera);

		std::cout << "New cam pos: " << PRINT_VEC3(camera.Position) << std::endl
		<< "\t pitch: " << camera.Pitch << ", Yaw: " << camera.Yaw << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	#endif

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

	btCollisionObjectArray c_obj_arr = d_world->getCollisionObjectArray();
	MyContactResultCallback resultCallback;
	int arr_size = c_obj_arr.size();
	for (int i = 0; i < arr_size-1; i++) {
		for (int j = i + 1; j < arr_size; j++) {
			d_world->contactPairTest(c_obj_arr[i],c_obj_arr[j], resultCallback);
		}
	}
}

void addGround(btDynamicsWorld* d_world, Object* grd) {
	// Create a box shape for the ground 
	glm::vec3 dims = grd->getInitialDimensions();
	glm::vec3 ctr = grd->getInitialCenter();
	glm::vec3 offset = grd->getModel()[3];
	/*
	std::cout << "Ground center: " << ctr.x << ", " << ctr.y << ", " << ctr.z << std::endl;
	std::cout << "Ground offset: " << offset.x << ", " << offset.y << ", " << offset.z << std::endl;
	std::cout << "Ground dimensions: " << dims.x << ", " << dims.y << ", " << dims.z << std::endl;
	*/
	btCollisionShape* groundShape = new btBoxShape(btVector3(dims.x, dims.y, dims.z));

	glm::vec3 trans = glm::vec3(ctr.x - dims.x / 2 + offset.x, ctr.y - dims.y / 2 + offset.y, ctr.z - dims.z / 2 + offset.z);
	//std::cout << "ground hitbox translation: " <<  trans.x << ", " << trans.y << ", " << trans.z << std::endl;
	// Set up transformation (position it at y = -dims.y to align top at y = 0)
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(trans.x,trans.y,trans.z));

	// Create a motion state
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(groundTransform);

	// Set mass to 0 (static object)
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setRestitution(0.5f);

	// Add to the dynamics world
	d_world->addRigidBody(groundRigidBody);
}

void switchCameras() {
	camera = theCameras[(++camIdx) % theCameras.size()];
}