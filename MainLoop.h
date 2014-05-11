#pragma once

#include "Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>

struct particle{//a position, velocity, and acceleration
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	//particle():position(0.0, 0.0, 0.0), velocity(0.0), acceleration(0.0) {}
};

/*figure out later
struct hitbox{
}
*/

struct world{
	glm::quat camera;
	particle * parts;//the zeroth is the player
};

namespace mainLoop{
	extern world createWorld();//runtime: 7 days

	extern world loop(world oldWorld);
}