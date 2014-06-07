#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

smooth out vec3 fragPosition;
smooth out vec3 modSpacePos;
smooth out vec3 fragNormal;
smooth out vec3 fragColor;

uniform mat4 normalTransform;
uniform mat4 perspective;
uniform vec3 worldPosition;

void main() {
	gl_Position.xyz = position;
	gl_Position.w = 1.0;
	fragPosition = position + worldPosition;
	gl_Position = perspective*gl_Position;
	modSpacePos = position;

	/*
	vec4 nw = vec4(normal, 1.0);
	nw = normalTransform*nw;
	fragNormal = nw.xyz;
	*/
	fragNormal = normal;

	fragColor = color;
}
