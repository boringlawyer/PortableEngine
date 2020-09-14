#pragma once
#include "IGraphicsAPI.h"
#include "GameWindow.h"
#ifdef _WIN64
#include "WinOpenGLContext.h"
#elif defined(__EMSCRIPTEN__)
#include "OpenGLESContext.h"
#endif
class OpenGLAPI : public IGraphicsAPI
{
public:
	int Init();
	void ClearScreen();
	void Draw();
	OpenGLAPI(GameWindow*);
private:
	GameWindow* window;
#ifdef _WIN64
	WinOpenGLContext* glContext;
#elif defined(__EMSCRIPTEN__)
	OpenGLESContext* glContext;
#endif
};

