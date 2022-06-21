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

#define N_PT_LIGHTS 3

in vec3 Normal;   // perpendicular to the vertex
in vec3 FragPos;  // vertex position in world view
  
 uniform vec3 position;
uniform vec3 viewPos; 
uniform bool isSnowball;
uniform float PI;
uniform Material material;                        // material vals of the vertex
uniform DirectionalLight directionalLight;        // pos and material vals of directional light source
uniform PointLight pointLights[N_PT_LIGHTS];
vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computeDirectionalLightSpec(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
vec3 computePointLightSpec(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
float discretize(float f, int steps);
vec3 useCel(vec3 result);
uniform samplerCube skybox;

void main()
{
float maxDist= 10;
    vec3 norm = normalize(Normal);                // ambiguously the vector "Normal" is not normalized yet (necesserily), only perpendicular
    normal=vec4(norm,1.0);


    vec3 viewDir = normalize(viewPos - FragPos);  // get the direction of view, pointing from fragment (fragPos) to the view (Camera)
        depth = vec4(vec3(length(FragPos)/maxDist),1);
    vec3 result = computeDirectionalLight(directionalLight, norm, viewDir, material);      // influence from the directional light
    //vec3 result = vec3(normalize(norm));
    for(int i = 0; i < N_PT_LIGHTS; i++){
      result += computePointLight(pointLights[i], norm, FragPos, viewDir, material); 
      }    // compute influence on the vertex from all the point lights
  
    if(isSnowball){
      vec3 reflection  = texture(skybox, reflect(viewDir, norm)).rgb;
        result=mix(result*2, reflection*2,0.7);
        normal=vec4(normalize(position-viewPos), 1.0);
        depth=vec4(vec3(length(FragPos)/maxDist),1.0);
    
    }
           result = useCel(result);
    //translate to hsv

    //result = prelim;
    result+=computeDirectionalLightSpec(directionalLight, norm, viewDir, material);



    for(int i = 0; i < N_PT_LIGHTS; i++){
        result += computePointLightSpec(pointLights[i], norm, FragPos, viewDir, material); } 
    //result = useCel(result);
    FragColor = vec4(result, 1.0);
      //FragColor = vec4(1.0);
}

//TODO: change this with 1D texture though not necessarily necessary
float discretize(float f, int steps){
if(steps==1) return 1;
   /*if(f>=0.0 && f<0.1){
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
    */
    float times = ceil(steps*f);
    f= (times)/steps;
    //return f;
    return f;
}

vec3 useCel(vec3 result){
    float cmax= max(result.r, max(result.g,result.b));
    float v=cmax;

    float cmin= min(result.r, min(result.g,result.b));
    float diff=cmax-cmin;
    float s= 0;
    float h=0;
      float a; 
        float b;
        float c;
        
    if (cmax>0.0){
        s=(diff)/cmax;
   
        if(cmax==result.r){
            a=result.g;
            b=result.b;
            c= 0;
            }
       else if(cmax==result.g){
            a=result.b;
            b=result.r;
            c=2;
            }
        else{
            a=result.r;
            b=result.g;
            c=4;
            }         

    //leave out the pi meaning 
 
        h=(((a-b)/(cmax-cmin))+c);


    }
    //discretize value
        v=discretize(v, 10);
    //transform result back into rgb
    float hi  = (floor(h));//why floor doesnt cast to int is a mystery to me but whatever
    //float hi=0;

    float f = (h)-hi;
    float p= v*(1-s);
    float q = v*(1-s*f);
    float t = v*(1-s*(1-f)); 
    vec3 retVal = vec3(0);
     retVal = result;
    if((h<1)||(h>=6))retVal=vec3(v,t,p);
    else if(h>=1&&h<2)retVal=vec3(q,v,p);
    else if(h>=2&&h<3)retVal=vec3(p,v,t);
    else if(h>=3&&h<4)retVal=vec3(p,q,v);
    else if(h>=4&&h<5)retVal=vec3(t,p,v); 
    else if(h>=5&&h<6)retVal=vec3(v,p,q);

return retVal;
}
 
vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 ambient  = light.ambient * material.ambient;  // ambient shading
    float diff = max(dot(normal, lightDir), 0.0);  // diffuse shading
        //diff=discretize(diff,5);

    vec3 diffuse = light.diffuse * diff * material.diffuse;  
    vec3 saved= (diffuse+ambient);
    return (saved);
   
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.pos - fragPos);   // get direction of light ray, pointing from the fragment (fragPos) to the light
    // ambient shading
    vec3 ambient = light.ambient * material.ambient;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //diff=discretize(diff,5);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    // specular shading
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));    
    ambient *= attenuation;
    diffuse *= attenuation;
    vec3 saved= (diffuse+ambient);
    return (saved);  // combine results
}
vec3 computeDirectionalLightSpec(DirectionalLight light, vec3 normal, vec3 viewDir, Material material){
vec3 lightDir = normalize(-light.direction);
vec3 reflectDir = reflect(-lightDir, normal);  // specular shading
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = light.specular * spec * material.specular;
return specular;
}
vec3 computePointLightSpec(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material){
     vec3 lightDir = normalize(light.pos - fragPos);   // get direction of light ray, pointing from the fragment (fragPos) to the light
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * (distance * distance));    
    specular *= attenuation;
     return specular;
}
