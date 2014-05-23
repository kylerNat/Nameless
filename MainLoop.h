#pragma once

#include "Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

struct particle{//a position, velocity, and acceleration
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

struct controlData{
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
	controlData playerRel;
	particle playerPart;
	particle knife;
	bool knifeSwitching;
	float knifeAngle;
	bool knifeSide;

	particle * rockets;

	//glm::vec3 ** positions;
	int * modelIds;
	float slowMoTimer;
	particle * enemies;
	float * enemyHealth;
};

namespace mainLoop{
	extern world createWorld();//runtime: 7 days

	extern particle particleLoop(particle p, float dt);

	extern glm::vec3 * getPositions(world theWorld);
	extern glm::mat4 * getTransforms(world theWorld);

	extern world loop(world oldWorld);

	extern world playerLoop(world oldWorld, float dt, float phi, float theta);
}