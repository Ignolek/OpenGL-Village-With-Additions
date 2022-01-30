#version 400 core

// fragment/pixel color
out vec4 FragColor;

// data from vertex shader
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// Struct which describes object appearience
struct Material 
{
    sampler2D diffuse;
    vec3 specular;    
    float shininess;
}; 

// Struct which describes lights output data
struct LightMain 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Point light struct
struct LightPoint 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiny;

    float constant;
    float linear;
    float quadratic;

    vec3 pointcolor;
};

// Dir light struct
struct DirLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiny;
    vec3 dircolor;
};

// Spot light struct
struct SpotLight 
{
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
    float shiny;

    vec3 spot1color;
    vec3 spot2color;
};

#define NR_SPOTLIGHT 2 // Number of spotLights
uniform vec3 viewPos; // Camera position
uniform Material material; // Material data
uniform DirLight dirlight; // DirLight data
uniform LightPoint lightPoint; // PointLight data
uniform SpotLight spotLight[NR_SPOTLIGHT]; // SpotLight data
uniform sampler2D texture_diffuse1; // color texture
uniform sampler2D texture_specular1; // specular texture

uniform vec3 PointColor;
uniform vec3 SpotLightColor[NR_SPOTLIGHT];

uniform bool dir;
uniform bool point;
uniform bool ref;

LightMain CalculatePointLight(vec3 FragPos, vec3 normal, LightPoint light, vec3 viewDir) {
    LightMain model;

    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shiny);
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(texture_specular1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
    
    model.ambient = attenuation *ambient; 
    model.diffuse = attenuation *diffuse ;
    model.specular = attenuation*specular;
    return model;
}

LightMain CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    LightMain model;

    vec3 lightDir = normalize(light.position - fragPos);

    // ambient
    vec3 ambient =  light.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb; 

    // specular
    viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shiny);
    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;  
        
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
    diffuse   *= attenuation;
    specular *= attenuation;   
            
    vec3 result = ambient + diffuse + specular;
    model.ambient = ambient;
    model.diffuse = diffuse;
    model.specular = specular;

    return model;
}

LightMain CalculateDirectionalLight(vec3 FragPos, vec3 normal, DirLight light, vec3 viewDir) {
    LightMain model;
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),light.shiny);
    // combine results
    model.ambient =  light.ambient * texture(texture_diffuse1, TexCoords).rgb;
    model.diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
    model.specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
   return model;
} 

void main()
{      
    vec3 result = vec3(0.0f);
    LightMain last;

    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 specul =  texture(texture_specular1, TexCoords).rgb;

    vec3 lightDir = normalize(lightPoint.position - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0); // zmiana normal -> lightdir
   
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    //result +=vec3( dirlight.ambient);

    if(dir){
        last = CalculateDirectionalLight(FragPos,normal,dirlight,viewDir);
        result += dirlight.dircolor * vec3(last.ambient + last.diffuse + last.specular);
    }

    if(point){
        last = CalculatePointLight(FragPos,normal,lightPoint,viewDir);
        result += lightPoint.pointcolor * vec3(last.ambient + last.diffuse + last.specular);
    }

    if(ref){
        last = CalculateSpotLight(spotLight[0], normal, FragPos, viewDir);
        result += spotLight[0].spot1color * vec3(last.ambient + last.diffuse + last.specular);
        last = CalculateSpotLight(spotLight[1], normal, FragPos, viewDir);
        result += spotLight[1].spot2color * vec3(last.ambient + last.diffuse + last.specular);
    }

    FragColor = vec4(result, 1.0f);
}







