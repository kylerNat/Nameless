#version 330

smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec2 fragUV;
smooth in vec3 fragColor;
flat in vec3 vertPosition;
flat in vec2 vertUV;

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

void main(){
	//temp render texture from image for gui
	vec2 pToC = gl_FragCoord.xy - vec2(960, 600); 

	if(dot(pToC, pToC) < 50.0f){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}
	//endtemp

	//TODO: optimize math if needed
	vec3 r2the2 = dot(vertPosition-fragPosition, vertPosition-fragPosition);
	float yProjR = (pow(fragUV.y-vertUV.y, 2) - pow(fragUV.x, 2) + r2the2)/(2.0*sqrt(r2the2));
	float yRejR = sqrt();

	normUp = 

	//vec3 normal = normalize(fragNormal + 0.1*(normalTransform*vec4(0.25*423.0*cos(fragUV.x*423.0), 0.25*423.0*cos(fragUV.y*423.0), 0.0, 1.0)).xyz);
	vec3 normal = normalize(fragNormal + 0.1*(
		  0.25*423.0*cos(fragUV.x*423.0) * cross(normUp, fragNormal)
		+ 0.25*423.0*cos(fragUV.y*423.0) * -normUp
	));

	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

	for(int l = 0; l < 3; l++){ //TODO: figure out how to use the equivelent of sizeof in GLSL
		vec3 lightIn = normalize(lights[l]-fragPosition);
		float cosine = dot(normalize(normal), lightIn);

		gl_FragColor.xyz += 
			fragColor*(0.5+0.25*sin(fragUV.x*423.0)+0.25*sin(fragUV.y*423.0))*(
				//lambertian
				0.2//+clamp(cosine, 0.0, 1.0)
			)
		
			//specular
			+vec3(1.0, 1.0, 1.0)*(
				+clamp(beckmann(fragPosition, normal, lightIn, m), 0.0, 1.0)
			);
	}
}
