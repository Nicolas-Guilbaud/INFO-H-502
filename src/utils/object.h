#pragma once

#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <float.h>  // For FLT_MAX and FLT_MIN

#include <math.h>  // Required for fabsf

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#ifndef OBJECT_H
#define OBJECT_H
#define VTXMASS 1.0f
#define NDEBUG
#define CHARACTERISTIC_LEN 1.0f 
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))

struct Vertex{
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 normal;
};

glm::mat4 btTransformToGlmMat4(const btTransform& transform);

void updateExtremes(float x, float y, float z, float* xmin,float* xmax, float* ymin, float* ymax, float* zmin, float* zmax);

class Object
{	
private:
	bool textureFlag;
	std::vector<Vertex> vertices;

	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 inverse_transpose = model;

	glm::vec3 scaling = glm::vec3(1.0);

	glm::vec3 initial_dimensions = glm::vec3(0.0);

	btConvexHullShape* hull = nullptr; // Convex hull for collision shape

	btRigidBody* rigidBody = nullptr; // Rigid body for dynamics world

	void setHullInit() {
		if (hull) delete hull;

		hull = new btConvexHullShape();
		for (const auto& vertex : vertices) {
			// Transform the vertex positions using the current model matrix
			hull->addPoint(btVector3(vertex.position.x, vertex.position.y, vertex.position.z), false);
		}
		// Scaling the hull according to the characteristic length
		float s = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
		hull->setLocalScaling(btVector3(s, s, s));

		hull->recalcLocalAabb(); // Recalculate the bounding box
	}

	void setModel(const glm::mat4& newModel) {
		model = newModel;
		inverse_transpose = glm::inverse(glm::transpose(model));
	}

public:

	int numVertices;

	GLuint VBO, VAO, texture;

	Object(const char* path) {

		// Read the file defined by the path argument 
		// open the .obj file into a text editor and see how the data are organized
		// you will see line starting by v, vt, vn and f --> what are these ?
		// Then parse it to extract the data you need
		// keep track of the number of vertices you need

		std::string name;
		std::vector<glm::vec3> position, normal;
		std::vector<glm::vec2> textureCoord;

		// v = vertices
		// vt = vertices for the texture
		// vn = normal vertices
		// f = face -> v/vt/vn
		std::ifstream file;
		file.exceptions(std::ifstream::badbit);
		try{
			file.open(path);
			std::string line;
			float xmin = FLT_MAX, xmax = FLT_MIN;
			float ymin = FLT_MAX, ymax = FLT_MIN;
			float zmin = FLT_MAX, zmax = FLT_MIN;
			while(std::getline(file,line)){
				std::istringstream streamedLine(line);
				std::string word;
				streamedLine >> word;
				if(word == "v"){
					float x,y,z;
					float vmass = VTXMASS;
					streamedLine >> x >> y >> z;
					updateExtremes(x,y,z,&xmin,&xmax,&ymin,&ymax,&zmin,&zmax);
					position.push_back(glm::vec3(x,y,z));
				}

				else if(word == "vt"){
					//texture
					float x,y;
					streamedLine >> x >> y;
					textureCoord.push_back(glm::vec2(x,y));
				}

				else if(word == "vn"){
					//normal
					float x,y,z;
					streamedLine >> x >> y >> z;
					normal.push_back(glm::vec3(x,y,z));
				}

				else if(word == "f"){
					//face
					// v/vt/vn
					while(true){

						std::string p,t,n;

						streamedLine >> word;

						if(streamedLine.fail()) break;

						p = word.substr(0,word.find("/"));
						word.erase(0,word.find("/")+1);

						t = word.substr(0,word.find("/"));
						word.erase(0,word.find("/")+1);

						n = word.substr(0,word.find("/"));

						Vertex v;
						v.position = position.at(std::stof(p)-1);
						v.normal = normal.at(std::stof(n)-1);
						v.texture = textureCoord.at(std::stof(t)-1);

						vertices.push_back(v);
					}
				}
			}
			std::cout << "The initial dimensions are x:[" << xmin << ", " << xmax << "] y:[" << ymin << ", " << ymax << "] z:[" << zmin << ", " << zmax << "] " << std::endl;
			initial_dimensions = glm::vec3(fabsf(xmax - xmin), fabsf(ymax - ymin), fabsf(zmax - zmin));
			float s = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
			std::cout << "The normalized dimensions are x:[" << s*xmin << ", " << s*xmax << "] y:[" << s*ymin << ", " << s*ymax << "] z:[" << s*zmin << ", " << s*zmax << "] " << std::endl;
			
		}catch(std::ifstream::failure e){
			std::cout << "ERROR READING '" << path << "' OBJ FILE:\n" << e.what() << std::endl;
		}

		#ifdef NDEBUG

		std::cout << "Loaded " << vertices.size() << " vertices" << std::endl
			<< "- pos: " << position.size() << std::endl
			<< "- texture: " << textureCoord.size() << std::endl
			<< "- normals: " << normal.size() << std::endl;

		#endif

		file.close();
		numVertices = vertices.size();

		setHullInit();
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
		float s = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
		scaling = s * model_scale;
		hull->setLocalScaling(btVector3(scaling.x,scaling.y,scaling.z));
		hull->calculateLocalInertia(mass, inertia);
		

		// Set up the rigid body
		btMotionState* motionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, motionState, hull, inertia);
		rigidBody = new btRigidBody(*rbInfo);
		rigidBody->proceedToTransform(btTransform(rotation, position));

	}

	void setVelocity(glm::vec3 V) {
		if (!rigidBody) setRigidBody(glm::mat4(1.0),1.0);
		rigidBody->setLinearVelocity(btVector3(V.x,V.y,V.z));
	}

	const glm::mat4& getModel() {
		return model;
	}

	void nextFrame() {
		if (!rigidBody) setRigidBody(glm::mat4(1.0), 1.0);
		btTransform updatedTransform;
		rigidBody->getMotionState()->getWorldTransform(updatedTransform);
		setModel(glm::scale(btTransformToGlmMat4(updatedTransform),scaling));// Retrieve updated transform for an object
	}

	const glm::mat4& getInverseTranspose() {
		return inverse_transpose;
	}

	btRigidBody* getRigidBody() {
		return rigidBody;
	}

	void makeObject(Shader shader, bool useTexture = true) {

		//Put your data into your VBO
		float* data = new float[8*numVertices];
		for(int i = 0; i < numVertices; i++){
			//p=3,t=2,n=3
			Vertex v = vertices.at(i);
			data[i*8] = v.position.x;
			data[i*8+1] = v.position.y;
			data[i*8+2] = v.position.z;

			data[i*8+3] = v.texture.x;
			data[i*8+4] = v.texture.y;

			data[i*8+5] = v.normal.x;
			data[i*8+6] = v.normal.y;
			data[i*8+7] = v.normal.z;
		}

		//Create the VAO and VBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		
		//Define VBO and VAO as active buffer and active vertex array
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, data, GL_STATIC_DRAW);

		//Use the VAO to specify how your data should be read by your shader (glVertexAttribPointer and co)
		
		//position
		auto att_pos = glGetAttribLocation(shader.ID, "position");
		glEnableVertexAttribArray(att_pos);
		glVertexAttribPointer(att_pos, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);

		//texture
		if(useTexture){
			auto att_texture = glGetAttribLocation(shader.ID, "tex_coords");
			glEnableVertexAttribArray(att_texture);
			glVertexAttribPointer(att_texture, 2, GL_FLOAT, false, 8 * sizeof(float), (void*) (sizeof(float)*3));
		}
		//normal
		auto att_normal = glGetAttribLocation(shader.ID, "normal");
		glEnableVertexAttribArray(att_normal);
		glVertexAttribPointer(att_normal, 3, GL_FLOAT, false, 8 * sizeof(float), (void*) (sizeof(float)*5));
		

		//Sometimes your shader will not use texture or normal attribute
		//you can use the boolean defined above to not specify these attribute 
		//desactive the buffer and delete the datas when your done
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		delete[] data;
		this->textureFlag = useTexture;
	}

	void changeTexture(GLuint texture){
		if(!textureFlag) 
			throw std::runtime_error("Texture is not allowed on this mesh !");
		
		this->texture = texture;
	}

	void draw() {

		if(textureFlag){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
		}

		//bind your vertex arrays and call glDrawArrays
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		// glDrawElements(GL_TRIANGLES,numVertices,GL_UNSIGNED_INT,0);
	}

	const bool hasTexture() const{
		return this->textureFlag;
	}

	const GLuint getTexture() const{
		return this->texture;
	}
};
#endif

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

void updateExtremes(float x, float y, float z, float* xmin, float* xmax, float* ymin, float* ymax, float* zmin, float* zmax) {
	if (*xmin > x) *xmin = x;
	if (*xmax < x) *xmax = x;
	if (*ymin > y) *ymin = y;
	if (*ymax < y) *ymax = y;
	if (*zmin > z) *zmin = z;
	if (*zmax < z) *zmax = z;
}