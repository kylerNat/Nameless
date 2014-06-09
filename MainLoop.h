#pragma once

#include "Input.h"

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

#define n_models 203
#define gravity 20.0

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
	float shakeTime;
};

struct enemy {
	particle part;
	glm::quat * att;
	float * health;
};

struct model {
	unsigned int id;
	glm::vec3 pos;
	glm::quat att;
	float m;
	unsigned int matId;
};

struct rocket {
	bool exploded;
	float explosionTimer;
	particle part;
	glm::quat * att;
	unsigned int * modelId;
};

struct rocketLauncher {
	bool shot;
	particle part;
	glm::quat * att;
};

struct world {
	float * slowMoTimer;
	bool * shaking;
	camera cam;
	player plr;
	knife knf;
	enemy * enemies;
	rocket * rkts;
	rocketLauncher rL;
	model * models;
	unsigned int wpn;
};

namespace mainLoop {
	extern world createWorld();//runtime: 7 days

	extern particle particleLoop(particle p, float dt);

	extern glm::vec3 * getPositions(world theWorld);
	extern glm::mat4 * getTransforms(world theWorld);

	extern world loop(world oldWorld, float dt);

	extern bool blowUp(particle p, float dt, rocket * rkts);

	extern player playerLoop(world oldWorld, float dt, float phi, float theta);
}