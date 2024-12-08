#version 450
layout(location = 0) out vec4 outColor;
layout(location =0) in vec4 dcolor;

layout(binding = 1)uniform UBO2{
	vec3 color;
}ubo2;
void main()
{
	outColor = vec4(dcolor);
}