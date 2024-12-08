#version 450
layout(location = 0) out vec4 outColor;

layout(set = 0,binding = 1) uniform vec3 cubeColor{
    vec3 color;
}CC;
void main()
{
	outColor = vec4(CC.color,1.0f);
}