#version 330

layout (location = 0) in vec3 position;
//layout (location = 1) in vec4 color;
layout (location = 1) in vec3 normal;

smooth out vec4 fragColor;
smooth out vec3 fragPosition;
smooth out vec3 fragNormal;

uniform mat4 modelToWorld;
uniform mat4 rotation;
uniform mat4 perspective;
uniform vec3 worldPosition;

void main() {
	vec4 nw = vec4(1.0);
	nw.xyz = normal;
	nw = rotation*nw;
	vec4 color = vec4(0.8, 0.8, 0.8, 1.0);
	gl_Position.xyz = position;
	gl_Position.w = 1.0;
	fragPosition = position + worldPosition;
	gl_Position = perspective*gl_Position;

	fragColor = color;
	fragNormal = nw.xyz;
}
