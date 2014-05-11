#pragma once

#include <windows.h>
#include <glm/glm.hpp>

namespace input{
	const size_t maxBut = 256;// the greatest virtual key code+1 //actually 255

	extern bool buts[maxBut];

	extern glm::vec2 moose;

	extern void press(USHORT button);
	extern void unpress(USHORT button);

	extern bool pressed(USHORT button);

	extern void moveMouse(long dx, long dy);

	extern glm::vec2 mouse();
}