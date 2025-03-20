#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec2 tex_coord;

uniform sampler2D fbo_img;

void main(){
    FragColor = texture(fbo_img,tex_coord);
}