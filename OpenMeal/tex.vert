#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec2 texCoord;

out vec4 gl_Position;
out vec4 ex_Color;
out vec2 tex_Coord;

uniform mat4 myMatrix;
uniform mat4 viewShader;
uniform mat4 projectionShader;

void main(void)
{
    gl_Position = projectionShader*viewShader*in_Position;
    ex_Color = vec4(0.0,1.0,0.0,1.0);
    tex_Coord = vec2(texCoord.x, 1 - texCoord.y);
}
