#version 330 core
uniform vec3 lightCol;
uniform vec3 lightCol2;
uniform vec3 matAmb;
uniform vec3 matDif;
uniform vec3 matSpec;
uniform float matShine;
uniform float baseAlpha;
uniform sampler2D water;
in vec3 fragNorRaw;
in vec3 lightVecRaw;
in vec3 lightVecRaw2;
in vec3 halfVecRaw;
in vec3 halfVecRaw2;
in vec3 vertPosWorld;
// out vec4 color;
layout(location = 0) out vec4 color;
// layout(location = 0) out vec3 color;

in vec4 caust_pos;

void main()
{
    vec4 caustTex = caust_pos / caust_pos.w;
    caustTex *= 0.5;
    caustTex += 0.5;
    vec3 caustColor = texture(water, caustTex.xy).rgb;
    
	if (vertPosWorld.y < 0.0f) {
	   discard;
	}

	vec3 fragNor = normalize(fragNorRaw);
	vec3 lightVec = normalize(lightVecRaw);
	vec3 halfVec = normalize(halfVecRaw);
    
    vec3 lightVec2 = normalize(lightVecRaw2);
    vec3 halfVec2 = normalize(halfVecRaw2);

	// Compute Phong color.
	vec3 diffuseCol = lightCol * max(dot(fragNor, lightVec), 0.0f) * matDif;
	vec3 specularCol = lightCol * pow(max(dot(fragNor, halfVec), 0.0f), matShine) * matSpec;
    diffuseCol += lightCol2 * max(dot(fragNor, lightVec2), 0.0f) * matDif;
    specularCol += lightCol2 * pow(max(dot(fragNor, halfVec2), 0.0f), matShine) * matSpec;
	vec3 vertCol = diffuseCol + specularCol + matAmb;

	// Compute Fade alpha.
	float alpha = 1.0f;
    
    // density of fog
    float density = 0.03;
    
    const float LOG2 = 1.442695;
    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = exp2(- density * density * z * z * LOG2);
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    
    vec4 fog_color = vec4(0.31f, 0.53f, 0.61f, 1.0f);

	color = mix(fog_color, vec4(vertCol + caustColor, max(alpha * baseAlpha, 0.0f)), fogFactor);
	// color = vertCol;
}
