#pragma once

#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

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

struct Vertex{
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 normal;
};

class Object
{	
private:
	bool textureFlag;
	std::vector<Vertex> vertices;

	glm::vec3 ctr_of_mass = glm::vec3(0.0);
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 inverse_transpose = model;

	btConvexHullShape* hull = nullptr; // Convex hull for collision shape

	btCollisionObject* object = new btCollisionObject(); // Collision object

	void setCollisionInit() {
		if (hull) delete hull;

		hull = new btConvexHullShape();
		for (const auto& vertex : vertices) {
			// Transform the vertex positions using the current model matrix
			glm::vec4 transformedPos = model * glm::vec4(vertex.position, 1.0f);
			hull->addPoint(btVector3(transformedPos.x, transformedPos.y, transformedPos.z), false);
		}
		hull->recalcLocalAabb(); // Recalculate the bounding box
		object->setCollisionShape(hull); // Recalculate the collision object
		btTransform identityTransform;
		identityTransform.setIdentity();
		identityTransform.setOrigin(btVector3( 0.0, 0.0, 0.0));
		object->setWorldTransform(identityTransform);
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
			while(std::getline(file,line)){
				std::istringstream streamedLine(line);
				std::string word;
				streamedLine >> word;
				if(word == "v"){
					float x,y,z;
					float vmass = VTXMASS;
					streamedLine >> x >> y >> z;
					position.push_back(glm::vec3(x,y,z));
					ctr_of_mass = ctr_of_mass + glm::vec3(x, y, z)*vmass;
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
		ctr_of_mass = ctr_of_mass / (numVertices*VTXMASS);
		std::cout << "Object with center of mass " << ctr_of_mass.x << " " << ctr_of_mass.y << " " << ctr_of_mass.z << " " << "set.\n";
		
		setCollisionInit();
	}

	void setModel(const glm::mat4& newModel) {
		model = newModel;
		inverse_transpose = glm::inverse(glm::transpose(model));
		ctr_of_mass = glm::vec3(model * glm::vec4(ctr_of_mass, 1.0));
		//std::cout << "Updated center of mass " << ctr_of_mass.x << " " << ctr_of_mass.y << " " << ctr_of_mass.z << "\n";

		setCollisionInit();
	}

	const glm::mat4& getModel() {
		return model;
	}

	const glm::mat4& getInverseTranspose() {
		return inverse_transpose;
	}

	btCollisionObject* getCollisionObject() {
		return object;
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