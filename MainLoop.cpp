#include "MainLoop.h"

namespace mainLoop{
	world createWorld(){
		world newWorld;
		newWorld.parts = new particle[10]();
		return newWorld;
	}

	world loop(world oldWorld){
		world newWorld = oldWorld;

		float dt = 0.0030f;

		particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		newWorld.camera = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);

		glm::vec2 moveDir = glm::vec2(0.0, 0.0);

		if(input::pressed('W')){
			moveDir.x += 1.0;
		}
		if(input::pressed('S')){
			moveDir.x -= 1.0;
		}

		float turn = 0.0;
		if(input::pressed('A')){
			moveDir.y += 1.0;
			turn -= 1.0;//*abs(cos(atan2(player->velocity.x, player->velocity.y)-phi));
		}
		if(input::pressed('D')){
			moveDir.y -= 1.0;
			turn += 1.0;//*abs(cos(atan2(player->velocity.x, player->velocity.y)-phi));
		}

		glm::vec3 lookDir = glm::vec3(-sin(phi), 0.0, cos(phi));
		
//		player->velocity = lookDir*dPos.x;
//		player->acceleration = lookDir*dPos.y;

		//wierd strafe thingy
		//if(dPos != glm::vec2(0.0, 0.0)){
		//	dPos = 1.0f*(dPos)/(0.1f+glm::dot(player->velocity, player->velocity));
		//	if(player->velocity != glm::vec3(0.0)){
		//		dPos *= 1.0f+1.0f-glm::dot(glm::normalize(player->velocity), lookDir);
		//	}
		//	
		//	/*if(cos(atan2(player->velocity.x, player->velocity.y)-phi) < 10.0){
		//		player->velocity.x = dPos.y*cos(phi) - dPos.x*sin(phi);
		//		player->velocity.z = dPos.x*cos(phi) + dPos.y*sin(phi);
		//	}*/
		//}

		player->acceleration.x = moveDir.y;//dPos.y*cos(phi) - dPos.x*sin(phi);
		player->acceleration.z = moveDir.x;//dPos.x*cos(phi) + dPos.y*sin(phi);

		//another weird strafe thingy
		/*if(glm::vec2(player->velocity.z, player->velocity.x) != glm::vec2(0.0, 0.0)){
			glm::vec3 sideDir = glm::normalize(glm::cross(glm::vec3(player->velocity.z, 0.0, -player->velocity.x), glm::vec3(0.0, 1.0, 0.0)));
			glm::vec2 sideAcceleration = 10.0f*glm::proj(glm::vec2(player->acceleration.x, player->acceleration.z), glm::vec2(sideDir.x, sideDir.z));
			player->acceleration.x += sideAcceleration.y;
			player->acceleration.z += sideAcceleration.x;
		}*/

		//and another one
		/*if(glm::cross(player->velocity, glm::vec3(0.0, 1.0, 0.0)) != glm::vec3(0.0)){
			float sideAccelerate = 1.0f - glm::dot(glm::normalize(glm::cross(player->velocity, glm::vec3(0.0, 1.0, 0.0))), lookDir);
			player->acceleration += glm::normalize(glm::cross(player->velocity, glm::vec3(0.0, 1.0, 0.0))) * sideAccelerate * turn;
		}*/
		
		if(player->position.y >= 0.0){
			if(input::pressed(' ')){
				//player->velocity.x += 2.0*(dPos.y*cos(phi) - dPos.x*sin(phi));///glm::dot(player->velocity, player->velocity);
				//player->velocity.z += 2.0*(dPos.x*cos(phi) + dPos.y*sin(phi));///glm::dot(player->velocity, player->velocity);
				player->velocity.y = -1.0;
			}
			else{
				player->acceleration.x -= 1.0f*player->velocity.x;
				player->acceleration.z -= 1.0f*player->velocity.z;
			}
		}
		
		/*if(input::pressed(' ')){
			if(player->position.y >= 0.0){
				player->velocity.y = -1.0;
			}

			player->acceleration.x = 2.0*dPos.y*cos(phi) - dPos.x*sin(phi);
		
			player->acceleration.z = dPos.x*cos(phi) + 2.0*dPos.y*sin(phi);
		}
		else{
			player->velocity.x = 2.0*(dPos.y*cos(phi) - dPos.x*sin(phi));
		
			player->velocity.z = 2.0*(dPos.x*cos(phi) + dPos.y*sin(phi));

			player->acceleration.x = 0.0;
		
			player->acceleration.z = 0.0;
		}*/
		/*if(player->velocity.x*player->velocity.x + player->velocity.z*player->velocity.z < player->acceleration.x*player->acceleration.x + player->acceleration.z+player->acceleration.z){
			player->velocity.x += player->acceleration.x;
			player->velocity.z += player->acceleration.z;
		}*/

		player->acceleration.y = .98;

		player->velocity += player->acceleration*dt;

		glm::vec3 ds = (0.5f*player->acceleration*dt*dt+player->velocity*dt);



		player->position += glm::vec3(ds.x*cos(phi) - ds.z*sin(phi), ds.y, ds.z*cos(phi) + ds.x*sin(phi));

		if(player->position.y >= 0.0){
			player->velocity.y = 0.0;
			player->position.y = 0.0;
		}

		return newWorld;
	}
}