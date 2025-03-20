#include "./object.h"
#include "../utils/camera.h"

class MirrorFace : public Object{

private:
    GLuint fbo, texColorBuffer, rbo;
    glm::vec3 position = glm::vec3(0); //center of the mirror
    glm::vec3 oldCamPos = glm::vec3(0);

    static Mesh initMesh(){
        Shader shader(PATH_TO_SHADERS "/mirror.vert",PATH_TO_SHADERS "/mirror.frag");
        return Mesh(PATH_TO_MESHES "/mirror.obj",shader,true);
    }

public:

    const glm::vec3 getPosition() const{
        return position;
    }

    int width,height;

    MirrorFace(int width,int height) : Object(initMesh(),glm::vec3(0.0)), width(width),height(height){
        
        //create fbo
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);

        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        //glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texColorBuffer,0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);  
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,texColorBuffer,0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Error initializing mirror object: could not create FBO properly !");
        
        GLenum drawingBuff[1] = {GL_COLOR_ATTACHMENT0};

        glDrawBuffers(1,drawingBuff);

        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    void placeCamera(Camera* c){
        glm::vec3 distance = this->position - c->Position;

        c->Position += distance;
        c->BehindRotation();
    }

    void beginMirrorRendering(){
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);

        glViewport(0,0,width,height);

        //clean up fbo
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void endMirrorRendering(){
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        mesh.changeTexture(texColorBuffer);
    }

    template<class T, bool = std::is_base_of<Object, T>::value>
    void renderMirrorTexture(Camera c, Light l, std::vector<T> objList){
        //TODO: modify mirror internal camera with external one to have illusion to see the reflection
        
        //calculate distance between camera & center of the mirror
        glm::vec3 oldCamPos = c.Position;
        glm::vec3 distance = this->position - c.Position;

        //set camera in a symetrical point to the center of the mirror
        c.Position += distance;
        c.BehindRotation();
        //swap to fbo
        glBindFramebuffer(GL_FRAMEBUFFER,fbo);

        glViewport(0,0,width,height);

        //clean up fbo
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //render the scene from the mirror PoV
        for(Object o : objList){
            o.draw(c,l);
        }

        //store the texture for the mirror rendering itself
        mesh.changeTexture(texColorBuffer);

        //replace cmera back to its old position
        c.Position = oldCamPos;

        //go back to main rendering
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    void draw(Camera c, Light l){
        //called after the draw in the FBO

        //render on 1 side of the mirror only
        glEnable(GL_CULL_FACE);

        //set texture in FBO as active texture
        Shader s = mesh.getShader();
        s.use();
        s.setTexture("fbo_img",0);
        s.setMatrix4("V", c.GetViewMatrix());
		s.setMatrix4("P", c.GetProjectionMatrix());
		s.setMatrix4("M", model);
        mesh.draw();

        //disable culling for cubemap
        glDisable(GL_CULL_FACE);
        
    }

    void translateModel(glm::vec3 translation){
        position += translation;
        Object::translateModel(translation);
    }

    void translateModel(float x, float y, float z){
        MirrorFace::translateModel(glm::vec3(x,y,z));
    }

};