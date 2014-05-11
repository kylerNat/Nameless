#version 330

smooth in vec4 fragColor;
smooth in vec3 fragPosition;
smooth in vec3 fragNormal;
smooth in vec3 shadowPosition;

uniform sampler2D shadowMap;
uniform vec3 cameraPos;

#define pi 3.14159265358979323846264338327950

float fresnel(vec3 position, vec3 normal, vec3 lightIn){
	float n0 = 1.3;
	float n1 = 1.5;

	float cosineCamLight = dot(-normalize(position), lightIn);
	float cosineLightNorm = dot(normalize(normal), lightIn);
	float sine2 = (1-cosineLightNorm*cosineLightNorm);

	return (+0.5*cosineCamLight*pow(abs(
		(n0*cosineLightNorm-n1*sqrt(1-(n0/n1)*(n0/n1)*sine2))/
		(n0*cosineLightNorm+n1*sqrt(1-(n0/n1)*(n0/n1)*sine2))
		), 2)

		+0.5*cosineCamLight*pow(abs(
		(n1*cosineLightNorm-n0*sqrt(1-(n0/n1)*(n0/n1)*sine2))/
		(n1*cosineLightNorm+n0*sqrt(1-(n0/n1)*(n0/n1)*sine2))
		), 2));
}

float beckmann(vec3 position, vec3 normal,vec3 lightIn){
	float m = 0.025;//rms slope = sqrt(mean (slope^2))

	float cosine2 = (1-dot(normalize(normal), -normalize(normalize(position-cameraPos)+normalize(lightIn)) ))/2;
	float sine2 = (1-cosine2);
	float tangent2 = sine2/(cosine2);

	return exp(-tangent2/(m*m))/(pi*m*m*pow(cosine2, 2));
}

void main(){

	vec3 lightIn = normalize(fragPosition-vec3(-10.0, 0.0, -15.0));

	float cosine = dot(normalize(fragNormal), lightIn);

/*
	float tolerance = 0.005*tan(acos(cosine));
	tolerance = clamp(tolerance, 0.0, 0.01);
*/
	float tolerance = 0.01;

	float lighting = 1.0;
/*
	shadowPosition.z -= tolerance;
	shadowPosition.z /= shadowPosition.w;
	lighting = 1.0*texture(shadowMap, shadowPosition.xyz);
*/
	if(texture(shadowMap, shadowPosition.xy).x < -tolerance+shadowPosition.z){
		lighting = 0.0;
		//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		//return;
	}
/*
	for(int i = 0; i < 16; i++){
		vec2 texPos;
		texPos.x = shadowPosition.x + 0.005*sin(13485.989*i+43458.12494*shadowPosition.x+14314.31249*shadowPosition.y);//randomish
		texPos.y = shadowPosition.y + 0.005*sin(14905.994*i+31242.41983*shadowPosition.x+19139.14989*shadowPosition.y);
		if(texture(shadowMap, texPos.xy).x < -tolerance+shadowPosition.z){
			lighting -= 1.0/16.0;
			//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
			//return;
		}
	}
*/

	gl_FragColor = 
		fragColor*(
			//lambertian
			+1.0*clamp(cosine, 0.0, 1.0)
		)

		+0.0*vec4(0.7, 0.8, 1.0, 1.0)*(
			//fresnel reflectence
			clamp(fresnel(fragPosition, fragNormal, lightIn), 0.0, 1.0)
		)
		
		//specular
		+0.2*vec4(1.0, 1.0, 1.0, 1.0)*(
			//+pow(cosine, 1000)

			+beckmann(fragPosition, fragNormal, lightIn)
		);

	gl_FragColor *= lighting;
	//gl_FragColor.xyz = normalize(cameraPos-fragPosition);
	//gl_FragColor += vec4(1.0, 0.0, 0.0, 0.0)*(1-lighting);
/*
	vec2 texcoord = vec2((1.0/1024.0)*(gl_FragCoord.x-500), (1.0/1024.0)*gl_FragCoord.y);
	gl_FragColor = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), (1.0/1200.0)*gl_FragCoord.y)+mix(vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0), (1.0/1920.0)*gl_FragCoord.x);
	gl_FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), texture(shadowMap, shadowPosition.xy).x);//texture2D(shadowMap, texcoord).x );
	//gl_FragColor = texture2D(shadowMap, texcoord);
*/
}
