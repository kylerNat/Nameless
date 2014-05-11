#pragma once

#include <sys/stat.h>
#include <fstream>

#include <GL/glew.h>

namespace files{
	extern const char * getString(const char * filename);

	struct modelData{
		size_t vertexSize;
		size_t indexSize;
		GLfloat * vertexData;
		GLushort * indexData;
	};

	extern modelData getVertexData(const char * filename);
}