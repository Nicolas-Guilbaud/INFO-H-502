#include "./Mesh.h"

class refMesh : public Mesh {
private:

public:
    refMesh(std::string path, Shader shader, GLuint texture) : Mesh(path, shader, texture) {}

    void draw() override {
        glDepthMask(GL_FALSE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glBindVertexArray(this->VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

        glDepthMask(GL_TRUE);
    }
};