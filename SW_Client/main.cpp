
//#include <stdio.h>
//#include <EGL/egl.h>
//#include <GLES2/gl2.h>
//#include "SW_Client.h"
//#include <emscripten/emscripten.h>
#include "SW_Client.h"
#include "GraphicsCommon.h"


extern "C"
{
	extern void SendMessage(int dPtr, int length);
	extern void CloseConnection();
	extern void PrintRefresh();
}


#include <iostream>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <SDL/SDL.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif


using namespace std;

SDL_Surface* screen;

extern "C" int Initialize(int width, int height)
{
	//initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0)
	{
		screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
		if (screen == NULL)
		{
			cerr << "Could not set video mode: " << SDL_GetError() << endl;
			return 0;
		}
	}
	else
	{
		cerr << "Could not initialize SDL: " << SDL_GetError() << endl;
		return 0;
	}

	//SDL initialised successfully, now load shaders and geometry

	cout << "SDL initialised successfully";

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	glViewport(0, 0, width, height);
	return 1;
}