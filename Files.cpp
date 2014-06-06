#include "Files.h"

namespace files{

	#ifdef WIN32
	#define fileno _fileno
	#define fstat _fstat
	#define stat _stat
	#endif

	const char * getString(const char * filename){
		FILE * file = fopen(filename, "rb");

		//get length
		struct stat file_stats;
		stat(filename, &file_stats);
		off_t length = file_stats.st_size;

		//read the file
		char * out = new char[length+1];
		fread(out, sizeof(out[0]), length, file);
		out[length] = '\0';//NULL-terminate the string

		fclose(file);
		return out;
	}

	modelData getVertexData(const char * filename){
		size_t nVerts = 0;
		size_t nFaces = 0;
		modelData out;

		FILE * file = fopen(filename, "r");
		if(file == 0){
			return out;
		}

		char * keyword = new char[32]();
		while(1){
			if(fscanf(file, "%31s", keyword) < 0){
				return out;//if the fscanf was unsucessful, return 0 (aka NULL)
			}
			if(!strcmp(keyword, "end_header")){
				break;
			}
			else
			if(!strcmp(keyword, "element")){
				char * element_name = new char[32]();
				fscanf(file, "%31s", element_name);
				if(!strcmp(element_name, "vertex")){
					fscanf(file, "%d", &nVerts);
					out.vertexData = new GLfloat[nVerts*3*n_properties+nVerts*2];//3 for the position, 3 for the color, 3 for the normal
				}
				else
				if(!strcmp(element_name, "face")){
					fscanf(file, "%d", &nFaces);
					out.indexData = new GLushort[nFaces*6];//6 per face incase the face is a quad(two triangles)
				}
			}
			else
			if(!strcmp(keyword, "property")){
				//TODO: add checkerstuffs
			}
			char * oooot = new char[1024];
			fgets(oooot, 1024, file);
		}

		for(size_t v = 0; v < nVerts; v++){
			for(int c = 0; c < 3; c++){
				fscanf(file, "%f", &out.vertexData[3*v+c]);
			}
			for(int c = 0; c < 3; c++){
				fscanf(file, "%f", &out.vertexData[nVerts*3+3*v+c]);
			}
			for(int c = 0; c < 2; c++){
				fscanf(file, "%f", &out.vertexData[nVerts*3*2+2*v+c]);
			}
			for(int c = 0; c < 3; c++){
				float color;
				fscanf(file, "%f", &color);
				out.vertexData[nVerts*3*2+nVerts*2+3*v+c] = color/255.0;
			}
		}
		
		
		unsigned int face = 0;
		for(size_t f = 0; f < nFaces; f++){
			unsigned int faceSize;
			fscanf(file, "%u", &faceSize);
			fscanf(file, "%u", &out.indexData[3*face+0]);
			fscanf(file, "%u", &out.indexData[3*face+1]);
			for(unsigned int v = 0; v < faceSize-2; v++){
				if(v > 0){
					out.indexData[3*face+0] = out.indexData[3*(face-1)+0];
					out.indexData[3*face+1] = out.indexData[3*(face-1)+2];
				}
				fscanf(file, "%u", &out.indexData[3*face+2]);
				face++;
			}
		}

		out.indexSize = face*3;
		out.vertexSize = nVerts*3*n_properties+nVerts*2;
		return out;//figure out return values
	}
}