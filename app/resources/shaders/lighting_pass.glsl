//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}


//#shader fragment
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform float border;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform vec3 cameraPos;
uniform int spotLightSwitch;

vec3 pointLightCalc(PointLight pointLight, vec3 FragPos, vec3 Normal, vec3 CameraPos, vec3 texture_diffuse0, float texture_specular0){

    float distance = length(FragPos - pointLight.position);
    float attenuation = 1.0/(1.0 + pointLight.linearC * distance + pointLight.quadraticC * pow(distance, 2));

    vec3 ambient = 0.4f * pointLight.intensity * pointLight.ambient * texture_diffuse0;
    ambient *= attenuation;

    vec3 pointLightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(normalize(Normal), pointLightDir), 0.0);
    vec3 diffuse = pointLight.intensity * pointLight.diffuse * texture_diffuse0 * diff;
    diffuse *= attenuation;

    float specularStrength = 0.8;
    vec3 v = normalize(reflect(-pointLightDir, normalize(Normal)));
    vec3 viewDir = normalize(cameraPos - FragPos);
    float spec = pow(max(dot(viewDir, v), 0.0), pointLight.shininess);
    vec3 specular = pointLight.intensity * pointLight.specular * texture_specular0 * spec * specularStrength;
    specular *= attenuation;

    return vec3(ambient+diffuse+specular);
}

vec3 spotLightCalc(SpotLight spotLight, vec3 FragPos, vec3 Normal, vec3 CameraPos, vec3 texture_diffuse0, float texture_specular0){

    vec3 pointLightDir = normalize(FragPos - cameraPos);
    float theta = dot(pointLightDir, normalize(spotLight.direction));

    //ambient
    // already set
    //diffuse
    float diff = max(dot(-pointLightDir, normalize(Normal)), 0.0);
    vec3 diffuseSpotLight = spotLight.diffuse * texture_diffuse0 * diff;

    //specular
    float specularStrength = 0.8;
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = normalize(reflect(pointLightDir, Normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), spotLight.shininess);
    vec3 specularSpotLight = spotLight.specular * texture_specular0 * spec * specularStrength;

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

    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 texture_diffuse0 = texture(gAlbedoSpec, TexCoords).rgb;
    float texture_specular0 = texture(gAlbedoSpec, TexCoords).a;

    vec3 spot = spotLightCalc(spotLight, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    vec3 point = pointLightCalc(pointLight, FragPos, Normal, cameraPos, texture_diffuse0, texture_specular0);
    vec3 result = spot*spotLightSwitch + point;

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness >= border)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
}