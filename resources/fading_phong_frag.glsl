#version 330 core 
uniform vec3 lightCol;
uniform vec3 matAmb;
uniform vec3 matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform float viewDist;
uniform float baseAlpha;
in vec3 fragNorRaw;
in vec3 lightVecRaw;
in vec3 halfVecRaw;
in vec3 vertPosWorld;
in float dist;
// out vec4 color;
layout(location = 0) out vec4 color;
// layout(location = 0) out vec3 color;

//varying vec4 caust_pos;

void main()
{
    //vec4 caustTex = caust_pos / caust_pos.w;
    //caustTex *= 0.5;
    //caustTex += 0.5;
    //vec3 caustColor = texture2D(water, caustTex.xy).rgb;
    
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

    color = vec4(vertCol * 0.5, max((alpha * baseAlpha) - 0.2, 0.0f)); //+ caustColor, max(alpha * baseAlpha, 0.0f));
    //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);            //+ caustColor.r, 0.0f + caustColor.g, 0.0f + caustColor.b, 1.0f);
}
