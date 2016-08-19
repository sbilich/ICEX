#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 camPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragNorRaw;
out vec2 fragTex;
out vec3 lightVecRaw1;
out vec3 lightVecRaw2;
out vec3 halfVecRaw1;
out vec3 halfVecRaw2;
// out vec3 lightVecRaw[2];
// out vec3 halfVecRaw[2];
out float dist;
out vec3 vertPosWorld;

uniform mat4 caust_V;
out vec4 caust_pos;

void main()
{
    caust_pos = P * caust_V * M * vertPos;

	vec4 viewPos4 = V * M * vertPos;
	gl_Position = P * viewPos4;
	fragNorRaw = (M * vec4(vertNor, 0.0)).xyz;
	fragTex = vertTex;

	// Compute Phong vectors.
	vertPosWorld = (M * vertPos).xyz;
	vec3 viewVec = normalize(camPos - vertPosWorld);
    lightVecRaw1 = normalize(lightPos1 - vertPosWorld);
    lightVecRaw2 = normalize(lightPos2 - vertPosWorld);
    halfVecRaw1 = 0.5f * (viewVec + lightVecRaw1);
    halfVecRaw2 = 0.5f * (viewVec + lightVecRaw2);
    
    
 //    lightVecRaw[0] = normalize(lightPos[0] - vertPosWorld);
 //    lightVecRaw[1] = normalize(lightPos[1] - vertPosWorld);
 //    halfVecRaw[0] = 0.5f * (viewVec + lightVecRaw[0]);
 //    halfVecRaw[1] = 0.5f * (viewVec + lightVecRaw[1]);



	// lightVecRaw[0] = normalize(lightPos[0] - vertPosWorld);
	// lightVecRaw[1] = normalize(lightPos[1] - vertPosWorld);
	// halfVecRaw[0] = 0.5f * (viewVec + lightVecRaw[0]);
	// halfVecRaw[1] = 0.5f * (viewVec + lightVecRaw[1]);

	// Send dist to frag shader for fading.
	vec3 camDist = vertPosWorld - camPos;
	dist = length(vec2(camDist.x, camDist.z));
}
