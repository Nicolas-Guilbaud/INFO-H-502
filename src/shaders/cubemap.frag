#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec3 tex_coord;

uniform samplerCube cubemap;

void main() {
    FragColor = texture(cubemap,tex_coord);
}