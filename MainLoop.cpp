#include "MainLoop.h"

namespace mainLoop{
	world createWorld(){
		world newWorld;
		newWorld.parts = new particle[10]();
		newWorld.shaken = true;
		return newWorld;
	}

	world playerLoopFail(world oldWorld, float dt, float phi){
		world newWorld = oldWorld;

		particle * player = &newWorld.parts[0];

		glm::vec2 moveDir = glm::vec2(0.0, 0.0);

		if(input::pressed('W')){
			moveDir.y += 1.0;
		}
		if(input::pressed('S')){
			moveDir.y -= 1.0;
		}

		if(input::pressed('A')){
			moveDir.x += 1.0;
		}
		if(input::pressed('D')){
			moveDir.x -= 1.0;
		}

		player->acceleration.x = moveDir.x*cos(phi)-moveDir.y*sin(phi);
		player->acceleration.z = moveDir.y*cos(phi)+moveDir.x*sin(phi);

		player->acceleration *= log(1.0+3.0+(glm::dot(player->velocity, player->velocity)));

		if(player->position.y >= 0.0){
			if(input::pressed(' ')){
				player->velocity.y = -3.0;
			}
			else{
				player->acceleration -= 0.5f*player->velocity;
			}
		}
		
		player->acceleration.y = 1.74;
		player->velocity += player->acceleration*dt;
		player->position += 0.5f*player->acceleration*dt*dt+player->velocity*dt;

		if(player->position.y >= 0.0){
			player->velocity.y = 0.0;
			player->position.y = 0.0;
		}

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

		player->acceleration.x = moveDir.y;
		if(player->velocity.x*moveDir.y < -0.0){
			//player->velocity *= -1.0;
			player->acceleration.x *= 2.0*sqrt(abs(player->velocity.x));
		}
		if(abs(player->velocity.x) < 10.0){
			player->acceleration.x *= 10.0;
		}
		player->acceleration.x -= 0.5f*player->velocity.x;

		//forward
		player->acceleration.z = moveDir.x*2.0*exp(-abs(player->velocity.z)/37.0);
		if(player->velocity.z*moveDir.x < -0.1){
			//player->velocity *= -1.0;
			player->acceleration.z *= 2.0*sqrt(abs(player->velocity.z));
		}
		
		if(player->position.y >= 0.0){
			if(input::pressed(' ')){
				player->velocity.y = -3.0;
			}
			else{
				if(abs(player->velocity.z) < 10.0){
					player->acceleration.z *= 10.0;
				}
				//player->acceleration.x -= 1.0f*player->velocity.x;
				player->acceleration.z -= 1.0f*player->velocity.z;
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

		if(input::pressed('L')){
			if(!newWorld.shaken){
				newWorld.cameraParticle.velocity.x = 10.0;
			}
			newWorld.shaken = true;
		}else{
			newWorld.shaken = false;
		}

		newWorld.cameraParticle.acceleration = -40.0f*newWorld.cameraParticle.position - 5.0f*newWorld.cameraParticle.velocity;

		newWorld.cameraParticle.velocity += newWorld.cameraParticle.acceleration*dt;
		newWorld.cameraParticle.position += 0.5f*newWorld.cameraParticle.acceleration*dt*dt+newWorld.cameraParticle.velocity*dt;

		newWorld.cameraPosition = newWorld.cameraParticle.position+player->position;

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