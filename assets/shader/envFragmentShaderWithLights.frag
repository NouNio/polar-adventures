#version 330 core
// NOTE: frag shader with multiple lights: 1 dir light and n point lights (spotlight to come)
out vec4 FragColor;

struct Material {   
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;   
};

struct PointLight {
    vec3 pos;
    float Kc, Kl, Kq;   // constant, linear and quadratic terms for attenuation formula
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;      
};

#define N_PT_LIGHTS 5

in vec3 Normal;   // perpendicular to the vertex
in vec3 FragPos;  // vertex position in world view
  
 
uniform vec3 viewPos; 
uniform Material material;                        // material vals of the vertex
uniform DirectionalLight directionalLight;        // pos and material vals of directional light source
uniform PointLight pointLights[N_PT_LIGHTS];

vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);

void main()
{
    vec3 norm = normalize(Normal);                // ambiguously the vector "Normal" is not normalized yet (necesserily), only perpendicular
    vec3 viewDir = normalize(viewPos - FragPos);  // get the direction of view, pointing from fragment (fragPos) to the view (Camera)
    
    vec3 result = computeDirectionalLight(directionalLight, norm, viewDir, material);      // influence from the directional light
   
    for(int i = 0; i < N_PT_LIGHTS; i++)
        result += computePointLight(pointLights[i], norm, FragPos, viewDir, material);     // compute influence on the vertex from all the point lights
   
    FragColor = vec4(result, 1.0);
}


vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(-light.direction);
    
    vec3 ambient  = light.ambient * material.ambient;  // ambient shading
    
    float diff = max(dot(normal, lightDir), 0.0);  // diffuse shading
    vec3 diffuse = light.diffuse * diff * material.diffuse;  
    
    vec3 reflectDir = reflect(-lightDir, normal);  // specular shading
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    return (ambient + diffuse + specular);
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.pos - fragPos);   // get direction of light ray, pointing from the fragment (fragPos) to the light
    
    // ambient shading
     vec3 ambient = light.ambient * material.ambient;

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);  // combine results
}
