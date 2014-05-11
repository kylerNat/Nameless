#include "Input.h"

namespace input{
	bool buts[maxBut] = {};

	glm::vec2 moose = glm::vec2(0.0, 0.0);

	void press(USHORT button){
		if(button < maxBut){
			buts[button] = true;
		}
	}

	void unpress(USHORT button){
		if(button < maxBut){
			buts[button] = false;
		}
	}

	bool pressed(USHORT button){
		return buts[button];
	}

	void moveMouse(long dx, long dy){
		moose += 0.001f*glm::vec2((float)dx, (float)dy);
	}

	glm::vec2 mouse(){
		return moose;
	}
}