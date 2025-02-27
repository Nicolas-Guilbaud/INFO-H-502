#version 330 core
in vec3 position; 
in vec2 tex_coords; 
in vec3 normal; 

out vec3 tex_coord;

uniform mat4 V; 
uniform mat4 P;

void main(){
    tex_coord = position;
    mat4 v_rot = mat4(mat3(V));
    vec4 pos = P*v_rot*vec4(position,1.0);
    gl_Position = pos.xyww;

}