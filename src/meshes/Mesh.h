#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <vector>
#include "../utils/shader.h"

#define VTXMASS 1.0f
#define NDEBUG
#define CHARACTERISTIC_LEN 1.0f 
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#define ORIENTATION glm::vec3(0.0,0.0,1.0) // pins along z-axis

struct Vertex{
	glm::vec3 position;
	glm::vec2 texture;
	glm::vec3 normal;
};

static void updateExtremes(float x, float y, float z, float* xmin, float* xmax, float* ymin, float* ymax, float* zmin, float* zmax) {
    if (*xmin > x) *xmin = x;
    if (*xmax < x) *xmax = x;
    if (*ymin > y) *ymin = y;
    if (*ymax < y) *ymax = y;
    if (*zmin > z) *zmin = z;
    if (*zmax < z) *zmax = z;
}

class Mesh{
protected:
    GLuint VAO,VBO, texture;
    bool useTexture = false;
    int numVertices;
    std::string name;
    glm::vec3 initial_dimensions = glm::vec3(0.0);
	glm::vec3 initial_ctr = glm::vec3(0.0);
	float scale;
    Shader shader;
    std::vector<Vertex> vertices;

    /**
     * Reads obj file
     */
    void loadFromFile(std::string path){
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

			initial_dimensions = glm::vec3(fabsf(xmax - xmin), fabsf(ymax - ymin), fabsf(zmax - zmin));
			initial_ctr = glm::vec3((xmax + xmin) / (2), (ymax + ymin) / (2), (zmax + zmin) / (2));
			scale = CHARACTERISTIC_LEN / MAX3(initial_dimensions.x, initial_dimensions.y, initial_dimensions.z);
            #ifdef NDEBUG
			std::cout << "The initial dimensions are x: " << initial_dimensions.x << " y: " << initial_dimensions.y << " z: " << initial_dimensions.z<< std::endl;
			std::cout << "The normalized dimensions are x: " << initial_dimensions.x * scale << " y: " << initial_dimensions.y * scale << " z: " << initial_dimensions.z * scale << std::endl;
			std::cout << "The normalized center is x: " << initial_ctr.x * scale << " y: " << initial_ctr.y * scale << " z: " << initial_ctr.z * scale << std::endl;
			#endif
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

    }

    void makeMesh(Shader shader){
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

		//position
		auto att_pos = glGetAttribLocation(shader.ID, "position");
		glEnableVertexAttribArray(att_pos);
		glVertexAttribPointer(att_pos, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);

        //texture
		if(this->useTexture){
			auto att_texture = glGetAttribLocation(shader.ID, "tex_coords");
			glEnableVertexAttribArray(att_texture);
			glVertexAttribPointer(att_texture, 2, GL_FLOAT, false, 8 * sizeof(float), (void*) (sizeof(float)*3));
		}
		//normal
		auto att_normal = glGetAttribLocation(shader.ID, "normal");
		glEnableVertexAttribArray(att_normal);
		glVertexAttribPointer(att_normal, 3, GL_FLOAT, false, 8 * sizeof(float), (void*) (sizeof(float)*5));
		
        glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		delete[] data;
    }

public:

    Mesh(std::string path, Shader shader, bool useTexture = false) : shader(shader), useTexture(useTexture){
        loadFromFile(path);
        this->makeMesh(shader);
    }

    Mesh(std::string path, Shader shader, GLuint texture): Mesh(path,shader,true){
        this->texture = texture;
    }

    void changeTexture(GLuint texture){
        if(!this->useTexture) 
            throw std::runtime_error("Texture is not allowed on this mesh !");
        
        this->texture = texture;
	}

    virtual void draw() {

		if(useTexture){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
		}

		//bind your vertex arrays and call glDrawArrays
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

    const bool hasTexture() const{
		return this->useTexture;
	}

	const GLuint getTexture() const{
		return this->texture;
	}

    const std::string getName() const{
        return this->name;
    }

    const Shader getShader() const{
        return this->shader;
    }

    const std::vector<Vertex> getVertices() const{
        return this->vertices;
    }

    const glm::vec3 getInitialDims() const{
        return this->initial_dimensions;
    }

	const glm::vec3 getInitialCtr() const {
		return this->initial_ctr;
	}

	const glm::vec3 getNormalizedDims() const {
		return this->initial_dimensions*scale;
	}

	const glm::vec3 getNormalizedCtr() const {
		return this->initial_ctr*scale;
	}


};