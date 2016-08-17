#version 330 core 
uniform float viewDist;
uniform float lightCol[6];
uniform vec3 lightCol1;
uniform vec3 lightCol2;
uniform float matAmb;
uniform float matDif;
uniform vec3 matSpec;
uniform float matShine;
//uniform float brighness; //with 1 being full and 0 being completely dark
uniform sampler2D texture0;
in vec3 fragNorRaw;
in vec2 fragTex;
in vec3 lightVecRaw1;
in vec3 lightVecRaw2;
in vec3 halfVecRaw1;
in vec3 halfVecRaw2;
in vec3 lightVecRaw[2];
in vec3 halfVecRaw[2];
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
	vec3 lightVec[2];
	vec3 lightVec1 = normalize(lightVecRaw1);
	vec3 lightVec2 = normalize(lightVecRaw2);
	vec3 halfVec1 = normalize(halfVecRaw1);
	vec3 halfVec2 = normalize(halfVecRaw2);

	lightVec[0] = normalize(lightVecRaw[0]);
	lightVec[1] = normalize(lightVecRaw[1]);
	vec3 halfVec[2];
	halfVec[0] = normalize(halfVecRaw[0]);
	halfVec[1] = normalize(halfVecRaw[1]);

	// Compute Phong color.
	// fragNor = normalize(fragNor);

	// vec3 tex = texture(texture0, fragTex).rgb;
	// vec3 ambientCol = matAmb * tex;
	// vec3 diffuseCol;
	// vec3 specularCol;
	// vec3 vertCol = ambientCol;
	// for(int i = 0; i < 2; i++){
	// 	diffuseCol = lightCol[i] * max(dot(fragNor, lightVec[i]), 0.0f) * matDif * tex;
	// 	specularCol = lightCol[i] * pow(max(dot(fragNor, halfVec[i]), 0.0f), matShine) * matSpec;
	// 	vertCol += diffuseCol + specularCol;
	// }

	fragNor = normalize(fragNor);

	vec3 tex = texture(texture0, fragTex).rgb;
	vec3 ambientCol = matAmb * tex;
	vec3 diffuseCol;
	vec3 specularCol;
	vec3 vertCol = ambientCol;
	diffuseCol = lightCol1 * max(dot(fragNor, lightVec1), 0.0f) * matDif * tex;
	diffuseCol += lightCol2 * max(dot(fragNor, lightVec2), 0.0f) * matDif * tex;

	specularCol = lightCol1 * pow(max(dot(fragNor, halfVec1), 0.0f), matShine) * matSpec;
	specularCol += lightCol2 * pow(max(dot(fragNor, halfVec2), 0.0f), matShine) * matSpec;
	
	vertCol += diffuseCol + specularCol;

	
	
	// Compute Fade alpha.
	// float alpha = 1.0f;
	// float fadeBegin = 0.5f * viewDist;
	// float fadeEnd = viewDist;
	// if (dist > fadeBegin) {
	//    alpha = 1.0f - (dist - fadeBegin)/(fadeEnd - fadeBegin);
	// }

	 color = vec4(vertCol * 0.5/*brightness*/, 1.0f); //textures are way too bright so just uniformly darken them by 0.5
    //color = vec4(tex, 1); //+ caustColor, 1);
	//color = vec4(fragTex.st, 0, 1);
}
