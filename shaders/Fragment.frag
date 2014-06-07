#version 330

smooth in vec3 fragPosition;
smooth in vec3 modSpacePos;
smooth in vec3 fragNormal;
smooth in vec3 fragColor;

uniform vec3 cameraPos;
uniform vec3 lights[3];
uniform float m;
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
	return 0.25+0.25*sin(position.x*100.0f)*sin(position.x*100.0f)+0.25*sin(position.y*100.0f)*sin(position.y*100.0f)+0.25*sin(position.z*100.0f)*sin(position.z*100.0f);
}

vec3 procTexGrad(vec3 position){
	return vec3(0.25*2.0*100.0*100.0*sin(position.x*100.0f)*cos(position.x*100.0f), 0.25*2.0*100.0*100.0*sin(position.y*100.0f)*cos(position.y*100.0f), 0.25*2.0*100.0*100.0*sin(position.z*100.0f)*cos(position.z*100.0f));//the gradient of the texture
}

void main(){
	//temp render texture from image for gui
	vec2 pToC = gl_FragCoord.xy - vec2(960, 600); 

	if(dot(pToC, pToC) < 50.0f){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	//endtemp
	
	vec3 grad = 0.0001*procTexGrad(modSpacePos);
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
