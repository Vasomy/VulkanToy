
#version 450
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Texcoord;

layout(location = 0) out vec2 texcoord;

layout(binding = 0) uniform ViewProjMat{
	mat4 view;
	mat4 proj;

}mvp;

layout(push_constant)uniform PushConstant
{
	mat4 model;
}pc;

void main()
{
	gl_Position = mvp.proj*mvp.view*pc.model*vec4(Position,1.0f);
	texcoord = Texcoord;
}