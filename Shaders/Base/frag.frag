#version 450
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 texcoord;

layout(binding = 1)uniform UBO2{
	vec3 color;
}ubo2;

layout(set = 1,binding = 0)uniform sampler2D Sampler;

void main()
{
	outColor = vec4(ubo2.color,1.0f)*texture(Sampler,texcoord);
}