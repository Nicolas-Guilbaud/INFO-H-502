
#include "object.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#define VTXMASS 1.0f
#define NDEBUG
#define CHARACTERISTIC_LEN 1.0f 
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))


class rigidObject : public Object{
private:
	glm::vec3 scaling = glm::vec3(1.0);

	btConvexHullShape* hull = nullptr; // Convex hull for collision shape
	btCompoundShape* compound = new btCompoundShape(); // Wrapping the hull around the rigid body
	btRigidBody* rigidBody = nullptr; // Rigid body for dynamics world

	void setHullInit(bool isCubic) {
		if (hull) delete hull;

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
				hull->addPoint(vertices[i]+btVector3(mesh.getInitialCtr().x, mesh.getInitialCtr().y, mesh.getInitialCtr().z));
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
	}

public:
	rigidObject(Mesh mesh, bool isCubic) : Object(mesh) { // constructor
		setHullInit(isCubic);
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
		hull->calculateLocalInertia(mass, inertia);

		// Set up the rigid body
		btMotionState* motionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, hull, inertia);
		rigidBody = new btRigidBody(*rbInfo);
		rigidBody->proceedToTransform(btTransform(rotation, position));



	}
	void setVelocity(glm::vec3 V) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		rigidBody->setLinearVelocity(btVector3(V.x, V.y, V.z));
	}

	void draw(Camera c, Light l) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		btTransform updatedTransform;
		rigidBody->getMotionState()->getWorldTransform(updatedTransform);
		setModel(glm::scale(btTransformToGlmMat4(updatedTransform), scaling));// Retrieve updated transform for an object
		Object::draw(c,l); //classic draw routine
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
};
