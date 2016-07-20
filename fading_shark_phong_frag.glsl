#version 330 core 
uniform vec3 lightCol;
uniform vec3 matAmb;
uniform vec3 matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform float viewDist;
uniform float baseAlpha;
uniform vec3 cenPos;
in vec3 fragNorRaw;
in vec3 lightVecRaw;
in vec3 halfVecRaw;
in vec3 vertPosWorld;
in float dist;
// out vec4 color;
layout(location = 0) out vec4 color;

void main()
{
	if (vertPosWorld.y < 0.0f) {
	   discard;
	}

	vec3 fragNor = normalize(fragNorRaw);
	vec3 lightVec = normalize(lightVecRaw);
	vec3 halfVec = normalize(halfVecRaw);

	// Compute Phong color.
	vec3 diffuseCol = lightCol * max(dot(fragNor, lightVec), 0.0f) * matDif;
	vec3 specularCol = lightCol * pow(max(dot(fragNor, halfVec), 0.0f), matShine) * matSpec;
	vec3 vertCol = diffuseCol + specularCol + matAmb;

	// Compute Fade alpha.
	float alpha = 1.0f;
	float fadeBegin = 0.5f * viewDist;
	float fadeEnd = viewDist;
	if (dist > fadeBegin) {
	   alpha = 1.0f - (dist - fadeBegin)/(fadeEnd - fadeBegin);
	}

	// Lighten up the belly.
	vec3 lighterBelly = vec3(1.0f, 1.0f, 1.0f);
	float a = -1.0f;
	float b = -0.75f;
	float c = fragNor.y;
	if (c >= a && c <= b) {
		float d = sqrt(pow(b - a, 2.0f) - pow(c - a, 2.0f)) / (b - a);
		lighterBelly = d * (0.25f * vec3(1.0f, 1.0f, 1.0f) / vertCol);
	}

	color = vec4(vertCol * lighterBelly, max(alpha * baseAlpha, 0.0f));
}
