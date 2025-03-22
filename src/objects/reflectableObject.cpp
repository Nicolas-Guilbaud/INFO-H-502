#include "object.h"

class reflectableObject : public Object {
public:
	reflectableObject(Mesh mesh, glm::vec3 F0 = glm::vec3(1.0)):Object(mesh, F0) { // induces slicing on rMesh hence the need for addtional attribute
	}

	void draw(Camera camera, Light l, GLuint cubeMapTexture){
		Shader shader = mesh.getShader();
		shader.use();

		shader.setMatrix4("V", camera.GetViewMatrix());
		shader.setMatrix4("P", camera.GetProjectionMatrix());
		shader.setMatrix4("M", model);
		shader.setMatrix4("itM", inverse_transpose);
		shader.setVector3f("u_view_pos", camera.Position);
		this->mesh.draw();
	}

};