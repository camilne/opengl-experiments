#version 330 core

in vec3 fragPos;
in vec3 f_normal;
in vec2 f_texCoords;
in vec4 f_fragPosLightSpace;

out vec3 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec4 seed4) {
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	float bias = max(0.015 * (1.0 - dot(normal, lightDir)), 0.005);
	float shadow = 0.0;
	for(int i = 0; i < 4; i++) {
		int index = int(16.0*random(vec4(floor(fragPos.xyz*1000.0), i)))%16;
		//visibility -= 0.2*(1.0-texture( shadowMap, vec3(ShadowCoord.xy + poissonDisk[index]/700.0,  (ShadowCoord.z-bias)/ShadowCoord.w) ));
		float poissonDepth = texture(shadowMap, projCoords.xy + poissonDisk[index]/500.0).r;
		shadow += currentDepth - bias > poissonDepth ? 1.0 : 0.0; 
	}
	
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 13.0;
	
	if(projCoords.z > 1.0)
	shadow = 0.0;
	
	return shadow;
}

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(f_normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specularStrength = 1.0;
	float shininess = 64.0;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
	vec3 specular = spec * specularStrength * lightColor;
	
	float shadow = ShadowCalculation(f_fragPosLightSpace, norm, lightDir);
	vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
	vec3 textureColor = texture(texture_diffuse1, vec2(f_texCoords.s, 1 - f_texCoords.t)).rgb;
	color = lighting * textureColor;
	
	// Apply gamma correction
	float gamma = 1.5;
	color = pow(color, vec3(1.0/gamma));
}