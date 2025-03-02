//processamento dos vértices

#version 330 core

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InColor;
layout (location = 3) in vec2 InUV;

uniform mat4 NormalMatrix;
uniform mat4 ModelViewProjection;

out vec3 Normal;
out vec3 Color;
out vec2 UV;

void main(){
	Normal = vec3(NormalMatrix * vec4(InNormal,0.0));
	Color = InColor;
	UV = InUV; //openGL interpola esse valor no pipeline

	//Aplica a matriz nos vértices do triângulo
	gl_Position = ModelViewProjection * vec4(InPosition, 1.0);
}