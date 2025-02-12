//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model, view, projection;

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

//#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 intensity;
};

struct SpotLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

uniform SpotLight spotLight;
uniform Light light;
uniform Material material;
uniform vec3 cameraPos;

uniform sampler2D texture_diffuse1;

void main(){

   //POINT
    float distance = length(FragPos - light.position);
    float attenuation = 1.0/(1.0 + material.linearC * distance + material.quadraticC * pow(distance, 2));

    vec3 ambient = 0.4f * light.intensity * light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
    ambient *= attenuation;

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = light.intensity * light.diffuse * texture(material.texture_diffuse, TexCoords).rgb * diff;
    diffuse *= attenuation;

    float specularStrength = 0.8;
    vec3 v = normalize(reflect(-lightDir, normalize(Normal)));
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, v), 0.0), material.shininess);
    vec3 specular = light.intensity * light.specular * texture(material.texture_specular, TexCoords).rgb * spec * specularStrength;
    specular *= attenuation;

    //SPOT
    lightDir = normalize(FragPos - cameraPos);
    float theta = dot(lightDir, normalize(spotLight.direction));

    //ambient
    // already set
    //diffuse
    diff = max(dot(-lightDir, normalize(Normal)), 0.0);
    vec3 diffuse_spotLight = spotLight.diffuse * texture(material.texture_diffuse, TexCoords).rgb * diff;

    //specular
    viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = normalize(reflect(lightDir, Normal));
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular_spotLight = spotLight.specular * texture(material.texture_specular, TexCoords).rgb * spec * specularStrength;

    float proximity = (theta - spotLight.outerCut_off) / (spotLight.cut_off - spotLight.outerCut_off);
    float intensity = clamp(proximity, 0.0, 1.0);
    diffuse_spotLight *= intensity;
    specular_spotLight *= intensity;

    //attenuation
    float d = length(cameraPos - FragPos);
    float att = 1.0 / (1.0 + d * material.linearC + pow(d, 2) * material.quadraticC);
    diffuse_spotLight *= att;
    specular_spotLight *= att;

    diffuse += diffuse_spotLight;
    specular += specular_spotLight;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.f);
}




