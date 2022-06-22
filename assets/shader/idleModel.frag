#version 330 core
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


vec3 computeDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computeDirectionalLightSpec(DirectionalLight light, vec3 normal, vec3 viewDir, Material material);
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
vec3 computePointLightSpec(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material);
float discretize(float f, int steps);
vec3 useCel(vec3 result);

uniform Material material;                        // material vals of the vertex
uniform DirectionalLight directionalLight;        // pos and material vals of directional light source
uniform PointLight pointLights[N_PT_LIGHTS];
//is this where the dog is begraben?
in vec3 position;
in vec3 coorN;

in vec2 TexCoords;
in vec3 n;
uniform sampler2D texture_diffuse;
uniform vec3 viewPos;




void main()
{    
    FragColor = texture(texture_diffuse, TexCoords);
    vec3 norm=normalize(coorN);
    vec3 viewDir=normalize(position-viewPos);

   vec3 result = computeDirectionalLight(directionalLight, norm, viewDir, material);      // influence from the directional light
    // result =  texture(texture_diffuse, TexCoords).rgb;
    for(int i = 0; i < N_PT_LIGHTS; i++){
      result += computePointLight(pointLights[i], norm, position, viewDir, material); 
      }    // compute influence on the vertex from all the point lights
  
 
     //result = useCel(result);
    //translate to hsv

    //result = prelim;
    result+=computeDirectionalLightSpec(directionalLight, norm, viewDir, material);



    for(int i = 0; i < N_PT_LIGHTS; i++){
       result += computePointLightSpec(pointLights[i], norm, position, viewDir, material);
        } 
    //result = useCel(result);
    FragColor = vec4(result, 1.0);
      //FragColor = vec4(1.0);
    normal= vec4(vec3(1,1,1),1);
    depth=vec4(vec3(0),1);
}



//TODO: change this with 1D texture though not necessarily necessary
float discretize(float f, int steps){
if(steps==1) return 1;

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
        v=discretize(v, 20);
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
    vec3 ambient  = light.ambient * texture(texture_diffuse, TexCoords).rgb*material.ambient;  // ambient shading
    float diff = max(dot(normal, lightDir), 0.0);  // diffuse shading
        //diff=discretize(diff,5);

    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, TexCoords).rgb*material.diffuse;  
    vec3 saved= (diffuse+ambient);
    return (saved);
   
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
    vec3 lightDir = normalize(light.pos - fragPos);   // get direction of light ray, pointing from the fragment (fragPos) to the light
    // ambient shading
    vec3 ambient = light.ambient * texture(texture_diffuse, TexCoords).rgb*material.ambient;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //diff=discretize(diff,5);
   vec3 diffuse = light.diffuse * diff * texture(texture_diffuse, TexCoords).rgb*material.diffuse;  
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