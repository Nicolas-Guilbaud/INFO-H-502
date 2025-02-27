#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <map>

/**
 * Used to load textures
 * Throws a runtime error if the path is incorrect.
 */
static GLuint loadTexture(std::string path){
    GLuint buffer;
    int width, height, nbChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(),&width,&height,&nbChannels,0);

    if(!data){
        throw std::runtime_error("Could not load texture at " + path);
    }

    glGenTextures(1,&buffer);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,buffer);

    //TODO: modifier les options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return buffer;
}

/**
 * Loads a cubemap from a texture subfolder, and a mapping over the faces to load.
 */
static GLuint loadCubemap(std::string folderPath, std::map<std::string, GLenum> facesToLoad){
    GLuint buffer;

    glGenTextures(1,&buffer);
    glBindTexture(GL_TEXTURE_CUBE_MAP,buffer);

    int width, height, nbChanels;

    for(std::pair<std::string,GLenum> pair : facesToLoad){
        std::string file = folderPath + "/" + pair.first;
        unsigned char* data = stbi_load(file.c_str(),&width,&height,&nbChanels,0);

        if(!data){
            throw std::runtime_error("Could not load texture at " + file);
        }

        glTexImage2D(
            pair.second,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,GL_UNSIGNED_BYTE,
            data
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return buffer;

}

/**
 * Loads a cubemap from a texture subfolder.
 * It assumes the following file convention:
 * - negx.jpg
 * - negy.jpg
 * - negz.jpg
 * - posx.jpg
 * - posy.jpg
 * - posz.jpg
 */
static GLuint loadCubemap(std::string folderPath){
    std::map<std::string,GLenum> faces = {
        {"negx.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_X},
        {"negy.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Y},
        {"negz.jpg",GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},

        {"posx.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_X},
        {"posy.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Y},
        {"posz.jpg",GL_TEXTURE_CUBE_MAP_POSITIVE_Z},
    };

    return loadCubemap(folderPath,faces);
}