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

#include "../utils/shader.h"
#include "../meshes/Mesh.h"
#include "../utils/Light.h"
#include "../utils/camera.h"

#ifndef OBJECT_H
#define OBJECT_H

/**
 * Represents an object that can be rendered by openGL
 */
class Object {	
protected:
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 inverse_transpose = glm::mat4(1.0);
	Mesh mesh;
	glm::vec3 F0;
	float roughness;
private:
public:

	Object(Mesh mesh, glm::vec3 F0=glm::vec3(1.0),float roughness = 1.0f) :
	F0(F0), roughness(roughness), mesh(mesh){
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
		shader.setVector3f("F0", F0);
		shader.setVector3f("light_spectrum", l.spectrum);
		shader.setFloat("roughness",roughness);

		if(mesh.hasTexture()){
			shader.setTexture("img",0);
		}

		mesh.draw();
	}
	
	void setModel(const glm::mat4& newModel) {
		model = newModel;
		inverse_transpose = glm::inverse(glm::transpose(model));
	}
	
	void translateModel(glm::vec3 translation){
		model = glm::translate(model, translation);
	}

	void translateModel(float x, float y, float z){
		this->translateModel(glm::vec3(x,y,z));
	}

	void rotateModel(float angle, glm::vec3 axle){
		model = glm::rotate(model,glm::radians(angle),axle);
	}

	void scaleModel(float x, float y, float z){
		this->scaleModel(glm::vec3(x,y,z));
	}

	void scaleModel(glm::vec3 scaling){
		model = glm::scale(model,scaling);
	}

	const glm::mat4& getModel() {
		return model;
	}

	const glm::mat4 getInverseTranspose() {
		return inverse_transpose;
	}
	const glm::vec3 getInitialDimensions() {
		return mesh.getInitialDims();
	}
	const glm::vec3 getInitialCenter() {
		return mesh.getInitialCtr();
	}
};
#endif
