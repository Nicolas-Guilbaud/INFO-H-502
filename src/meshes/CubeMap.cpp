#include "./Mesh.h"

class CubeMapMesh : public Mesh{
private:

public:
    CubeMapMesh(Shader shader, GLuint texture): Mesh(PATH_TO_MESHES "/cube.obj",shader,texture){}

    void draw() override{
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);

        glDepthMask(GL_TRUE);
    }
};