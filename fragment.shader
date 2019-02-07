#version 330 core
uniform vec3 triangleColor;

in vec3 Normal;
in vec3 FragPos;
out vec4 outColor;

void main()
{
    vec3 lightPos = vec3(45.0, 45.0, 80.0);
    vec3 lightColor = vec3(0.9, 0.9, 0.9);
    float ambientStrength = 0.2;
    
    vec3 viewPos = vec3(0.0, 0.0, 50.0);
    float specularStrength = 0.5;

    
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - FragPos);
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = Normal;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor; 
    vec3 result = (ambient + diffuse + specular) * triangleColor;
    outColor = vec4(result, 1.0);
    //outColor = vec4(triangleColor, 1.0);
}


