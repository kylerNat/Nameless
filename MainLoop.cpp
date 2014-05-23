#include "MainLoop.h"

//TODO: rewrite
namespace mainLoop{
	world createWorld(){
		srand (time(NULL));
		world newWorld;
		newWorld.slowMoTimer = 1.0;
		newWorld.playerRel = controlData();
		newWorld.playerPart = particle();
		newWorld.knife = particle();
		newWorld.shaken = true;
		/*
		newWorld.positions = new glm::vec3*[10];
		newWorld.positions[0] = &newWorld.cameraPosition;
		newWorld.positions[1] = new glm::vec3(0.0);//the main world is at the origin
		newWorld.positions[2] = &newWorld.knife.position;
		*/
		newWorld.modelIds = new int[203];
		newWorld.modelIds[0] = -1;// a model id of -1 sets camera
		newWorld.modelIds[1] = 0;
		newWorld.modelIds[2] = 1;
		
		for(int i = 0; i < 100; i++){
			newWorld.modelIds[3+i] = 2;
		}

		for(int i = 0; i < 100; i++){
			newWorld.modelIds[103+i] = 3;
		}

		newWorld.knifeAngle = 1.05;
		newWorld.knifeSwitching = false;
		newWorld.knifeAngle = 1;

		newWorld.rockets = new particle[100]();

		newWorld.enemies = new particle[100]();
		newWorld.enemyHealth = new float[100];
		for(int i = 0; i < 100; i++){
			newWorld.enemies[i].position = glm::vec3((rand()%100-50)/100.0*100.0, 0.0, (rand()%100-50)/100.0*100.0);
			newWorld.enemyHealth[i] = 10.0;
		}
		return newWorld;
	}

	particle particleLoop(particle p, float dt){
		p.velocity += p.acceleration*dt;
		p.position += 0.5f*p.acceleration*dt*dt+p.velocity*dt;
		return p;
	}

	glm::vec3 * getPositions(world theWorld){
		glm::vec3 * positions = new glm::vec3[203]();
		positions[0] = theWorld.cameraPosition;
		positions[1] = glm::vec3(0.0, 0.0, 0.0);//the main world is at the origin
		positions[2] = theWorld.knife.position;
		for(int i = 0; i < 100; i++){
			positions[3+i] = theWorld.enemies[i].position;
		}/*
		for(int i = 0; i < 100; i++){
			positions[103+i] = theWorld.rockets[i].position;
		}*/
		return positions;
	}

	glm::mat4 * getTransforms(world theWorld){
		glm::mat4 * transforms = new glm::mat4[203]();
		//transforms[0] = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), theWorld.cameraPosition, glm::vec3(0.0, 1.0, 0.0));
		transforms[1] = glm::mat4(1.0);//the main world is at the origin
		transforms[2] = glm::inverse(glm::lookAt(glm::vec3(0.0, 0.0, 0.0), (theWorld.knifeSide ? 1.0f : -1.0f)*glm::cross(glm::vec3(0.0, 1.0, 0.0), theWorld.playerPart.position-theWorld.knife.position)+2.0f*(theWorld.playerPart.position-theWorld.knife.position-0.5f*glm::vec3(0.0, theWorld.playerPart.position.y-theWorld.knife.position.y, 0.0)), (theWorld.knifeSide ? 1.0f : -1.0f)*glm::vec3(0.0, 1.0, 0.0)));
		for(int i = 0; i < 100; i++){
			transforms[3+i] = glm::inverse(glm::lookAt(glm::vec3(0.0, 0.0, 0.0), -theWorld.enemies[i].velocity, glm::vec3(0.0, 1.0, 0.0)));
			if(theWorld.enemies[i].velocity == glm::vec3(0.0)){
				transforms[3+i] = glm::inverse(glm::lookAt(glm::vec3(0.0, 0.0, 0.0), theWorld.enemies[i].position- theWorld.playerPart.position, glm::vec3(0.0, 1.0, 0.0)));
			}
		}
		return transforms;
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

	//TODO: split into different functions and rewrite
	world playerLoop(world oldWorld, float dt, float phi, float theta){
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
			player->acceleration.z = moveDir.x*1.0*exp(-abs(player->velocity.z)/7.0);
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
					realPlayer->velocity.y = -5.0;
				}
				else{
					realPlayer->acceleration -= 1.0f*realPlayer->velocity;
				}
			}
		
			realPlayer->acceleration.y = 5.74;
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

		glm::vec3 fullLookDir(0.0);

		if(input::pressed(VK_RBUTTON)){
			if(!newWorld.knifeSwitching){
				newWorld.knife.velocity = glm::vec3(3.0*newWorld.knifeAngle*cos(phi), -5.0, 3.0*newWorld.knifeAngle*sin(phi))+realPlayer->velocity;
				newWorld.knifeSide = !newWorld.knifeSide;
			}
			newWorld.knifeSwitching = true;
		}

		if(input::pressed(VK_LBUTTON)){
			fullLookDir = 2.0f*glm::vec3(-sin(phi)*cos(theta), sin(theta), cos(phi)*cos(theta));
			newWorld.knifeAngle -= 3.0*dt*newWorld.knifeAngle/abs(newWorld.knifeAngle);
			fullLookDir = 1.5f*glm::vec3(-sin(phi+newWorld.knifeAngle)*cos(theta), sin(theta), cos(phi+newWorld.knifeAngle)*cos(theta));
			if(!newWorld.shaken){
				auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
				//newWorld.knife.velocity = 50.0f*lookDir;//glm::normalize(randVector);
				newWorld.cameraParticle.velocity = /*10.0f*lookDir+*/5.0f*glm::normalize(randVector);
				newWorld.cameraParticle.velocity -= glm::proj(newWorld.cameraParticle.velocity, lookDir);
			}
			newWorld.shaken = true;
		}
		else {
			newWorld.knifeAngle = 1.05*(newWorld.knifeSide ? 1 : -1);
			fullLookDir = 0.5f*glm::vec3(-sin(phi+newWorld.knifeAngle)*cos(theta+0.5), sin(theta+0.5), cos(phi+newWorld.knifeAngle)*cos(theta+0.5));
			newWorld.shaken = false;
		}

		newWorld.cameraParticle.acceleration = -250.0f*newWorld.cameraParticle.position;
		
		//camera friction
		if(newWorld.cameraParticle.velocity != glm::vec3(0.0)){
			newWorld.cameraParticle.acceleration -= 20.0f*glm::normalize(newWorld.cameraParticle.velocity);
		}

		newWorld.cameraParticle.velocity += newWorld.cameraParticle.acceleration*dt;
		newWorld.cameraParticle.position += 0.5f*newWorld.cameraParticle.acceleration*dt*dt+newWorld.cameraParticle.velocity*dt;

		newWorld.cameraPosition = newWorld.cameraParticle.position*glm::mat3_cast(newWorld.camera)+realPlayer->position;

		if(!newWorld.knifeSwitching){
			newWorld.knife.acceleration = 75.0f//*exp(sqrt(glm::dot((realPlayer->position+5.0f*fullLookDir)-newWorld.knife.position, (realPlayer->position+5.0f*fullLookDir)-newWorld.knife.position))*1.1f)
											*((realPlayer->position+5.0f*fullLookDir)-newWorld.knife.position)
											- 2.0f*sqrt(10.0f*10.0f)*newWorld.knife.velocity
											+ 19.0f*realPlayer->velocity;
		}
		else{
			newWorld.knife.acceleration = glm::vec3(0.0, 5.74, 0.0);
			if(newWorld.knife.position.y > realPlayer->position.y+5.0*sin(theta+0.5) && newWorld.knife.velocity.y > 0.0){//the knife has been caught
				newWorld.knifeSwitching = false;
				auto randVector = newWorld.knife.velocity;//glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
				newWorld.cameraParticle.velocity = 5.0f*glm::normalize(randVector);
			}
		}

		if(glm::dot((realPlayer->position+5.0f*fullLookDir)-newWorld.knife.position, (realPlayer->position+5.0f*fullLookDir)-newWorld.knife.position) > 1.0){
			//newWorld.knife.acceleration += 10.0f*realPlayer->velocity;
		}
		newWorld.knife.velocity += newWorld.knife.acceleration*dt;
		newWorld.knife.position += 0.5f*newWorld.knife.acceleration*dt*dt+newWorld.knife.velocity*dt;

		return newWorld;
	}

	world enemiesLoop(world oldWorld, float dt){
		world newWorld = oldWorld;
		for(int i = 0; i < 100; i++){
			if(newWorld.enemyHealth[i] >= 0.0){
				auto knifeDist = newWorld.knife.position - newWorld.enemies[i].position;

				if(newWorld.enemies[i].position != newWorld.playerPart.position){
					newWorld.enemies[i].acceleration = 1.0f*(5.0f*glm::normalize(newWorld.playerPart.position - newWorld.enemies[i].position) - newWorld.enemies[i].velocity);
					if(glm::dot(knifeDist, knifeDist) < pow(1.6+1.0, 2)){
						newWorld.enemies[i].acceleration = 2.0f*((i & 1) ? 1 : -1)*(5.0f*glm::normalize(glm::cross(newWorld.playerPart.position - newWorld.enemies[i].position, glm::vec3(0.0, 1.0, 0.0)) - newWorld.enemies[i].velocity));
					}
				}

				if(glm::dot(knifeDist, knifeDist) < pow(1.6, 2)){
					newWorld.enemies[i].position -= sqrt(pow(1.6f, 2.0f) - glm::dot(knifeDist, knifeDist)) * glm::normalize(knifeDist);
					newWorld.enemies[i].velocity += log(1.0f+0.01f*glm::dot(newWorld.knife.velocity, newWorld.knife.velocity))
													*glm::proj(newWorld.knife.velocity, newWorld.knife.position - newWorld.enemies[i].position)
												  - glm::proj(newWorld.enemies[i].velocity, newWorld.playerPart.position - newWorld.enemies[i].position);
					newWorld.enemies[i].velocity.y = -0.5*sqrt(glm::dot(newWorld.knife.velocity, newWorld.knife.velocity));

					newWorld.enemyHealth[i] -= 0.5*sqrt(glm::dot(newWorld.knife.velocity, newWorld.knife.velocity));

					//screen shake
					auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
					newWorld.cameraParticle.velocity = 0.05f*sqrt(glm::dot(newWorld.knife.velocity, newWorld.knife.velocity))*glm::normalize(randVector);

					if(newWorld.slowMoTimer < 0.0){
						newWorld.slowMoTimer = 0.0;
					}
					newWorld.slowMoTimer += 0.005f*sqrt(glm::dot(newWorld.knife.velocity, newWorld.knife.velocity));
					if(newWorld.slowMoTimer > 0.5){
						newWorld.slowMoTimer = 0.5;
					}
				}

				auto playerDist = newWorld.playerPart.position - newWorld.enemies[i].position;

				if(glm::dot(playerDist, playerDist) < pow(2.0, 2)){
					newWorld.playerPart.velocity.y = -10.0f;
				}

				newWorld.enemies[i].acceleration.y = 5.75;
				
				newWorld.enemies[i].velocity += newWorld.enemies[i].acceleration*dt;
				newWorld.enemies[i].position += 0.5f*newWorld.enemies[i].acceleration*dt*dt+newWorld.enemies[i].velocity*dt;
			
				if(newWorld.enemies[i].position.y >= 0.0){
					newWorld.enemies[i].velocity.y = 0.0;
					newWorld.enemies[i].position.y = 0.0;
				}
			}
			else{
				newWorld.enemies[i].acceleration.y = 5.75;

				auto horVel = glm::vec2(newWorld.enemies[i].velocity.x, newWorld.enemies[i].velocity.z);

				if(glm::dot(horVel, horVel) >= 0.01 || newWorld.enemies[i].position.y < 1.0) {
					newWorld.enemies[i].velocity += newWorld.enemies[i].acceleration*dt;
					newWorld.enemies[i].position += 0.5f*newWorld.enemies[i].acceleration*dt*dt+newWorld.enemies[i].velocity*dt;
				}

				if(newWorld.enemies[i].position.y >= 4.6){
					newWorld.enemies[i].acceleration = -1.0f*newWorld.enemies[i].velocity;
					newWorld.enemies[i].velocity.y = 10.0;
					newWorld.enemies[i].position.y = 4.6;
				}
			}
		}
		return newWorld;
	}

	world rocketLoop(world oldWorld, float dt) {
		auto newWorld = oldWorld;
		for(int i = 0; i < 100; i++){
			if(newWorld.rockets[i].position.y >= 100.0f){
				continue;
			}
			newWorld.rockets[i] = particleLoop(newWorld.rockets[i], dt);
		}
		return newWorld;
	}

	world loop(world oldWorld){
		world newWorld = oldWorld;

		float dt = 0.0090f;

		newWorld.slowMoTimer -= dt;

		if(newWorld.slowMoTimer >= 0.0){
			dt *= 0.2;
		}

		//particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		newWorld.camera = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);

		newWorld = playerLoop(newWorld, dt, phi, theta);

		newWorld = enemiesLoop(newWorld, dt);

		return newWorld;
	}
}