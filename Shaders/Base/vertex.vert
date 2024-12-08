
#version 450
layout(location = 0) in vec3 Position;

layout(binding = 0) uniform ViewProjMat{
	mat4 view;
	mat4 proj;

}mvp;

layout(location = 0) out vec4 dcolor;

layout(push_constant)uniform PushConstant
{
	mat4 model;
}pc;
/*
layout(push_constant)uniform ModelMat{
}modelMat;
*/
void main()
{

	gl_Position = mvp.proj*mvp.view*pc.model*vec4(Position,1.0f);
	dcolor = mvp.proj*mvp.view*pc.model*vec4(Position,1.0f);
	//dcolor = vec4(tr,0,tr,1.0f);
}