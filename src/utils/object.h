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

#include "camera.h"
#include "./Mesh.h"
#include "./Light.h"

#ifndef OBJECT_H
#define OBJECT_H
#define MAX3(a, b, c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#define ORIENTATION glm::vec3(0.0,0.0,1.0) // pins along z-axis

/**
 * Represents an object that can be rendered by openGL
 */
class Object {	
protected:
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 inverse_transpose = model;
	glm::mat4 permutation = glm::mat4(1.0); // needed to align the pins along z axis
	Mesh mesh;
private:
public:

	Object(Mesh mesh) : mesh(mesh){
		alignAlongZ(mesh.getInitialDims());
	}

	void draw(Camera camera, Light l){
		Shader shader = mesh.getShader();
		shader.use();

		shader.setMatrix4("V", camera.GetViewMatrix());
		shader.setMatrix4("P", camera.GetProjectionMatrix());
		shader.setMatrix4("M", model);
		shader.setMatrix4("itM", inverse_transpose);
		shader.setVector3f("u_view_pos", camera.Position);
		shader.setVector3f("u_light_pos", l.position);

		if(mesh.hasTexture()){
			shader.setTexture("img",0);
		}
		mesh.draw();
	}

	void alignAlongZ(glm::vec3 dims) {
		if (MAX3(dims.x, dims.y, dims.z) > dims.z) {
			if (dims.x > dims.z && dims.x > dims.y) {
				#ifdef NDEBUG
				std::cout << "maximal dimensions along x axis" << std::endl;
				glm::vec4 temp = permutation[0];
				permutation[0] = permutation[2];
				permutation[2] = temp;
				#endif
			}
			else if (dims.y > dims.z && dims.y > dims.x) {
				#ifdef NDEBUG
				std::cout << "maximal dimensions along y axis" << std::endl;
				glm::vec4 temp = permutation[1];
				permutation[1] = permutation[2];
				permutation[2] = temp;
				#endif
			}
		}
	}
	
	void setModel(const glm::mat4& newModel) {
		model = newModel;
		inverse_transpose = glm::inverse(glm::transpose(model));
	}

	const glm::mat4& getModel() {
		return model;
	}

	const glm::mat4& getInverseTranspose() {
		return inverse_transpose;
	}
};
#endif
