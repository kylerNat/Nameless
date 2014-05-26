#include "MainLoop.h"

glm::quat lookQuat(glm::vec3 camera, glm::vec3 center, glm::vec3 up){//lookAt for quaternions
	auto look = camera-center;
	float theta0 = atan2(look.x, look.z);
	auto quat0 = glm::quat(cos(theta0/2), 0.0, sin(theta0/2), 0.0);
	
	auto hor = glm::vec2(look.x, look.z);
	float theta1 = -atan2(look.y, sqrt(glm::dot(hor, hor)));
	auto quat1 = glm::quat(cos(theta1/2), sin(theta1/2), 0.0, 0.0);

	//can probly optimize the maths
	auto upCare = up-glm::proj(up, look);
	auto y = glm::vec3(0.0, 1.0, 0.0)-glm::proj(glm::vec3(0.0, 1.0, 0.0), look);
	auto x = glm::cross(y, look);
	float projX = glm::dot(upCare, x)/sqrt(glm::dot(x, x));
	float projY = glm::dot(upCare, y)/sqrt(glm::dot(y, y));
	float theta2 = -atan2(projX, projY);
	auto quat2 = glm::quat(cos(theta2/2), 0.0, 0.0, sin(theta2/2));

	return quat0*quat1*quat2;
	/*
	float cosine = glm::dot(glm::vec3(0.0, 0.0, 1.0), glm::normalize(camera-center));
	float theta = -acos(cosine);
	return glm::quat(cos(theta/2), sin(theta/2)*glm::normalize(glm::cross(camera-center, glm::vec3(0.0, 0.0, 1.0))));
	*/
}

//TODO: rewrite
namespace mainLoop{
	world createWorld(){//make constructor?
		srand (time(NULL));
		world newWorld;

		newWorld.models = new model[n_models]();

		newWorld.models[0].id = 0;
		newWorld.models[0].pos = glm::vec3(0.0, 0.0, 0.0);
		newWorld.models[0].att = glm::quat();

		newWorld.models[1].id = 1;
		newWorld.models[1].pos = glm::vec3(0.0, 0.0, 0.0);
		newWorld.models[1].att = glm::quat();
		newWorld.knf.part.p = &newWorld.models[1].pos;
		newWorld.knf.att = &newWorld.models[1].att;

		newWorld.enemies = new enemy[100]();
		for(int i = 0; i < 100; i++){
			newWorld.models[2+i].id = 2;
			newWorld.models[2+i].pos = glm::vec3(0.0, 0.0, 0.0);
			newWorld.models[2+i].att = glm::quat();
			newWorld.enemies[i].part.p = &newWorld.models[2+i].pos;
			newWorld.enemies[i].att = &newWorld.models[2+i].att;
			newWorld.enemies[i].health = 10.0f;
			*newWorld.enemies[i].part.p = 1000.0f*glm::vec3((rand()%100-50)/100.0f, 0.0, (rand()%100-50)/100.0f);
		}

		newWorld.slowMoTimer = new float();
		*newWorld.slowMoTimer = 1.0f;

		newWorld.shaking = new bool();

		return newWorld;
	}

	particle particleLoop(particle p, float dt){
		(*p.v) += (*p.a)*dt;
		(*p.p) += 0.5f*(*p.a)*dt*dt+(*p.v)*dt;
		return p;
	}

	particle ground(particle p, float h){
		if(p.p->y >= -h){
			p.p->y = -h;
			p.v->y = 0;
		}
		return p;
	}

	player playerLoop(world wrld, player plr, float dt, float phi, float theta){
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


		if(moveDir != glm::vec2(0.0, 0.0)){
			//straif //TODO: make better
			plr.relAccel->x = 1.0*moveDir.x;

			//direction change boost
			if(plr.relVel->x*moveDir.x <= 0.0){
				plr.relAccel->x *= 2.0*sqrt(1.0+abs(plr.relVel->x));
			}

			if(abs(plr.relVel->x) < 10.0){
				plr.relAccel->x *= 10.0;
			}
			plr.relAccel->x -= 0.5f*plr.relVel->x;

			//forward
			plr.relAccel->y = moveDir.y*1.0*exp(-abs(plr.relVel->y)/7.0);
			//direction change boost
			if(plr.relVel->y*moveDir.y < -0.1){
				plr.relAccel->y *= 2.0*sqrt(abs(plr.relVel->y));
			}

			if(plr.part.p->y >= 0.0){
				if(input::pressed(' ')){
					plr.part.v->y = -5.0;
				}
				else{
					if(abs(plr.relVel->y) < 10.0){
						plr.relAccel->y *= 10.0;
					}
					plr.relAccel->y -= 1.0f*plr.relVel->y;
				}
			}
			plr.part.a->x = cos(phi)*plr.relAccel->x-sin(phi)*plr.relAccel->y;
			plr.part.a->y = gravity;
			plr.part.a->z = cos(phi)*plr.relAccel->y+sin(phi)*plr.relAccel->x;

			plr.part.v->x = cos(phi)*plr.relVel->x-sin(phi)*plr.relVel->y;
			plr.part.v->z = cos(phi)*plr.relVel->y+sin(phi)*plr.relVel->x;
		}
		else{
			plr.part.a->x = 0.0;
			plr.part.a->y = gravity;
			plr.part.a->z = 0.0;

			if(plr.part.p->y >= 0.0){
				if(input::pressed(' ')){
					plr.part.v->y = -5.0;
				}
				else{
					(*plr.part.a) -= 1.0f*(*plr.part.v);
				}
			}
		}

		plr.part = particleLoop(plr.part, dt);
		plr.part = ground(plr.part, 0.0);

		plr.relVel->x = cos(phi)*plr.part.v->x+sin(phi)*plr.part.v->z;
		plr.relVel->y = cos(phi)*plr.part.v->z-sin(phi)*plr.part.v->x;

		return plr;
	}

	camera cameraLoop(camera cam, glm::vec3 target, float dt, float phi, float theta){
		*cam.part.a = -250.0f*(*cam.part.p);
		if(*cam.part.v != glm::vec3(0.0)){
			*cam.part.a -= 20.0f*glm::normalize(*cam.part.v);
		}
		cam.part = particleLoop(cam.part, dt);
		cam.pos = target+*cam.part.p*glm::mat3_cast(cam.att);
		cam.att = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);
		return cam;
	}

	//TODO: add knife screen shake
	knife knifeLoop(world wrld, knife knf, player plr, float dt, float phi, float theta){//the world will be used later to colide the knife with stuff
		glm::vec3 arm;
		
		if(input::pressed(VK_RBUTTON)){
			if(!knf.switching){
				knf.part.v->x = 3.0*(knf.onRight ? 1.0 : -1.0)*cos(phi);
				knf.part.v->y = -3.0;
				knf.part.v->z = 3.0*(knf.onRight ? 1.0 : -1.0)*sin(phi);
				(*knf.part.v) += (*plr.part.v);

				knf.onRight = !knf.onRight;
			}
			knf.switching = true;
		}

		
		if(input::pressed(VK_LBUTTON)){
			knf.angle -= 3.0*dt*knf.angle/abs(knf.angle);
			arm = 7.5f*glm::vec3(-sin(phi+knf.angle)*cos(theta), sin(theta), cos(phi+knf.angle)*cos(theta));
			if(!(*wrld.shaking)){
				auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
				//newWorld.knife.velocity = 50.0f*lookDir;//glm::normalize(randVector);
				*wrld.cam.part.v += /*10.0f*lookDir+*/5.0f*glm::normalize(randVector);
				*wrld.cam.part.v -= *knf.part.v;
			}
			*wrld.shaking = true;
		}
		else {
			*wrld.shaking = false;
			arm = 2.5f*glm::vec3(-sin(phi+knf.angle)*cos(theta+0.5), sin(theta+0.5), cos(phi+knf.angle)*cos(theta+0.5));

			if(abs(knf.angle) < 1.04 && glm::cross((*plr.part.p+arm)-(*knf.part.p), glm::vec3(0.0, 1.0, 0.0)) != glm::vec3(0.0, 0.0, 0.0)){
				*knf.part.v += 15.0f*glm::normalize(glm::cross((*plr.part.p+arm)-(*knf.part.p), glm::vec3(0.0, (knf.onRight ? 1.0f : -1.0f), 0.0)));
			}
			/*if(abs(knf.angle) < 1.05) {
				knf.angle += 3.0*dt*knf.angle/abs(knf.angle);
			}
			else {
				knf.angle = 1.05*(knf.onRight ? 1.0 : -1.0);
			}*/
			knf.angle = 1.05*(knf.onRight ? 1.0 : -1.0);
		}

		if(!knf.switching){
			(*knf.part.a) = 
				  55.0f*(((*plr.part.p)+arm)-(*knf.part.p))
				- 20.0f*glm::proj((*knf.part.v), ((*plr.part.p)+arm)-(*knf.part.p))
				- 15.0f*((*knf.part.v)-glm::proj((*knf.part.v), ((*plr.part.p)+arm)-(*knf.part.p)))
				+ 19.0f*(*plr.part.v);
		}
		else{
			(*knf.part.a) = glm::vec3(0.0, gravity, 0.0);
			if(knf.part.p->y > plr.part.p->y+arm.y-knf.part.p->y && knf.part.v->y > 0.0){//the knife has been caught
				knf.switching = false;
			}
		}

		knf.part = particleLoop(knf.part, dt);
		//knf.part = ground(knf.part, 0.0);

		(*knf.att) =
			lookQuat(
				glm::vec3(0.0, 0.0, 0.0),//eye
					(knf.onRight ? 1.0f : -1.0f)*glm::cross(glm::vec3(0.0, 1.0, 0.0), (*plr.part.p)-(*knf.part.p))
					//turns the knife up and down
					+2.0f*((*plr.part.p)-(*knf.part.p)-0.5f*glm::vec3(0.0, plr.part.p->y-knf.part.p->y, 0.0)),// 2* the plr to knf vector w/o the vertical component
				glm::vec3(0.0, (knf.onRight ? 1.0f : -1.0f), 0.0)//up
			);
			
		return knf;
	}

	enemy * enemyLoop(world wrld, enemy * enemies, player plr, float dt, float phi, float theta){
		for(int i = 0; i < 100; i++){
			auto knifeDist = *wrld.knf.part.p - *enemies[i].part.p;
			
			if(1){//enemies[i].health > 0.0){
				if(*enemies[i].part.p != *plr.part.p){
					*enemies[i].part.a = 1.0f*(5.0f*glm::normalize(*plr.part.p - *enemies[i].part.p) - *enemies[i].part.v);
					if(glm::dot(knifeDist, knifeDist) < pow(1.6+1.0, 2)){
						*enemies[i].part.a = 2.0f*((i & 1) ? 1 : -1)*(5.0f*glm::normalize(glm::cross(*wrld.plr.part.p - *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0)) - *enemies[i].part.v));
					}
				}

				if(glm::dot(knifeDist, knifeDist) < pow(1.6, 2)){
						float dmg = sqrt(glm::dot(*wrld.knf.part.v, *wrld.knf.part.v));
				
						*enemies[i].part.p -= sqrt(pow(1.6f, 2.0f) - glm::dot(knifeDist, knifeDist)) * glm::normalize(knifeDist);
						*enemies[i].part.v +=
							  log(1.0f+0.01f*glm::dot(*wrld.knf.part.v, *wrld.knf.part.v))
							* glm::proj(*wrld.knf.part.v, *wrld.knf.part.p - *enemies[i].part.p)
							- glm::proj(*enemies[i].part.v, *plr.part.p - *enemies[i].part.p);
						enemies[i].part.v->y = -0.5*dmg;

						enemies[i].health -= 0.5*dmg;

						//screen shake
						auto shake = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
						*wrld.cam.part.v = 0.5f*dmg*glm::normalize(shake);

						if(*wrld.slowMoTimer < 0.0){
							*wrld.slowMoTimer = 0.0;
						}
						*wrld.slowMoTimer += 0.0005f*dmg;
						if(*wrld.slowMoTimer > 0.5){
							*wrld.slowMoTimer = 0.5;
						}
				}

				auto playerDist = *plr.part.p - *enemies[i].part.p;

				if(glm::dot(playerDist, playerDist) < pow(2.0, 2)){
					plr.part.v->y = -10.0f;
				}

				enemies[i].part.a->y = 5.75;

				enemies[i].part = particleLoop(enemies[i].part, dt);
				enemies[i].part = ground(enemies[i].part, 0.0);
			}
			
			*enemies[i].att = lookQuat(*plr.part.p, *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0));
		}

		return enemies;
	}

	world loop(world oldWorld){
		world newWorld = oldWorld;

		float dt = 0.0090f;

		*newWorld.slowMoTimer -= dt;

		if(*newWorld.slowMoTimer >= 0.0){
			dt *= 0.2;
		};

		//particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		newWorld.plr = playerLoop(newWorld, newWorld.plr, dt, phi, theta);

		newWorld.cam = cameraLoop(newWorld.cam, *newWorld.plr.part.p, dt, phi, theta);

		newWorld.knf = knifeLoop(newWorld, newWorld.knf, newWorld.plr, dt, phi, theta);

		newWorld.enemies = enemyLoop(newWorld, newWorld.enemies, newWorld.plr, dt, phi, theta);

		return newWorld;
	}
}