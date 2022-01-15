#version 400

in vec2 tex_Coord;
in vec3 FragPos;  
in vec3 Normal; 
in vec3 inLightPos;
in vec3 inViewPos;

uniform sampler2D myTexture;
uniform vec3 lightColor;
uniform vec3 shadowColor;
uniform bool isShadow; 
uniform bool isTextured;
uniform vec3 color;
 
out vec4 out_Color;

void main(void)
{
    vec3 objectColor;

    if (isTextured == true) {
	    objectColor = texture(myTexture, tex_Coord).xyz;
    }
    else {
        objectColor = color;
    }

	// Ambient
    float ambientStrength = 0.5f;
    vec3 ambient_light = ambientStrength * lightColor;
    vec3 ambient_term = ambient_light * objectColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(inLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0); 
    vec3 diffuse_light = lightColor;
    vec3 diffuse_term = diff * diffuse_light * objectColor;

    // Specular
    float specularStrength = 0.5f;
    float shininess = 100.0f;
    vec3 viewDir = normalize(inViewPos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    vec3 specular_light = specularStrength * lightColor;
    vec3 specular_term = spec * specular_light * objectColor;

    // Emission
    vec3 emission = vec3(0.0, 0.0, 0.0);

    vec3 result = emission + (ambient_term + diffuse_term + specular_term);
    
    if (isShadow == true) {
       out_Color=vec4 (shadowColor, 0.5);
    }
    else {
	    out_Color = vec4(result, 0.5f);
    }
}
