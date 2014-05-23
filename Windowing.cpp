#include "Windowing.h"

namespace window {
	void resize(GLsizei width, GLsizei height){
		glViewport(0, 0, width, height);
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
		switch(msg)
		{ 
			case WM_INPUT: {
				RAWINPUT * raw = readRID(lParam);

				if (raw->header.dwType == RIM_TYPEKEYBOARD) {
					const RAWKEYBOARD kb = raw->data.keyboard;
					
					if(kb.Flags == RI_KEY_MAKE){
						input::press(kb.VKey);
					}
					else if(kb.Flags == RI_KEY_BREAK){
						input::unpress(kb.VKey);
					}

					if(input::pressed(VK_ESCAPE)){
						DestroyWindow(hwnd);
					}
				}
				if (raw->header.dwType == RIM_TYPEMOUSE) {
					const RAWMOUSE ms = raw->data.mouse;
					
					if(ms.usFlags == MOUSE_MOVE_RELATIVE){
						input::moveMouse(ms.lLastX, ms.lLastY);
					}

					if(ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN){//usButtonFlags can have more than one input at a time
						input::press(VK_LBUTTON);
					}
					else if(ms.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP){
						input::unpress(VK_LBUTTON);
					}

					if(ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN){//usButtonFlags can have more than one input at a time
						input::press(VK_RBUTTON);
					}
					else if(ms.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP){
						input::unpress(VK_RBUTTON);
					}

					raw->header.dwType = 1;
				}
			}
			break;
			case WM_CLOSE:
				DestroyWindow(hwnd);
			break;
			case WM_DESTROY:
				PostQuitMessage(0);
			break;
			case WM_SIZE:
				resize(LOWORD(lParam),HIWORD(lParam));
				break;
			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}

	WNDCLASSEX windowClass(HINSTANCE hInstance, char * className){
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = window::WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName = 0;
		wc.lpszClassName = className;
		wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

		return wc;
	}

	HWND initWindow(char * title, char * className, HINSTANCE hInstance){
		HWND hwnd = CreateWindowEx(
			WS_EX_APPWINDOW,
			className,
			title,
			WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0, 1920, 1200,//position and size
			0, 0, hInstance, 0);

		return hwnd;
	}

	int window(HINSTANCE hInstance, int nCmdShow){
		char className[] = "class name";

		WNDCLASSEX wc = windowClass(hInstance, className);
		if(!RegisterClassEx(&wc)) {
			MessageBox(0, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
		DEVMODE dmScreenSettings;                   // Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);       // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth    = 1920;            // Selected Screen Width
		dmScreenSettings.dmPelsHeight   = 1200;           // Selected Screen Height
		dmScreenSettings.dmBitsPerPel   = 32;             // Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL){
			return -1;
		}
		
		HWND hwnd = initWindow("The... TITLE!", className, hInstance);
		if(hwnd == 0) {
			MessageBox(0, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}

		ShowWindow(hwnd, nCmdShow);
		UpdateWindow(hwnd);

		HDC dc = initContext(hwnd);

		GLuint program = graphics::initProgram();
		
		//TODO: move stuff
		graphics::init();

		MSG Msg;

		//temp
		graphics::vertexObject * vOs = new graphics::vertexObject[3];

		files::modelData data0 = files::getVertexData("models/World1.ply");
		vOs[0].data = data0;

		glGenBuffers(1, &vOs[0].vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vOs[0].vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vOs[0].data.vertexSize*sizeof(vOs[0].data.vertexData[0]), vOs[0].data.vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vOs[0].indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[0].indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,vOs[0]. data.indexSize*sizeof(vOs[0].data.indexData[0]), vOs[0].data.indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vOs[0].vertexArrayObject);
		glBindVertexArray(vOs[0].vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, vOs[0].vertexBufferObject);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(vOs[0].data.vertexSize*sizeof(vOs[0].data.vertexData[0])/2));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[0].indexBufferObject);
		glBindVertexArray(0);

		//second vertex object
		int i = 1;
		files::modelData data1 = files::getVertexData("models/knife.ply");
		vOs[i].data = data1;

		glGenBuffers(1, &vOs[i].vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0]), vOs[i].data.vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vOs[i].indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,vOs[i]. data.indexSize*sizeof(vOs[i].data.indexData[0]), vOs[i].data.indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vOs[i].vertexArrayObject);
		glBindVertexArray(vOs[i].vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0])/2));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBindVertexArray(0);

		//third vertex object
		i = 2;
		files::modelData data2 = files::getVertexData("models/monkey.ply");
		vOs[i].data = data2;

		glGenBuffers(1, &vOs[i].vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0]), vOs[i].data.vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vOs[i].indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,vOs[i]. data.indexSize*sizeof(vOs[i].data.indexData[0]), vOs[i].data.indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vOs[i].vertexArrayObject);
		glBindVertexArray(vOs[i].vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0])/2));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBindVertexArray(0);

		//fourth vertex object
		i = 3;
		files::modelData data3 = files::getVertexData("models/missle.ply");
		vOs[i].data = data3;

		glGenBuffers(1, &vOs[i].vertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0]), vOs[i].data.vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &vOs[i].indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,vOs[i]. data.indexSize*sizeof(vOs[i].data.indexData[0]), vOs[i].data.indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &vOs[i].vertexArrayObject);
		glBindVertexArray(vOs[i].vertexArrayObject);

		glBindBuffer(GL_ARRAY_BUFFER, vOs[i].vertexBufferObject);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, (void*)(vOs[i].data.vertexSize*sizeof(vOs[i].data.vertexData[0])/2));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vOs[i].indexBufferObject);
		glBindVertexArray(0);

		//init shadow stuff
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		GLuint shadowMap;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

		glDrawBuffer(0);

		GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(error != GL_FRAMEBUFFER_COMPLETE){
			fprintf(stderr, "glCheckFramebufferStatus: error %p", error);
			return -1;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLuint shadowProgram = graphics::initShadowProgram();
		//endtemp

		if(createRID() == 0){
			return -1;
		}

		SetCursor(0);//disable cursor

		world theWorld = mainLoop::createWorld();

		do {
			graphics::draw(dc, program, shadowProgram, vOs, 4, shadowMap, frameBuffer, theWorld);
			
			theWorld = mainLoop::loop(theWorld);

			while(PeekMessage(&Msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}

		} while(Msg.message != WM_QUIT);

		return Msg.wParam;
	}

	int createRID(){
		RAWINPUTDEVICE rid[2];

		//mouse
		rid[0].usUsagePage = 0x01; 
		rid[0].usUsage = 0x02;
		rid[0].dwFlags = RIDEV_NOLEGACY;
		rid[0].hwndTarget = 0;

		//keyboard
		rid[1].usUsagePage = 0x01; 
		rid[1].usUsage = 0x06; 
		rid[1].dwFlags = RIDEV_NOLEGACY;
		rid[1].hwndTarget = 0;
		
		return RegisterRawInputDevices(rid, sizeof(rid)/sizeof(rid[0]), sizeof(rid[0]));
	}

	RAWINPUT * readRID(LPARAM lParam){
		BYTE buffer[sizeof(RAWINPUT)] = {};
		size_t size = sizeof(RAWINPUT);

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

		return (RAWINPUT*)buffer;
	}
	
	HDC createDummyDC(HWND hwnd) {
		HDC dc = GetDC(hwnd);

	    PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
			PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
			32,                        //Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			32,                        //Number of bits for the depthbuffer
			0,                        //Number of bits for the stencilbuffer
			0,                        //Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		const int pf = ChoosePixelFormat(dc, &pfd);
		SetPixelFormat(dc, pf, &pfd);

		return dc;
	}

	HDC createDC(HWND hwnd){
		HDC dc = GetDC(hwnd);

		const int attribList[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 32,
			WGL_STENCIL_BITS_ARB, 0,
			0,        //End
		};

		int pixFormat;
		UINT numFormats;

		wglChoosePixelFormatARB(dc, attribList, 0, 1, &pixFormat, &numFormats);

		PIXELFORMATDESCRIPTOR pfd;
		SetPixelFormat(dc, pixFormat, &pfd);
		
		return dc;
	}

	void initGlew(HWND hwnd){
		HDC dc = createDummyDC(hwnd);
		HGLRC glrc = wglCreateContext(dc);

		wglMakeCurrent(dc, glrc);

		glewExperimental = true;
		GLenum err=glewInit();
		if(err!=GLEW_OK) {
			DestroyWindow(hwnd);
		}

		wglMakeCurrent(0, 0);
		wglDeleteContext(glrc);
	}

	HDC initContext(HWND hwnd){
		initGlew(hwnd);
		HDC dc = createDC(hwnd);
		
        int attribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 1,
                WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                0
        };

		HGLRC glrc = wglCreateContextAttribsARB(dc, 0, attribs);

		wglMakeCurrent(dc, glrc);

		return dc;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	return window::window(hInstance, nCmdShow);
}