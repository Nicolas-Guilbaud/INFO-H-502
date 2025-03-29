
#include "object.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#define VTXMASS 1.0f
#define NDEBUG
#define CHARACTERISTIC_LEN 1.0f 
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#define PRINT_VEC4(vec) "(" << vec.x << "," << vec.y << "," << vec.z << "," << vec[3] << ")"

class rigidObject : public Object{
private:
	int i = 0;
	glm::vec3 scaling = glm::vec3(1.0);
	btConvexHullShape* hull = nullptr; // Convex hull for collision shape
	glm::mat4 localTransform = glm::mat4(1.0);
	btRigidBody* rigidBody = nullptr; // Rigid body for dynamics world
	bool cubic = false;
	bool on_the_side = false;

	void setHullInit(bool isCubic) {
		if (hull) delete hull;

		cubic = isCubic;
		hull = new btConvexHullShape();


		if (isCubic) {
			btVector3 halfExtents(mesh.getInitialDims().x * 0.5f, mesh.getInitialDims().y * 0.5f, mesh.getInitialDims().z * 0.5f);
			btVector3 vertices[8] = {
			btVector3(-halfExtents.x(), -halfExtents.y(), -halfExtents.z()),
			btVector3(halfExtents.x(), -halfExtents.y(), -halfExtents.z()),
			btVector3(-halfExtents.x(),  halfExtents.y(), -halfExtents.z()),
			btVector3(halfExtents.x(),  halfExtents.y(), -halfExtents.z()),
			btVector3(-halfExtents.x(), -halfExtents.y(),  halfExtents.z()),
			btVector3(halfExtents.x(), -halfExtents.y(),  halfExtents.z()),
			btVector3(-halfExtents.x(),  halfExtents.y(),  halfExtents.z()),
			btVector3(halfExtents.x(),  halfExtents.y(),  halfExtents.z())
			};
			for (int i = 0; i < 8; i++) {
				hull->addPoint(vertices[i]);
			}
		}
		else {
			for (const auto& vertex : mesh.getVertices()) {
				// Transform the vertex positions using the current model matrix
				hull->addPoint(btVector3(vertex.position.x, vertex.position.y, vertex.position.z), false);
			}
			// Scaling the hull according to the characteristic length
			float s = CHARACTERISTIC_LEN / MAX3(mesh.getInitialDims().x, mesh.getInitialDims().y, mesh.getInitialDims().z);
			hull->setLocalScaling(btVector3(s, s, s));

			hull->recalcLocalAabb(); // Recalculate the bounding box
		}
		hull->setMargin(0.1f);
	}
	void setRigidBody(const glm::mat4& nextModel, float m) { // set the RigidBody in the world coordinates according to nextModel
		if (rigidBody) delete rigidBody;

		btScalar mass = (btScalar)m;

		// Extract translation (last column of the matrix)
		btVector3 position(nextModel[3][0], nextModel[3][1], nextModel[3][2]);

		// Extract rotation (upper-left 3x3 part)
		glm::mat3 rotationMatrix(nextModel);
		glm::quat glmQuat = glm::quat_cast(rotationMatrix);
		btQuaternion rotation(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);

		// Extract user-imposed scaling
		glm::vec3 model_scale = glm::vec3(glm::length(glm::vec3(nextModel[0])),
			glm::length(glm::vec3(nextModel[1])),
			glm::length(glm::vec3(nextModel[2])));

		// Computes inertia for dynamic objects after rescaling the hull
		btVector3 inertia(0, 0, 0);
		glm::vec3 initial_dimensions = mesh.getInitialDims();
		float s = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
		scaling = s * model_scale;
		hull->setLocalScaling(btVector3(scaling.x, scaling.y, scaling.z));

		// Define the local translation
		localTransform = glm::translate(localTransform, -glm::vec3(mesh.getNormalizedCtr().x * model_scale.x, mesh.getNormalizedCtr().y * model_scale.y, mesh.getNormalizedCtr().z * model_scale.z)); // Shift the hull upward by offsetY
			
		// Set up the rigid body
		btMotionState* motionState = new btDefaultMotionState();
		hull->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, hull, inertia);
		rigidBody = new btRigidBody(*rbInfo);
		rigidBody->proceedToTransform(btTransform(rotation, position));

		setModel(glm::scale(nextModel, glm::vec3(s)));
		
	}

public:
	rigidObject(Mesh mesh, bool isCubic, glm::vec3 F0 = glm::vec3(1.0), const glm::mat4& nextModel = glm::mat4(1.0), float m = 1.0) : Object(mesh, F0) { // constructor
		setHullInit(isCubic);
		setRigidBody(nextModel, m);
	}

	void setVelocity(glm::vec3 V) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		rigidBody->setLinearVelocity(btVector3(V.x, V.y, V.z));
	}
	void setGravity(glm::vec3 V) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		rigidBody->setGravity(btVector3(V.x, V.y, V.z));
	}

	void draw(Camera c, Light l, bool start_dynamics, bool printMode = false) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		glm::vec3 initial_dimensions = mesh.getInitialDims();
		float s = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
		if(start_dynamics){ // update model calculated by bullet dynamics world
			btTransform updatedTransform;
			rigidBody->getMotionState()->getWorldTransform(updatedTransform);
			glm::mat4 updatedGlmTransform = btTransformToGlmMat4(updatedTransform);
			if(++i%1000==0 && printMode) printVerticalInclination(updatedGlmTransform);
			on_the_side = rotGreater(80.0f, updatedGlmTransform);
			if (cubic) {
				setModel(glm::scale(updatedGlmTransform * localTransform,scaling));// Retrieve updated transform for an object
			}
			else {
				setModel(glm::scale(btTransformToGlmMat4(updatedTransform), scaling));// Retrieve updated transform for an object
			}
		}
		if (++i % 1000 == 0 && printMode) printVerticalInclination(glm::scale(this->model,1.0f/scaling));
		Object::draw(c, l); //classic draw routine
	}

	bool rotGreater(float angle, glm::mat4 model) {
		bool greater = false;
		glm::vec3 y_dir = glm::normalize(glm::vec3(model[1])); // transformed local Y axis
		glm::vec3 world_up(0.0f, 1.0f, 0.0f);

		float dot_product = glm::dot(y_dir, world_up);
		dot_product = glm::clamp(dot_product, -1.0f, 1.0f); // Ensure domain of acos is valid

		float angle_rad = glm::acos(dot_product); // angle with world Y axis
		float angle_deg = glm::degrees(angle_rad);
		// Check for rotation greater than given degrees
		if (std::abs(angle_deg) > angle) {
			// More than angle rotation around Y
			greater = true;
		}
		return greater;
	};

	void printVerticalInclination(const glm::mat4 model) {
		glm::vec3 y_dir = glm::normalize(glm::vec3(model[1])); // transformed local Y axis
		glm::vec3 world_up(0.0f, 1.0f, 0.0f);

		float dot_product = glm::dot(y_dir, world_up);
		dot_product = glm::clamp(dot_product, -1.0f, 1.0f); // Ensure domain of acos is valid

		float angle_rad = glm::acos(dot_product); // angle with world Y axis
		float angle_deg = glm::degrees(angle_rad);

		std::cout << "Inclination angle with respect to vertical (Y): " << angle_deg << " degrees" << std::endl;
	}

	void printModel(glm::mat4 mdl) {
		std::cout << "\nmodel 0: " << PRINT_VEC4((mdl)[0]) << std::endl;
		std::cout << "model 1: " << PRINT_VEC4((mdl)[1]) << std::endl;
		std::cout << "model 2: " << PRINT_VEC4((mdl)[2]) << std::endl;
		std::cout << "model 3: " << PRINT_VEC4((mdl)[3]) << std::endl;
	}

	btRigidBody* getRigidBody() {
		return rigidBody;
	}

	glm::mat4 btTransformToGlmMat4(const btTransform& transform) {
		glm::mat4 matrix(1.0f); // Identity matrix

		// Extract rotation  from btMatrix3x3
		const btMatrix3x3& basis = transform.getBasis();

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				matrix[j][i] = basis[i][j]; // Transpose because Bullet uses row-major order
			}
		}

		// Extract translation
		btVector3 origin = transform.getOrigin();
		matrix[3][0] = origin.x();
		matrix[3][1] = origin.y();
		matrix[3][2] = origin.z();

		return matrix;
	}

	bool isOnTheSide() {
		return on_the_side;
	}
};

