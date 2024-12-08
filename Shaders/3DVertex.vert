
#version 450
layout(location = 0) in vec2 Position;

layout(set = 0,binding = 0) uniform ViewProjMat{
	mat4 project;
	mat4 view;
    mat4 model;
}vpm;

// layout(push_constant)uniform ModelMat{
// 	mat4 model;
// }modelMat;

void main()
{
	gl_Position = vec4(Position,0.0,1.0f);
}