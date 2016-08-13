#version 330 core 
uniform float viewDist;
uniform vec3 lightCol;
uniform float matAmb;
uniform float matDif;
uniform vec3 matSpec;
uniform float matShine;
//uniform float brighness; //with 1 being full and 0 being completely dark
uniform sampler2D texture0;
in vec3 fragNorRaw;
in vec2 fragTex;
in vec3 lightVecRaw;
in vec3 halfVecRaw;
in float dist;
in vec3 vertPosWorld;
// out vec4 color;
layout(location = 0) out vec4 color;

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

	 color = vec4(vertCol * 0.5/*brightness*/, max(alpha, 0.0f)); //textures are way too bright so just uniformly darken them by 0.5
    //color = vec4(tex, 1); //+ caustColor, 1);
	//color = vec4(fragTex.st, 0, 1);
}
