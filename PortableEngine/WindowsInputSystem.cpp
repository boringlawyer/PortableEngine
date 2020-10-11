#include "WindowsInputSystem.h"
#include <ctype.h>
WindowsInputSystem::WindowsInputSystem(HWND h) : hwnd(h)
{
}
void WindowsInputSystem::GetKeyPressed()
{
	prevCursorPos = cursorPos;
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);
	cursorPos = glm::vec2(mousePos.x, mousePos.y);


	for (std::_Tree_const_iterator it = keyToFunction.begin(); it != keyToFunction.end(); ++it)
	{
		if (GetAsyncKeyState(it->first) & 0x8000)
		{
			it->second();
		}
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		rightClickFunction();
	}
}

void WindowsInputSystem::RegisterKeyPressFunction(char key, std::function<void()> function)
{
	// To my knowledge, the Windows API requires the char in GetAsyncKeyState() to be uppercase,
	// while Emscripten is case sensitive, i.e. pressing just the A key results in 'a', 
	// while A + Shift is 'A'. toupper() helps to strike a compromise.
	keyToFunction.emplace(toupper(key), function);
}

void WindowsInputSystem::RegisterRightMouseFunction(std::function<void()> rcFunc)
{
	rightClickFunction = rcFunc;
}

glm::vec2 WindowsInputSystem::GetCursorPosition()
{
	return cursorPos;
}

glm::vec2 WindowsInputSystem::GetPreviousCursorPosition()
{
	return prevCursorPos;
}