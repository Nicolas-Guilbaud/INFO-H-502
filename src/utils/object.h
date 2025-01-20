#ifndef OBJECT_H
#define OBJECT_H
#define VTXMASS 1.0f
#include<iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>


#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


struct Vertex{
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 normal;
};

class Object
{	
private:
	std::vector<Vertex> vertices;

	glm::vec3 ctrOfMass = glm::vec3(0.0);

	glm::mat4 model = glm::mat4(1.0);

	glm::mat4 inverseTranspose = model;

	btConvexHullShape* hull = nullptr; // Convex hull for collision shape

	btCollisionObject* object = new btCollisionObject(); // Collision object

	void setCollisionInit() {
		if (hull) delete hull; // Clean up any existing hull

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
		identityTransform.setOrigin(btVector3(ctrOfMass.x,ctrOfMass.y,ctrOfMass.z));
		object->setWorldTransform(identityTransform);
	}

public:

	int numVertices;

	GLuint VBO, VAO;

	Object(const char* path) {

		// Read the file defined by the path argument 
		// open the .obj file into a text editor and see how the data are organized
		// you will see line starting by v, vt, vn and f --> what are these ?
		// Then parse it to extract the data you need
		// keep track of the number of vertices you need

		std::string name;
		std::vector<glm::vec3> position, normal;
		std::vector<glm::vec2> texture;

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
					ctrOfMass = ctrOfMass + glm::vec3(x, y, z)*vmass;
					//std::cout << "Next processed vertex " << x << " " << y << " " << z << "\n" ;
					//std::cout << "Adjusted center of mass: " <<  ctrOfMass.x << " " << ctrOfMass.y << " " << ctrOfMass.z << "\n";
					//vertice
				}

				if(word == "vt"){
					//texture
					float x,y;
					streamedLine >> x >> y;
					texture.push_back(glm::vec2(x,y));
				}

				if(word == "vn"){
					//normal
					float x,y,z;
					streamedLine >> x >> y >> z;
					normal.push_back(glm::vec3(x,y,z));
				}

				if(word == "f"){
					//face
					// v/vt/vn
					while(!streamedLine.eof()){

						std::string p,t,n;

						streamedLine >> word;

						p = word.substr(0,word.find("/"));
						word.erase(0,word.find("/")+1);

						t = word.substr(0,word.find("/"));
						word.erase(0,word.find("/")+1);

						n = word.substr(0,word.find("/"));

						Vertex v;
						v.position = position.at(std::stof(p)-1);
						v.normal = normal.at(std::stof(n)-1);
						v.texture = texture.at(std::stof(t)-1);

						vertices.push_back(v);
					}
				}
			}
		}catch(std::ifstream::failure e){
			std::cout << "ERROR READING '" << path << "' OBJ FILE:\n" << e.what() << std::endl;
		}

		file.close();
		numVertices = vertices.size();
		ctrOfMass = ctrOfMass / (numVertices*VTXMASS);
		std::cout << "Object with center of mass " << ctrOfMass.x << " " << ctrOfMass.y << " " << ctrOfMass.z << " " << "set.\n";
		
		setCollisionInit();
	}

	void setModel(const glm::mat4& newModel) {
		model = newModel;
		inverseTranspose = glm::inverse(glm::transpose(model));
		ctrOfMass = glm::vec3(model * glm::vec4(ctrOfMass, 1.0));
		std::cout << "Updated center of mass " << ctrOfMass.x << " " << ctrOfMass.y << " " << ctrOfMass.z << "\n";

		setCollisionInit();
	}

	const glm::mat4& getModel() {
		return model;
	}

	const glm::mat4& getInverseTranspose() {
		return inverseTranspose;
	}

	const btCollisionObject* getCollisionObject() {
		return object;
	}

	void makeObject(Shader shader, bool texture = true) {

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
		if(texture){
			auto att_texture = glGetAttribLocation(shader.ID, "tex_coord");
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
	}

	void draw() {

		//bind your vertex arrays and call glDrawArrays
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}
};
#endif