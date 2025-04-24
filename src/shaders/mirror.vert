#version 330 core
in vec3 position; 
in vec2 tex_coords; 
in vec3 normal;

out vec2 tex_coord;

uniform mat4 M;
uniform mat4 V; 
uniform mat4 P; 

void main(){ 
    vec4 frag_coord = M*vec4(position, 1.0); 
    gl_Position = P*V*frag_coord;
    tex_coord = tex_coords;
}