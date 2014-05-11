#include "MainLoop.h"

namespace mainLoop{
	world createWorld(){
		world newWorld;
		newWorld.parts = new particle[10]();
		return newWorld;
	}

	world playerLoop(world oldWorld, float dt, float phi){
		world newWorld = oldWorld;

		particle * player = &newWorld.parts[0];

		glm::vec2 moveDir = glm::vec2(0.0, 0.0);

		if(input::pressed('W')){
			moveDir.x += 1.0;
		}
		if(input::pressed('S')){
			moveDir.x -= 1.0;
		}

		if(input::pressed('A')){
			moveDir.y += 1.0;
		}
		if(input::pressed('D')){
			moveDir.y -= 1.0;
		}

		glm::vec3 lookDir = glm::vec3(-sin(phi), 0.0, cos(phi));

		player->velocity.x = 10.0*moveDir.y;
		//								   base + extra for changing direction
		player->acceleration.z = moveDir.x;
		if(player->velocity.z*moveDir.x < -0.1){
			player->acceleration.z *= -2.0*player->velocity.z*moveDir.x;
		}
		
		if(player->position.y >= 0.0){
			if(input::pressed(' ')){
				player->velocity.y = -3.0;
			}
			else{
				//player->acceleration.x -= 1.0f*player->velocity.x;
				player->acceleration.z -= 0.5f*player->velocity.z;
			}
		}
		
		player->acceleration.y = 1.74;

		player->velocity += player->acceleration*dt;

		glm::vec3 ds = (0.5f*player->acceleration*dt*dt+player->velocity*dt);

		player->position += glm::vec3(ds.x*cos(phi) - ds.z*sin(phi), ds.y, ds.z*cos(phi) + ds.x*sin(phi));

		if(player->position.y >= 0.0){
			player->velocity.y = 0.0;
			player->position.y = 0.0;
		}

		return newWorld;
	}

	world loop(world oldWorld){
		world newWorld = oldWorld;

		float dt = 0.0030f;

		particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		newWorld.camera = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);

		newWorld = playerLoop(newWorld, dt, phi);

		return newWorld;
	}
}