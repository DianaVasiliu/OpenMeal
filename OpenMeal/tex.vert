#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec3 in_Normal;

out vec4 gl_Position;
out vec2 tex_Coord;
out vec3 Normal;
out vec3 FragPos;
out vec3 inLightPos;
out vec3 inViewPos;

uniform mat4 myMatrix;
uniform mat4 viewShader;
uniform mat4 shadowMatrix;
uniform mat4 projectionShader;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int codCol;

void main(void)
{
    gl_Position = projectionShader * viewShader * myMatrix * vec4(in_Position, 1);
    if(codCol == 1) {
       gl_Position = projectionShader * viewShader * shadowMatrix * myMatrix * vec4(in_Position, 1);
    }
    tex_Coord = vec2(texCoord.x, texCoord.y);
    FragPos = vec3(gl_Position);
    Normal = mat3(projectionShader * viewShader * myMatrix) * in_Normal;
    inLightPos = vec3(projectionShader * viewShader * myMatrix * vec4(lightPos, 1.0f));
    inViewPos = vec3(projectionShader * viewShader * myMatrix * vec4(viewPos, 1.0f));
}
