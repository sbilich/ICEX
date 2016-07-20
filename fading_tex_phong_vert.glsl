#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragNorRaw;
out vec2 fragTex;
out vec3 lightVecRaw;
out vec3 halfVecRaw;
out float dist;
out vec3 vertPosWorld;

void main()
{
	vec4 viewPos4 = V * M * vertPos;
	gl_Position = P * viewPos4;
	fragNorRaw = (M * vec4(vertNor, 0.0)).xyz;
	fragTex = vertTex;

	// Compute Phong vectors.
	vertPosWorld = (M * vertPos).xyz;
	vec3 viewVec = normalize(camPos - vertPosWorld);
	lightVecRaw = normalize(lightPos - vertPosWorld);
	halfVecRaw = 0.5f * (viewVec + lightVecRaw);

	// Send dist to frag shader for fading.
	vec3 camDist = vertPosWorld - camPos;
	dist = length(vec2(camDist.x, camDist.z));
}
