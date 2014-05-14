#pragma once

#include "Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>

struct particle{//a position, velocity, and acceleration
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

/*figure out later
struct hitbox{
}
*/

struct world{
	bool shaken;
	glm::quat camera;
	particle cameraParticle;
	glm::vec3 cameraPosition;
	particle * parts;//the zeroth is the player
};

namespace mainLoop{
	extern world createWorld();//runtime: 7 days

	extern world loop(world oldWorld);

	extern world playerLoop(world oldWorld, float dt, float phi);
}