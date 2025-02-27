#include "./object.h"
#include "../meshes/CubeMap.cpp"

class Cubemap{

private:
    CubeMapMesh mesh;

    static Shader createShader(){
        return Shader(PATH_TO_SHADERS "/cubemap.vert", PATH_TO_SHADERS "/cubemap.frag");
    }

public:
    Cubemap(GLuint texture) : mesh(createShader(),texture){
    }

    void draw(Camera c, Light l){
        //light on the cubemap ?
        Shader shader = mesh.getShader();

		shader.use();
		shader.setMatrix4("V", c.GetViewMatrix());
		shader.setMatrix4("P", c.GetProjectionMatrix());
        mesh.draw();
    }



};