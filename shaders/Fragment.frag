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
}

/*
float procTex(vec3 position){
	float grain =
		fract(
			sin(
				mod(
					dot(
						vec3(
							floor(
								position.xy*200.0
							)/200.0,
							floor(
								(
									(
										position.z
										+fract(
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
										)
									)
									*(1.0 + fract(
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
									))
								)*5.0
							)/5.0),
						vec3(62.335, 54.154, 13.320)
					)*43894.596,
					pi*2.0
				)
			)
		);
	return 0.75+0.25*grain;
}
*/

/*brushed metal old
highp float rand(vec2 co) {
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt = dot(floor(co*200.0), vec2(a, b));
    highp float sn = mod(dt, pi);
    return fract(sin(sn) * c);
}

float procTex(vec3 position){
	highp float a = 62.335;
	highp float b = 54.154;
	highp float c = 13.320;
	highp float d = 200.0;
	float dt = dot(vec3(floor(position.xy*0)/d, floor(( position.z+rand(position.xy*0)+0.07*sin(mod(position.z*54.534, pi)) )*5.0)/5.0), vec3(a, b, c));
	return 0.75+0.25*fract(sin(dt*43894.596));
}
*/

vec3 procTexGrad(vec3 position){
	return normalize(cross(vec3(position.xy - floor(position.xy*200.0)/200.0, 0.0), vec3(0.0, 0.0, 1.0)));
}

/*pretty cool as scales
vec3 procTexGrad(vec3 position){
	return normalize(position - floor(position*100.0)/100.0);
}
*/

/*
float procTex(vec3 position){
	return 0.25+0.125*(sin(position.x*50.0f)+1.0)/2+0.125*(sin(position.y*50.0f)+1.0)/2+0.125*(sin(position.z*50.0f)+1.0)/2
		+0.125+0.25*(sin(position.x*100.0f)+1.0)/2+0.125*(sin(position.y*100.0f)+1.0)/2+0.125*(sin(position.z*100.0f)+1.0)/2;
}

vec3 procTexGrad(vec3 position){
	return vec3(
		0.125*50.0*cos(position.x*50.0f) + 0.125*100.0*cos(position.x*100.0f),
		0.125*50.0*cos(position.y*50.0f) + 0.125*100.0*cos(position.y*100.0f),
		0.125*50.0*cos(position.z*50.0f) + 0.125*100.0*cos(position.z*100.0f)
	);//the gradient of the texture
}
*/

void main(){
	//temp render texture from image for gui
	vec2 pToC = gl_FragCoord.xy - vec2(960, 600); 

	if(dot(pToC, pToC) < 50.0f){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	//endtemp
	
	vec3 grad = 0.1*procTexGrad(modSpacePos);
	grad -= dot(grad, fragNormal)/dot(fragNormal, fragNormal)*fragNormal;
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
				+clamp(0.1*beckmann(fragPosition, normal, lightIn, m), 0.0, 1.0)
			);
	}
}
