#version 330 core 
//uniform float viewDist;
//uniform vec3 lightCol;
//uniform float lightCol[6];
uniform vec3 lightCol1;
uniform vec3 lightCol2;
uniform float matAmb;
uniform float matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform bool isAgisoftModel;
//uniform float brightness; //with 1 being full and 0 being completely dark
uniform sampler2D texture0;
uniform sampler2D water;
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

in vec4 caust_pos;
uniform bool caust;

void main()
{
    vec4 caustTex = caust_pos / caust_pos.w;
    caustTex *= 0.5;
    caustTex += 0.5;
    vec3 caustColor = texture(water, caustTex.xy).rgb;
    
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
    
 //    lightVec[0] = normalize(lightVecRaw[0]);
 //    lightVec[1] = normalize(lightVecRaw[1]);
 //    vec3 halfVec[2];
 //    halfVec[0] = normalize(halfVecRaw[0]);
 //    halfVec[1] = normalize(halfVecRaw[1]);

	// lightVec[0] = normalize(lightVecRaw[0]);
	// lightVec[1] = normalize(lightVecRaw[1]);
	// vec3 halfVec[2];
	// halfVec[0] = normalize(halfVecRaw[0]);
	// halfVec[1] = normalize(halfVecRaw[1]);

	// Compute Phong color.
	// fragNor = normalize(fragNor);

	// vec3 tex = texture(texture0, fragTex).rgb;
	// vec3 diffuseCol = lightCol * max(dot(fragNor, lightVec), 0.0f) * matDif * tex;
	// vec3 specularCol = lightCol * pow(max(dot(fragNor, halfVec), 0.0f), matShine) * matSpec;
	// vec3 ambientCol = matAmb * tex;
	// vec3 vertCol = diffuseCol + specularCol + ambientCol;
    
    
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

	specularCol = lightCol1 * pow(max(dot(fragNor, halfVec1), 0.0f), matShine) * matSpec;
	specularCol += lightCol2 * pow(max(dot(fragNor, halfVec2), 0.0f), matShine) * matSpec;
	
	vertCol += diffuseCol + specularCol;

	
	
	// Compute Fade alpha.
	// float alpha = 1.0f;
	// float fadeBegin = 0.5f * viewDist;
    // float fadeEnd = viewDist;
	// if (dist > fadeBegin) {
	//   alpha = 1.0f - (dist - fadeBegin)/(fadeEnd - fadeBegin);
	// }
    
    // density of fog
    // float density = 0.0025;
    
    // const float LOG2 = 1.442695;
    // float z = gl_FragCoord.z / gl_FragCoord.w;
    // float fogFactor = exp2(- density * density * z * z * LOG2);
    // fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    
    vec4 fog_color = vec4(0.31f, 0.53f, 0.61f, 1.0f);
    
    vec3 redGreen = vec3(0.4f, 0.3f, 0.3f);
    vec3 finalColor = vertCol - 0.3*(redGreen * log(dist));
    finalColor += vec3(0.0f, 0.0f, 0.1f);

    vec3 seaFloorColor = vec3(0.33f, 0.34f, 0.64f);
    if(isAgisoftModel){
    	finalColor -= 0.2f * ((vec3(1.0f, 1.0f, 1.0f) - seaFloorColor) * max(6 - vertPosWorld.y, 0.0f));
    }

	// color = vec4(vertCol, max(alpha, 0.0f));
    if (caust) {
        color = vec4(finalColor + (caustColor*0.8), 1.0f);
        //color = mix(fog_color, vec4((vertCol + (caustColor*0.3)) * 0.5/*brightness*/, 1.0f), fogFactor);
    } else {
        color = vec4(finalColor, 1.0f);
        //color = mix(fog_color, vec4(vertCol * 0.5/*brightness*/, 1.0f), fogFactor); //textures are way too bright so just uniformly darken them by 0.5
    }

	// color = vec4(fragTex.st, 0, 1);
}
