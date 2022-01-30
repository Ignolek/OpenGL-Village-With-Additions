#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#define NR_SPOTLIGHT 2

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform DirLight dirLight;
uniform SpotLight spotLight[NR_SPOTLIGHT];


uniform bool dir;
uniform bool point;
uniform bool ref;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 result= vec3(0,0,0);

    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
  	 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    if(dir)
    result += CalcDirLight(dirLight, norm, viewDir);

    if(point){
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;  
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  
    result += ambient + diffuse + specular;
    }

    if(ref)
    {
     result += CalcSpotLight(spotLight[0], norm, FragPos, viewDir);   
      result += CalcSpotLight(spotLight[1], norm, FragPos, viewDir);   
    }
   

    
    FragColor = vec4(result, 1.0);
}
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient =  light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient =  light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}


