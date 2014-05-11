#pragma once

#include "Files.h"
#include "MainLoop.h"

#include <windows.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <wglext.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics {
	extern GLuint createShader(GLenum, const char*);
	
	//extern GLuint createProgram(const GLuint * shaders, const int nShaders);

	extern GLuint initProgram();

	//TODO: combine with initProgram
	extern GLuint initShadowProgram();

	extern bool WGLExtensionSupported(const char *extension_name);

	extern int init();

	extern void draw(HDC dc, GLuint program, GLuint shadowProgram, files::modelData data, GLuint vertexArrayObject, GLuint vertexBufferObject, GLuint shadowMap, GLuint framebuffer, world theWorld);
}