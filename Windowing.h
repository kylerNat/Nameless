#pragma once

#include "Graphics.h"
#include "MainLoop.h"
#include "Input.h"

#include <windows.h>
//#define WGL_ARB_extensions_string
#include <GL/glew.h>
#include <GL/wglew.h>
#include <time.h>

//temp
#include "Files.h"
//endtemp

namespace window {
	extern LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	extern WNDCLASSEX windowClass(HINSTANCE hInstance, char * className);

	extern HWND createWindow(char * title, char * className, HINSTANCE hInstance);

	extern int createRID();
	extern RAWINPUT * readRID(LPARAM lParam);
	
	extern HDC createDC(HWND hwnd);
	extern HDC createDummyDC(HWND hwnd);

	extern void initGlew(HWND hwnd);

	extern HDC initContext(HWND hwnd);

	extern int window(HINSTANCE hInstance, int nCmdShow);
}