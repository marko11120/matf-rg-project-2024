//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

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
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct PointLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 intensity;
    float linearC;
    float quadraticC;
    float shininess;
};

struct SpotLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float linearC;
    float quadraticC;
    float shininess;
};

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;

uniform float border;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform vec3 cameraPos;

vec3 pointLightCalc(PointLight pointLight, vec3 FragPos, vec3 Normal, vec3 CameraPos, sampler2D texture_diffuse0, sampler2D texture_specular0){

    float distance = length(FragPos - pointLight.position);
    float attenuation = 1.0/(1.0 + pointLight.linearC * distance + pointLight.quadraticC * pow(distance, 2));

    vec3 ambient = 0.4f * pointLight.intensity * pointLight.ambient * texture(texture_diffuse0, TexCoords).rgb;
    ambient *= attenuation;

    vec3 pointLightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(normalize(Normal), pointLightDir), 0.0);
    vec3 diffuse = pointLight.intensity * pointLight.diffuse * texture(texture_diffuse0, TexCoords).rgb * diff;
    diffuse *= attenuation;

    float specularStrength = 0.8;
    vec3 v = normalize(reflect(-pointLightDir, normalize(Normal)));
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, v), 0.0), pointLight.shininess);
    vec3 specular = pointLight.intensity * pointLight.specular * texture(texture_specular0, TexCoords).rgb * spec * specularStrength;
    specular *= attenuation;

    return vec3(ambient+diffuse+specular);
}

vec3 spotLightCalc(SpotLight spotLight, vec3 FragPos, vec3 Normal, vec3 CameraPos, sampler2D texture_diffuse0, sampler2D texture_specular0){

    vec3 pointLightDir = normalize(FragPos - cameraPos);
    float theta = dot(pointLightDir, normalize(spotLight.direction));

    //ambient
    // already set
    //diffuse
    float diff = max(dot(-pointLightDir, normalize(Normal)), 0.0);
    vec3 diffuseSpotLight = spotLight.diffuse * texture(texture_diffuse0, TexCoords).rgb * diff;

    //specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = normalize(reflect(pointLightDir, Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), spotLight.shininess);
    vec3 specularSpotLight = spotLight.specular * texture(texture_specular0, TexCoords).rgb * spec * specularStrength;

    float proximity = (theta - spotLight.outerCutOff) / (spotLight.cutOff - spotLight.outerCutOff);
    float intensity = clamp(proximity, 0.0, 1.0);
    diffuseSpotLight *= intensity;
    specularSpotLight *= intensity;

    //attenuation
    float d = length(cameraPos - FragPos);
    float att = 1.0 / (1.0 + d * spotLight.linearC + pow(d, 2) * spotLight.quadraticC);
    diffuseSpotLight *= att;
    specularSpotLight *= att;

    return vec3(diffuseSpotLight + specularSpotLight);
}

void main(){

    vec3 spot = spotLightCalc(spotLight, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    vec3 point = pointLightCalc(pointLight, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    vec3 result = spot + point;

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness >= border)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}