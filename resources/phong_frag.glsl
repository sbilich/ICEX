on 330 core 
uniform vec3 lightCol;
uniform vec3 matAmb;
uniform vec3 matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform float baseAlpha;
in vec3 fragNorRaw;
in vec3 lightVecRaw;
in vec3 halfVecRaw;
in vec3 vertPosWorld;
// out vec4 color;
layout(location = 0) out vec4 color;
// layout(location = 0) out vec3 color;

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

	color = vec4(vertCol, max(alpha * baseAlpha, 0.0f));
	// color = vertCol;
}
