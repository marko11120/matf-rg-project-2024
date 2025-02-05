//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
}

//#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

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
    //POINT
    if(FragPos.x < 0.1f && FragPos.y < 0.1 && FragPos.z < 0.1f)
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    float distance = length(FragPos - light.position);
    float attenuation = 1.0/(1.0 + material.linearC * distance + material.quadraticC * pow(distance, 2));

    vec3 ambient = 0.4f * light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
    ambient *= attenuation;

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = light.diffuse * texture(material.texture_diffuse, TexCoords).rgb * diff;
    diffuse *= attenuation;

    float specularStrength = 0.8;
    vec3 v = normalize(reflect(-lightDir, normalize(Normal)));
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, v), 0.0), material.shininess);
    vec3 specular = light.specular * texture(material.texture_specular, TexCoords).rgb * spec * specularStrength;
    specular *= attenuation;

    //SPOT
    lightDir = normalize(FragPos - cameraPos);
    float theta = dot(lightDir, normalize(light.direction));

    //ambient
    ambient += 0.5 * light.ambient * texture(material.texture_diffuse, TexCoords).rgb;
    //diffuse
    diff = max(dot(-lightDir, normalize(Normal)), 0.0);
    diffuse += light.diffuse * texture(material.texture_diffuse, TexCoords).rgb * diff;

    //specular
    viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = normalize(reflect(lightDir, Normal));
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular += light.specular * texture(material.texture_specular, TexCoords).rgb * spec * specularStrength;

    float proximity = (theta - light.outerCut_off) / (light.cut_off - light.outerCut_off);
    float intensity = clamp(proximity, 0.0, 1.0);


    //ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    //attenuation
    //float d = length(cameraPos - FragPos);
    //float att = 1.0 / (1.0 + d * material.linearC + pow(d, 2) * material.quadraticC);

    //ambient *= att;
    //diffuse *= att;
    //specular *= att;

    vec3 result = ambient + diffuse + specular;

    //FragColor = vec4(result, 1.f);
}