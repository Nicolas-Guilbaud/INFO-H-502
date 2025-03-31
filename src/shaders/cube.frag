#version 330 core
out vec4 FragColor;
precision mediump float; 

in vec3 v_normal;
in vec3 v_frag_coords;
in vec2 tex_coord;

uniform vec3 u_light_pos; 
uniform vec3 u_view_pos; 
uniform vec3 light_spectrum; 
uniform sampler2D img;
uniform vec3 F0;
uniform float roughness;

float pi = 3.1415;
float shininess = 32.0;
float spec_strength = 0.8;


float lerp(float x, float y, float s){
    return x*(1-s) + y*s;
}

float chi(float x){
    return x > 0 ? 1.0f : 0.0f;
}

float squared(float x){
    return x*x;
}

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
    vec3 F = F0 + (vec3(1)-F0)*pow((1-max(dot(N,L),0)),5); // Schlick's approximation
    vec3 H = normalize(L+V);
    float nh_prod = dot(N,H);
    float nl_prod = abs(dot(N,L));
    float nv_prod = abs(dot(N,V));
    //M = H = half vector 
    float GGX_distrib = chi(nh_prod)*(squared(roughness))/(pi*squared(1+squared(nh_prod)*(squared(roughness)-1)));
    float G_2_approx = 0.5/lerp(2*nl_prod*nv_prod,nl_prod+nv_prod,roughness);
    vec3 final_spec = F*G_2_approx*GGX_distrib;
    FragColor = vec4(color+vec3(specular), 1.0)*vec4(light_spectrum, 1.0)*vec4(final_spec, 1.0)*texture(img,tex_coord);
}