#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#define sqrt3 1.73205080757

class Game {
private:
	std::vector<glm::vec3> pin_positions = {
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f,1.0f), glm::vec3(0.0f, 0.0f,-1.0f),
		glm::vec3(sqrt3 / 2.0f,0.0f, 0.5f), glm::vec3(sqrt3 / 2.0f, 0.0f,1.5f),
		glm::vec3(sqrt3 / 2.0f,0.0f ,-0.5f), glm::vec3(sqrt3 / 2.0f,  0.0f ,-1.5f),
		glm::vec3(-sqrt3 / 2.0f,0.0f, 0.5f), glm::vec3(-sqrt3 / 2.0f,  0.0f ,-0.5f),
		glm::vec3(-sqrt3,0.0f, 0.0f)
	};

	btDiscreteDynamicsWorld* dynamicsWorld;

	void initializeDynamics() {
		//Initializing the dynamic collision world
		btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
		btDbvtBroadphase* broadphase = new btDbvtBroadphase();
		int numSolvers = 1;
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
		dynamicsWorld->setGravity(btVector3(0, -9.81, 0)); // Set gravity
	}

public:
	const std::vector<glm::vec3> getPinPositions() {
		return pin_positions;
	}

	Game() {
		initializeDynamics();
	}
};