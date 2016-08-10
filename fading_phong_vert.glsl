#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragNorRaw;
out vec3 lightVecRaw;
out vec3 halfVecRaw;
out vec3 vertPosWorld;
out float dist;

//uniform mat4 caust_P;
//uniform mat4 caust_MV;
//varying vec4 caust_pos;

void main()
{
    //caust_pos = caust_P * caust_MV * aPos;
    
	gl_Position = P * V * M * vertPos;
	gl_PointSize = 10.0;
	fragNorRaw = (M * vec4(vertNor, 0.0)).xyz;

	// Compute Phong vectors.
	vertPosWorld = (M * vertPos).xyz;
	vec3 viewVec = normalize(camPos - vertPosWorld);
	lightVecRaw = normalize(lightPos - vertPosWorld);
	halfVecRaw = 0.5f * (viewVec + lightVecRaw);

	// Calculate dist for fading.
	vec3 camDist = camPos - vertPosWorld;
	dist = length(vec2(camDist.x, camDist.z));
}
