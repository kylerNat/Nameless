#include "Graphics.h"

namespace graphics{

    GLuint createShader(const GLenum eShaderType, const char * shaderSource){
        GLuint shader = glCreateShader(eShaderType);

        glShaderSource(shader, 1, &shaderSource, NULL);

        glCompileShader(shader);

         GLint status;
         glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
         if (status == GL_FALSE) {
             GLint infoLogLength;
             glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
             GLchar *infoLog = new GLchar[infoLogLength + 1];
             glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
             const char *shaderType = NULL;
             switch(eShaderType)
             {
             case GL_VERTEX_SHADER: shaderType = "vertex"; break;
             case GL_GEOMETRY_SHADER: shaderType = "geometry"; break;
             case GL_FRAGMENT_SHADER: shaderType = "fragment"; break;
             }
             fprintf(stderr, "Compile failure in %s shader:\n%s\n", shaderType, infoLog);
             delete[] infoLog;
         }

         return shader;
    }

	GLuint createProgram(const GLuint * shaders, const int nShaders){
        GLuint program = glCreateProgram();

        for(unsigned i = 0; i < nShaders; i++){
            glAttachShader(program, shaders[i]);
        }

        glLinkProgram(program);

        GLint status;
        glGetProgramiv (program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *infoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
            fprintf(stderr, "Linker failure: %s\n", infoLog);
            delete[] infoLog;
        }

        for(unsigned i = 0; i < nShaders; i++){
            glDetachShader(program, shaders[i]);
        }

        return program;
    }

	GLuint initProgram(){
		const GLuint shaders[] = {
			createShader(GL_VERTEX_SHADER, files::getString("shaders/Vertex.vert")),
			createShader(GL_FRAGMENT_SHADER, files::getString("shaders/Fragment.frag"))
		};

        GLuint program = createProgram(shaders, sizeof(shaders)/sizeof(GLuint));

		for(int i = 0; i < sizeof(shaders)/sizeof(GLuint); i++){
			glDeleteShader(shaders[i]);
		}

        return program;
    }
	
	//TODO:temp combine with init program
	GLuint initShadowProgram(){
		const GLuint shaders[] = {
			createShader(GL_VERTEX_SHADER, files::getString("shaders/Shadow.vert")),
			createShader(GL_FRAGMENT_SHADER, files::getString("shaders/Shadow.frag"))
		};

        GLuint program = createProgram(shaders, sizeof(shaders)/sizeof(GLuint));

		for(int i = 0; i < sizeof(shaders)/sizeof(GLuint); i++){
			glDeleteShader(shaders[i]);
		}

        return program;
    }

	//from http://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl
	bool WGLExtensionSupported(const char *extension_name) {
		// this is pointer to function which returns pointer to string with list of all wgl extensions
		PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

		// determine pointer to wglGetExtensionsStringEXT function
		_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

		if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
		{
			// string was not found
			return false;
		}

		// extension is supported
		return true;
	}
	//endfrom

	int init() {

	
		//from http://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl
		PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
		PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

		if (WGLExtensionSupported("WGL_EXT_swap_control"))
		{
			// Extension is supported, init pointers.
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");

			// this is another function from WGL_EXT_swap_control extension
			wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
		}
		//endfrom

		wglSwapIntervalEXT(0);//TODO: temp
		
		glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRange(0.0f, 1.0f);
        //glEnable(GL_DEPTH_CLAMP);
        return 0;
	}

	float theta = 0;//3.14;
	void draw(HDC dc, GLuint program, GLuint shadowProgram, vertexObject * objects, size_t nObjs, GLuint shadowMap, GLuint framebuffer, const world theWorld){//TODO: add render data(probably a VBO list) as an imput // do swap buffers stuff
		//TODO: fix leaks
		//TODO: redo shaders
		
		glClearColor(0.7, 0.8, 1.0, 1.0);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//temp


		glm::mat4 biasMat(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		GLuint modelToWorld = glGetUniformLocation(program, "modelToWorld");
		GLuint rotation = glGetUniformLocation(program, "rotation");
		GLuint perspective = glGetUniformLocation(program, "perspective");
		GLuint shadowMape = glGetUniformLocation(program, "shadowMap");
		GLuint bias = glGetUniformLocation(program, "biasMat");
		GLuint cameraPos = glGetUniformLocation(program, "cameraPos");
		
		GLuint worldPos = glGetUniformLocation(program, "worldPosition");

		//if(0){
		//	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//	
		//	glUseProgram(shadowProgram);

		//	glm::mat4 lightTransform(
		//		1.0, 0.0, 0.0, 1.0,
		//		0.0, 1.0, 0.0, 0.0,
		//		0.0, 0.0, 1.0, 15.0,
		//		0.0, 0.0, 0.0, 1.0
		//	);

		//	glm::mat4 shadowPersp = glm::ortho<float>(-10.0, 10.0, -10.0, 10.0, -10.0, 20.0);
		//	glm::mat4 lookMat = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -glm::vec3(10.0, 0.0, 15.0), glm::vec3(0.0, 1.0, 0.0));
		//	glm::mat4 depthModelMatrix = glm::mat4(1.0);
		//	glm::mat4 transform = shadowPersp * lookMat * rot;// * transformation;

		//	biasMat = biasMat*transform;

		//	//transform = transform*lightTransform;
		//	//TODO: move some crap outside for speedup// TODO: mabey done? check if the first TODO was completed

		//	glViewport(0, 0, 2048, 2048);

	 //       glClearDepth(1.0f);
		//	glClear(GL_DEPTH_BUFFER_BIT);
		//	
		//	GLuint trans = glGetUniformLocation(shadowProgram, "transformation");

		//	glBindVertexArray(objects[0].vertexArrayObject);

		//	glBindBuffer(GL_ARRAY_BUFFER, objects[0].vertexBufferObject);
		//	glEnableVertexAttribArray(0);
		//	glDisableVertexAttribArray(1);
		//	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		//	glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(objects[0].data.vertexSize*sizeof(objects[0].data.vertexData[0])/2));

		//	glUniformMatrix4fv(trans, 1, GL_FALSE, &transform[0][0]);
		//	glDrawElements(GL_TRIANGLES, objects[0].data.indexSize, GL_UNSIGNED_SHORT, 0);
		//	glBindVertexArray(0);
		//
		//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//}

		glViewport(0, 0, 1920, 1200);

		glClearColor(0.7, 0.8, 1.0, 1.0);
        glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);

		glm::mat4 transformation;
		glm::mat4 rot;
		glm::mat4 look;
		glm::mat4 persp;

		transformation = glm::mat4(1.0);
		transformation[3].x = theWorld.cam.pos.x;
		transformation[3].y = theWorld.cam.pos.y;
		transformation[3].z = theWorld.cam.pos.z;
		transformation[3].w = 1.0;

		rot = glm::mat4(1.0);

		transformation = transformation*rot;

		look = glm::mat4_cast(theWorld.cam.att);

		for(int e = 0; e < n_models; e++){
			int i = theWorld.models[e].id;

			if(i < 0 || i > nObjs){
				continue;
			}
			
			glm::mat4 transformation1(1.0);
			transformation1[3].x = -theWorld.models[e].pos.x;
			transformation1[3].y = -theWorld.models[e].pos.y;
			transformation1[3].z = -theWorld.models[e].pos.z;
			transformation1[3].w = 1.0;

			transformation1 = transformation1*transformation;

			rot = glm::mat4_cast(theWorld.models[e].att);//glm::mat4(1.0);

			transformation1 = transformation1*rot;
			//rot = look*rot;

			persp = glm::perspectiveFov(120.0, 1.6, 1.0, 0.1, 10000.0);//perspectiveFov with degrees is depreciated

			persp = persp*look*transformation1;

			glBindVertexArray(objects[i].vertexArrayObject);

			glBindBuffer(GL_ARRAY_BUFFER, objects[i].vertexBufferObject);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(objects[i].data.vertexSize*sizeof(objects[i].data.vertexData[0])/2));

			glUniformMatrix4fv(modelToWorld, 1, GL_FALSE, &transformation1[0][0]);
			glUniformMatrix4fv(rotation, 1, GL_FALSE, &rot[0][0]);
			glUniformMatrix4fv(perspective, 1, GL_FALSE, &persp[0][0]);
			glUniformMatrix4fv(bias, 1, GL_FALSE, &biasMat[0][0]);

			glUniform3fv(cameraPos, 1, &theWorld.cam.pos.x);

			glm::vec3 wrldPos = -theWorld.models[e].pos;

			glUniform3fv(worldPos, 1, &wrldPos.x);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, shadowMap);
			glUniform1i(shadowMape, 0);
			glDrawElements(GL_TRIANGLES, objects[i].data.indexSize, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
		}
		//endtemp
		SwapBuffers(dc);
	}
}