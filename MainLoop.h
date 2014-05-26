#pragma once

#include "Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

#define n_models 102
#define gravity 3.8

struct particle {//a position, velocity, and acceleration
	glm::vec3 * p;
	glm::vec3 * v;
	glm::vec3 * a;
	particle():p(new glm::vec3()),v(new glm::vec3()),a(new glm::vec3()){}
	particle(glm::vec3 * pos, glm::vec3 * vel, glm::vec3 * accel):p(pos), v(vel), a(accel){}
};

struct camera {
	particle part;
	glm::vec3 pos;
	glm::quat att;
};

/*figure out later
struct hitbox {
}
*/

struct player {
	glm::vec2 * relVel;
	glm::vec2 * relAccel;
	particle part;
	player():relVel(new glm::vec2()),relAccel(new glm::vec2()){}
};

struct knife {
	particle part;
	bool switching;
	float angle;
	bool onRight;
	glm::quat * att;
};

struct enemy {
	particle part;
	glm::quat * att;
	float health;
};

struct model {
	unsigned int id;
	glm::vec3 pos;
	glm::quat att;
};

struct world {
	float * slowMoTimer;
	bool * shaking;
	camera cam;
	player plr;
	knife knf;
	enemy * enemies;
	model * models;
};

namespace mainLoop {
	extern world createWorld();//runtime: 7 days

	extern particle particleLoop(particle p, float dt);

	extern glm::vec3 * getPositions(world theWorld);
	extern glm::mat4 * getTransforms(world theWorld);

	extern world loop(world oldWorld);

	extern player playerLoop(world oldWorld, float dt, float phi, float theta);
}