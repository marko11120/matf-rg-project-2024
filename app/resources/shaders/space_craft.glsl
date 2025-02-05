//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 projection, view, model;

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;

    vec3 direction;
    float cut_off;
    float outerCut_off;
};

struct Material{
    sampler2D texture_diffuse;
    sampler2D texture_specular;

    float linearC;
    float quadraticC;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 cameraPos;

void main(){

if(FragPos.x < 0.1f && FragPos.y < 0.1 && FragPos.z < 0.1f)
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);

    float distance = length(FragPos - light.position);
    float attenuation = 1.0/(1.0 + material.linearC * distance + material.quadraticC * pow(distance, 2));

    vec3 ambient = 0.3f * light.ambient * texture(material.texture_diffuse, TexCoords).rgb;

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = light.diffuse * texture(material.texture_diffuse, TexCoords).rgb * diff;

    float specularStrength = 0.8;
    vec3 v = normalize(reflect(-lightDir, normalize(Normal)));
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, v), 0.0), material.shininess);
    vec3 specular = light.specular * texture(material.texture_specular, TexCoords).rgb * spec * specularStrength;

    vec3 result = (ambient + diffuse + specular) * attenuation;
    //FragColor = vec4(result, 1.f);
}
