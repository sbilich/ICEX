#version 330 core 
uniform float viewDist;
uniform vec3 lightCol;
uniform float matAmb;
uniform float matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform sampler2D texture0;
in vec3 fragNorRaw;
in vec2 fragTex;
in vec3 lightVecRaw;
in vec3 halfVecRaw;
in float dist;
in vec3 vertPosWorld;
// out vec4 color;
layout(location = 0) out vec4 color;

void main()
{
	if (vertPosWorld.y < 0.0f) {
	   discard;
	}

	// Re-Normalize
	vec3 fragNor = normalize(fragNorRaw);
	vec3 lightVec = normalize(lightVecRaw);
	vec3 halfVec = normalize(halfVecRaw);

	// Compute Phong color.
	fragNor = normalize(fragNor);

	vec3 tex = texture(texture0, fragTex).rgb;
	vec3 diffuseCol = lightCol * max(dot(fragNor, lightVec), 0.0f) * matDif * tex;
	vec3 specularCol = lightCol * pow(max(dot(fragNor, halfVec), 0.0f), matShine) * matSpec;
	vec3 ambientCol = matAmb * tex;
	vec3 vertCol = diffuseCol + specularCol + ambientCol;

	// Compute Fade alpha.
	float alpha = 1.0f;
	float fadeBegin = 0.5f * viewDist;
	float fadeEnd = viewDist;
	if (dist > fadeBegin) {
	   alpha = 1.0f - (dist - fadeBegin)/(fadeEnd - fadeBegin);
	}

	// color = vec4(vertCol, max(alpha, 0.0f));
	color = vec4(tex, 1);
	// color = vec4(fragTex.st, 0, 1);
}
