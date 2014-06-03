#version 330

smooth in vec4 fragColor;
smooth in vec3 fragPosition;
smooth in vec3 fragNormal;

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
	vec2 pToC = gl_FragCoord.xy - vec2(960, 600); 

	if(dot(pToC, pToC) < 50.0f){
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
		return;
	}

	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 lightIn = normalize(vec3(16.0, 100.0, -145.0)-fragPosition);

	float cosine = dot(normalize(fragNormal), lightIn);

	gl_FragColor += 
		fragColor*(
			//lambertian
			+1.0*clamp(cosine, 0.0, 1.0)
		)

		+0.0*vec4(0.7, 0.8, 1.0, 1.0)*(
			//fresnel reflectence
			clamp(fresnel(fragPosition, fragNormal, lightIn), 0.0, 1.0)
		)
		
		//specular
		+1.0*vec4(1.0, 1.0, 1.0, 1.0)*(
			//+clamp(pow(cosine, 1000), 0.0, 1.0)

			+beckmann(fragPosition, fragNormal, lightIn)
		);

	lightIn = normalize(vec3(56.0, 10.0, 145.0)-fragPosition);

	cosine = dot(normalize(fragNormal), lightIn);

	gl_FragColor +=
		fragColor*(
			//lambertian
			+1.0*clamp(cosine, 0.0, 1.0)
		)

		+0.0*vec4(0.7, 0.8, 1.0, 1.0)*(
			//fresnel reflectence
			clamp(fresnel(fragPosition, fragNormal, lightIn), 0.0, 1.0)
		)
		
		//specular
		+1.0*vec4(1.0, 1.0, 1.0, 1.0)*(
			//+clamp(pow(cosine, 1000), 0.0, 1.0)

			+beckmann(fragPosition, fragNormal, lightIn)
		);

	lightIn = normalize(vec3(59.0, 1.0, 0.0)-fragPosition);

	cosine = dot(normalize(fragNormal), lightIn);

	gl_FragColor +=
		fragColor*(
			//lambertian
			+1.0*clamp(cosine, 0.0, 1.0)
		)

		+0.0*vec4(0.7, 0.8, 1.0, 1.0)*(
			//fresnel reflectence
			clamp(fresnel(fragPosition, fragNormal, lightIn), 0.0, 1.0)
		)
		
		//specular
		+1.0*vec4(1.0, 1.0, 1.0, 1.0)*(
			//+clamp(pow(cosine, 1000), 0.0, 1.0)

			+beckmann(fragPosition, fragNormal, lightIn)
		);

	gl_FragColor *= 0.75;
	//gl_FragColor.xyz = normalize(cameraPos-fragPosition);
	//gl_FragColor += vec4(1.0, 0.0, 0.0, 0.0)*(1-lighting);
	
	/*
	vec2 texcoord = vec2((1.0/1024.0)*(gl_FragCoord.x-500), (1.0/1024.0)*gl_FragCoord.y);
	gl_FragColor = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), (1.0/1200.0)*gl_FragCoord.y)+mix(vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0), (1.0/1920.0)*gl_FragCoord.x);
	gl_FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0), texture(shadowMap, (1.0/1000.0)*gl_FragCoord.xy).x);//texture2D(shadowMap, texcoord).x );
	//gl_FragColor = texture2D(shadowMap, texcoord);
	*/
}
