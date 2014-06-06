#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 color;

smooth out vec3 fragPosition;
smooth out vec3 fragNormal;
smooth out vec2 fragUV;
smooth out vec3 fragColor;
flat out vec3 vertPosition;
flat out vec2 vertUV;

uniform mat4 normalTransform;
uniform mat4 perspective;
uniform vec3 worldPosition;

void main() {
	gl_Position.xyz = position;
	gl_Position.w = 1.0;
	fragPosition = position + worldPosition;
	vertPosition = fragPosition;
	gl_Position = perspective*gl_Position;
	
	vec4 nw = vec4(normal, 1.0);
	nw = normalTransform*nw;
	fragNormal = nw.xyz;

	fragUV = uv;
	vertUV = fragUV;

	fragColor = color;
}
