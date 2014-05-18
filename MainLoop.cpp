#include "MainLoop.h"

//TODO: rewrite
namespace mainLoop{
	world createWorld(){
		world newWorld;
		newWorld.playerRel = controlData();
		newWorld.playerPart = particle();
		newWorld.knife = particle();
		newWorld.shaken = true;
		newWorld.positions = new glm::vec3*[10];
		newWorld.modelIds = new int[10];
		newWorld.positions[0] = &newWorld.cameraPosition;
		newWorld.positions[1] = new glm::vec3();//the main world is at the origin
		newWorld.positions[2] = &newWorld.knife.position;
		newWorld.modelIds[0] = -1;// a model id of -1 sets camera
		newWorld.modelIds[1] = 0;
		newWorld.modelIds[2] = 1;
		srand (time(NULL));
		return newWorld;
	}

	world playerLoopFail(world oldWorld, float dt, float phi){
		world newWorld = oldWorld;

		particle * player = &newWorld.playerPart;

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

		controlData * player = &newWorld.playerRel;
		particle * realPlayer = &newWorld.playerPart;

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
		if(moveDir != glm::vec2(0.0)) {
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
			player->acceleration.z = moveDir.x*2.0*exp(-abs(player->velocity.z)/57.0);
			if(player->velocity.z*moveDir.x < -0.1){
				//player->velocity *= -1.0;
				player->acceleration.z *= 2.0*sqrt(abs(player->velocity.z));
			}
		
			if(realPlayer->position.y >= 0.0){
				if(input::pressed(' ')){
					newWorld.shaken = false;
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
		
			player->acceleration.y = 2.74;

			player->velocity += player->acceleration*dt;

			glm::vec3 ds = (0.5f*player->acceleration*dt*dt+player->velocity*dt);

			realPlayer->position += glm::vec3(ds.x*cos(phi) - ds.z*sin(phi), ds.y, ds.z*cos(phi) + ds.x*sin(phi));

			glm::vec3 vel = player->velocity;

			realPlayer->velocity =  glm::vec3(vel.x*cos(phi) - vel.z*sin(phi), vel.y, vel.z*cos(phi) + vel.x*sin(phi));
		}
		else {
			realPlayer->acceleration = glm::vec3(0.0);

			if(realPlayer->position.y >= 0.0){
				if(input::pressed(' ')){
					realPlayer->velocity.y = -3.0;
				}
				else{
					realPlayer->acceleration -= 1.0f*realPlayer->velocity;
				}
			}
		
			realPlayer->acceleration.y = 2.74;
			realPlayer->velocity += realPlayer->acceleration*dt;
			realPlayer->position += 0.5f*realPlayer->acceleration*dt*dt+realPlayer->velocity*dt;

			glm::vec3 vel = realPlayer->velocity;

			player->velocity =  glm::vec3(vel.x*cos(-phi) - vel.z*sin(-phi), vel.y, vel.z*cos(-phi) + vel.x*sin(-phi));
		}
		if(realPlayer->position.y >= 0.0){
			/*if(!input::pressed(' ')){
				if(!newWorld.shaken){
					auto shakeDir = glm::vec3(0.0, 1.0, 0.0);s
					newWorld.cameraParticle.velocity = 21.0f*glm::normalize(shakeDir);
					newWorld.shaken = true;
				}
			}*/
			player->velocity.y = 0.0;
			realPlayer->velocity.y = 0.0;
			realPlayer->position.y = 0.0;
		}

		if(input::pressed(VK_LBUTTON)){
			if(!newWorld.shaken){
				auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, 0);
				newWorld.cameraParticle.velocity = 100.0f*glm::normalize(randVector);
				//newWorld.cameraParticle.velocity -= glm::proj(newWorld.cameraParticle.velocity, lookDir);
			}
			newWorld.shaken = true;
		}else{
			newWorld.shaken = false;
		}

		newWorld.cameraParticle.acceleration = -450.0f*newWorld.cameraParticle.position;
		
		//camera friction
		if(newWorld.cameraParticle.velocity != glm::vec3(0.0)){
			newWorld.cameraParticle.acceleration -= 50.0f*glm::normalize(newWorld.cameraParticle.velocity);
		}

		newWorld.cameraParticle.velocity += newWorld.cameraParticle.acceleration*dt;
		newWorld.cameraParticle.position += 0.5f*newWorld.cameraParticle.acceleration*dt*dt+newWorld.cameraParticle.velocity*dt;

		newWorld.cameraPosition = newWorld.cameraParticle.position*glm::mat3_cast(newWorld.camera)+realPlayer->position;

		return newWorld;
	}

	world loop(world oldWorld){
		world newWorld = oldWorld;

		float dt = 0.0030f;

		//particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		newWorld.camera = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);

		newWorld = playerLoop(newWorld, dt, phi);

		return newWorld;
	}
}