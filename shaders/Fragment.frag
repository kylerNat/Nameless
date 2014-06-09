#version 330

smooth in vec3 fragPosition;
smooth in vec3 modSpacePos;
smooth in vec3 fragNormal;
smooth in vec3 fragColor;

uniform vec3 cameraPos;
uniform vec3 lights[3];
uniform float m;
uniform unsigned int matId;
uniform mat4 normalTransform;

#define pi 3.14159265358979323846264338327950

float beckmann(vec3 position, vec3 normal,vec3 lightIn, float m){
	if(m == 0.0){
		return 0.0;
	}

	float cosine = dot(normalize(normal), normalize(-normalize(position+cameraPos)+normalize(lightIn)) );//cosine between the normal and half vector
	float cosine2 = cosine*cosine;
	float sine2 = (1-cosine2);
	float tangent2 = sine2/cosine2;

	return exp(-tangent2/(m*m))/(pi*m*m*pow(cosine2, 2));
}

float procTex(vec3 position){
	switch(matId){
		case 1:
			float grain =
				fract(
					sin(
						mod(
							dot(
								floor(
									position.xy*200.0
								),
								vec2(12.989, 78.233)
							),
							pi
						)
					) * 43758.5453
				);

			return 0.75+0.25*grain;
			break;
	}
	return 1.0;
}

vec3 procTexGrad(vec3 position){
	switch(matId){
		case 1:
			return 0.025*normalize(cross(vec3(position.xy - floor(position.xy*200.0)/200.0, 0.0), vec3(0.0, 0.0, 1.0)));
			break;
		case 2:
			return 0.5*normalize(position - floor(position*50.0)/50.0);//TODO: fix contour-line-ing
			break;
		case 3:
			return 0.1*vec3(sin(mod(position.x*0.4, pi)), 0.0, sin(mod(position.z*0.4, pi)));
			break;
	}
	return vec3(0.0);
}

void main(){
	//temp render texture from image for gui
	vec2 pToC = gl_FragCoord.xy - vec2(960, 600); 

	if(dot(pToC, pToC) < 50.0f){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	//endtemp
	
	vec3 grad = procTexGrad(modSpacePos);
	//grad -= dot(grad, fragNormal)/dot(fragNormal, fragNormal)*fragNormal;
	if(dot(grad, grad) > 1.0){
		grad = normalize(grad);
	}
	
	vec3 normal = normalize(fragNormal+grad);
	normal = (normalTransform*vec4(normal, 1.0)).xyz;

	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	for(int l = 0; l < 3; l++){ //TODO: figure out how to use the equivelent of sizeof in GLSL
		vec3 lightIn = normalize(lights[l]-fragPosition);
		float cosine = dot(normalize(normal), lightIn);

		gl_FragColor.xyz += 
			fragColor*procTex(modSpacePos)*(
				//lambertian
				+clamp(cosine, 0.0, 1.0)
			)
		
			//specular
			+vec3(1.0, 1.0, 1.0)*(
				+clamp(beckmann(fragPosition, normal, lightIn, m), 0.0, 1.0)
			);
	}
}
