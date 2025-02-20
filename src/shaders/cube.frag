#version 330 core
out vec4 FragColor;
precision mediump float; 

//What do you need to receive from the vertex shader ?
in vec3 v_normal;
in vec3 v_frag_coords;
in vec2 tex_coord;

uniform vec3 u_light_pos; 
uniform vec3 u_view_pos; 
uniform sampler2D img;

float shininess = 32.0;
float spec_strength = 0.8;
//Which part do you need to compute here ?
void main() { 
    vec3 L = normalize(u_light_pos - v_frag_coords);
    vec3 V = normalize(u_view_pos - v_frag_coords);
    vec3 N = normalize(v_normal);
    vec3 R_1 = max(2*dot(N,L)*N-L,0);
    vec3 color = vec3(dot(R_1,V));
    vec3 R = reflect(-L,N);
    float cosTheta = dot(R , V); 
    float spec = pow(max(cosTheta,0.0), shininess); 
    float specular = spec_strength * spec;
    // FragColor = texture(img,tex_coord);
    FragColor = vec4(color+vec3(specular), 1.0)*texture(img,tex_coord);
}