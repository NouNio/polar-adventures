#version 330 core
// NOTE: frag shader with multiple lights: 1 dir light and n point lights (spotlight to come)
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 depth;

struct Material {   
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;
    vec3 color;
 
};

struct PointLight {
    vec3 pos;
    float Kc, Kl, Kq;   // constant, linear and quadratic terms for attenuation formula
    vec3 color;      
};

#define N_PT_LIGHTS 3

in vec3 Normal;   // perpendicular to the vertex
in vec3 FragPos;  // vertex position in world view
  
 
uniform vec3 viewPos; 

uniform Material material;                        // material vals of the vertex
uniform DirectionalLight directionalLight;        // pos and material vals of directional light source
uniform PointLight pointLights[N_PT_LIGHTS];
vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
float discretize(float f);
uniform samplerCube skybox;

void main()
{
float maxDist= 10;
    vec3 norm = normalize(Normal);                // ambiguously the vector "Normal" is not normalized yet (necesserily), only perpendicular
    normal=vec4(norm,1.0);
  //THIS CAUSES BUGS
    //gl_FragDepth=FragPos.z;

    vec3 viewDir = normalize(viewPos - FragPos);  // get the direction of view, pointing from fragment (fragPos) to the view (Camera)
        depth = vec4(vec3(length(FragPos)/maxDist),1);
    vec3 result = computeDirectionalLight(directionalLight, norm, viewDir, material);      // influence from the directional light
    //vec3 result = vec3(normalize(norm));
    //for(int i = 0; i < N_PT_LIGHTS; i++)
    //    result += computePointLight(pointLights[i], norm, FragPos, viewDir, material);     // compute influence on the vertex from all the point lights
   
    FragColor = vec4(result, 1.0);
      //FragColor = vec4(1.0);
}

//TODO: change this with 1D texture though not necessarily necessary
float discretize(float f){
    if(f>=0.0 && f<0.1){
        f=0.1;
    }
    else if(f>=0.1 && f<0.2){
        f=0.2;
    }
    else if(f>=0.2 && f<0.3){
        f=0.3;
    }
    else if(f>=0.3 && f<0.4){
        f=0.4;
    }
    if(f>=0.4 && f<0.5){
        f=0.5;
    }
    else if(f>=0.5 && f<0.6){
        f=0.6;
    }
    else if(f>=0.6 && f<0.7){
        f=0.7;
    }
    else if(f>=0.7 && f<0.8){
        f=0.8;
    }
    else if(f>=0.9 && f<1.0){
        f=1.0;
    }

return f;
}

 
vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(-light.direction);
    
    vec3 ambient  = material.ambient;  // ambient shading
    
    float diff = max(dot(normal, lightDir), 0.0);  // diffuse shading
    //diff=discretize(diff);
    vec3 diffuse = diff * material.diffuse;  
    vec3 toCel = (diffuse+ambient)*light.color;
    toCel.r=discretize(toCel.r);
     toCel.g=discretize(toCel.g);
      toCel.b=discretize(toCel.b);
          vec3 saved= toCel;
    vec3 reflectDir = reflect(-lightDir, normal);  // specular shading

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;
    vec3 reflection  = texture(skybox, reflect(FragPos-viewPos, normal)).rgb;
    //if(spec>0.8){specular+=reflection*0.5;}
    //specular+=reflection;
;
    //saved.r=discretize(saved.r);

    return (saved + specular);
   
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.pos - fragPos);   // get direction of light ray, pointing from the fragment (fragPos) to the light
     float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));    
    light.color=light.color*attenuation;
    
    vec3 ambient  = material.ambient;  // ambient shading
    
    float diff = max(dot(normal, lightDir), 0.0);  // diffuse shading
   
    //diff=discretize(diff);
    vec3 diffuse = diff * material.diffuse;  
    vec3 toCel = (diffuse+ambient)*light.color;
    toCel.r=discretize(toCel.r);
     toCel.g=discretize(toCel.g);
      toCel.b=discretize(toCel.b);
          vec3 saved= toCel;
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color * spec * material.specular;
     vec3 reflection  = texture(skybox, reflect(FragPos-viewPos, normal)).rgb;
    // attenuation
     //  if(spec>0.8){specular+=reflection*0.5;}

    
    //float value= max(diffuse.r, diffuse.g);
	//value=max(value, diffuse.b);
	//value=discretize(value);
	//diffuse=diffuse*val;
    //float maximum=max(saved.x,saved.y);
    //maximum=max(maximum, saved.z);
    //maximum=discretize(maximum);
    //saved=saved*maximum;
    return (saved + specular);  // combine results
}
