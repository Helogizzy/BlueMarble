#version 330 core

uniform sampler2D TextureSampler; 
uniform sampler2D CloudsTexture;
uniform float Time;
uniform vec2 CloudsRotationSpeed = vec2(0.008, 0.008);
in vec3 Normal;
in vec3 Color;
in vec2 UV;
uniform vec3 LightDirection;
uniform float LightIntensity;
out vec4 OutColor;

void main(){
	//normaliza para não ter problemas na interpolação linear
	vec3 N = normalize(Normal);

	//inverte a direção de luz para calular o vetor L
	vec3 L = -normalize(LightDirection);
	
	float lambertian = max(dot(N, L), 0.0);

	// vetor V  
	vec3 ViewDirection = vec3(0.0, 0.0, -1.0);
	vec3 V = -ViewDirection;

	//Vetor R 
	vec3 R = reflect(-L, N);

	//Termo especular: (R . V) ^ alpha
	float SpecularReflection = pow(dot(ReflectionDirection, ViewDirection), 50.0);

	//Limita o valor da reflecção especular a números positivos
	SpecularReflection = max(0.0, SpecularReflection);

	vec3 EarthColor = texture(TextureSampler, UV).rgb;
	vec3 CloudColor = texture(CloudsTexture, UV + Time * CloudsRotationSpeed).rgb;
	vec3 FinalColor = (EarthColor + CloudColor) * LightIntensity * lambertian + SpecularReflection;

	OutColor =  vec4(FinalColor, 1.0);
}