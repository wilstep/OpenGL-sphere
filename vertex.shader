#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 rotate; 
uniform mat4 perspective;
float dz = -5.0;
vec4 position;

void main()
{    
    position = rotate * vec4(aPos, 1.0);
    position.z += dz; // move the whole sphere
    gl_Position = perspective * position; // predefined vertex output position
    Normal = vec3(rotate * vec4(aNormal, 0.0)); 
    FragPos = vec3(position); // real position for lighting calculations
}


