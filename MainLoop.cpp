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
	world createWorld(){
		srand (time(NULL));
		world newWorld;

		newWorld.wpn = 0;

		newWorld.models = new model[n_models]();

		newWorld.models[0].id = 1;
		newWorld.models[0].pos = glm::vec3(0.0, 0.0, 0.0);
		newWorld.models[0].att = glm::quat();
		newWorld.models[0].m = 0.0;
		newWorld.models[0].matId = 3;

		newWorld.models[1].id = 2;
		newWorld.models[1].pos = glm::vec3(0.0, 0.0, 0.0);
		newWorld.models[1].att = glm::quat();	
		newWorld.models[1].m = 0.325;
		newWorld.models[1].matId = 1;
		newWorld.knf.part.p = &newWorld.models[1].pos;
		newWorld.knf.att = &newWorld.models[1].att;

		newWorld.enemies = new enemy[100]();
		for(int i = 0; i < 100; i++){
			newWorld.models[2+i].id = 3;
			newWorld.models[2+i].pos = glm::vec3(0.0, 0.0, 0.0);
			newWorld.models[2+i].att = glm::quat();
			newWorld.models[2+i].m = 0.025;
			newWorld.models[2+i].matId = 2;
			newWorld.enemies[i].part.p = &newWorld.models[2+i].pos;
			newWorld.enemies[i].att = &newWorld.models[2+i].att;
			newWorld.enemies[i].health = new float();
			*newWorld.enemies[i].health = 40.0f;
			*newWorld.enemies[i].part.p = 1000.0f*glm::vec3((rand()%100-50)/100.0f, 0.0, (rand()%100-50)/100.0f);
		}

		newWorld.rkts = new rocket[100]();
		for(int i = 0; i < 100; i++){
			newWorld.models[102+i].id = 0;
			newWorld.models[102+i].pos = glm::vec3(0.0, 0.0, 0.0);
			newWorld.models[102+i].att = glm::quat();
			newWorld.models[102+i].m = 0.325;
			newWorld.models[102+i].matId = 1;
			newWorld.rkts[i].part.p = &newWorld.models[102+i].pos;
			newWorld.rkts[i].att = &newWorld.models[102+i].att;
			
			newWorld.rkts[i].exploded = true;
			newWorld.rkts[i].explosionTimer = 0.0;
			newWorld.rkts[i].modelId = &newWorld.models[102+i].id;
		}

		newWorld.models[202].id = 5;
		newWorld.models[202].pos = glm::vec3(0.0, 0.0, 0.0);
		newWorld.models[202].att = glm::quat();
		newWorld.models[202].m = 0.325;
		newWorld.models[202].matId = 1;
		newWorld.rL.part.p = &newWorld.models[202].pos;
		newWorld.rL.att = &newWorld.models[202].att;

		newWorld.slowMoTimer = new float();
		*newWorld.slowMoTimer = 0.0f;

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
		const float r = 1000.0;
		auto yLess = glm::vec3(p.p->x, 0.0, p.p->z);
		if(glm::dot(yLess, yLess) >= r*r){
			*p.p -= yLess;
			*p.p += r*glm::normalize(yLess);
			*p.v -= glm::proj(*p.v, yLess);
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

		float quickness = 3.0;

		float accel = 0.0;

		if(moveDir != glm::vec2(0.0, 0.0)){
			moveDir = glm::normalize(moveDir);

			if(*plr.dir != glm::vec2(0.0, 0.0)){
				*plr.dir = glm::normalize(*plr.dir);
			
				float dirperdt = 7.0*(1.0+0.25*(1.0-glm::dot(*plr.dir, moveDir)));
			
				if(glm::dot(moveDir - *plr.dir, moveDir - *plr.dir) < dirperdt*dt){
					*plr.dir = moveDir;
				}
				else{
					auto perp = glm::vec2(-plr.dir->y, plr.dir->x);
					bool cw = glm::dot(moveDir - *plr.dir, perp) > 0.0;
					*plr.dir += dirperdt*dt*(cw ? 1.0f: -1.0f)*perp;//glm::normalize(moveDir - *plr.dir);
					if(*plr.dir != glm::vec2(0.0, 0.0)){
						*plr.dir = glm::normalize(*plr.dir);
					}
				}
			}
			else{
				*plr.dir = moveDir;
			}

			accel = quickness*1.0*exp(-abs(plr.speed)/7.0);

			if(plr.part.p->y >= 0.0){
				if(input::pressed(' ')){
					plr.part.v->y = -12.0;
					plr.speed += quickness*2.0*exp(-abs(plr.speed)/14.0);
				}
				else{
					if(plr.speed < 50.0){
						accel *= 10.0;
					}
					accel -= 1.0f*plr.speed;
				}
			}

			plr.part.a->x = cos(phi)*accel*plr.dir->x-sin(phi)*accel*plr.dir->y;
			plr.part.a->y = gravity;
			plr.part.a->z = cos(phi)*accel*plr.dir->y+sin(phi)*accel*plr.dir->x;

			plr.part.v->x = cos(phi)*plr.speed*plr.dir->x-sin(phi)*plr.speed*plr.dir->y;
			plr.part.v->z = cos(phi)*plr.speed*plr.dir->y+sin(phi)*plr.speed*plr.dir->x;
		}
		else{
			plr.part.a->x = 0.0;
			plr.part.a->y = gravity;
			plr.part.a->z = 0.0;

			if(plr.part.p->y >= 0.0){
				if(input::pressed(' ')){
					plr.part.v->y = -12.0;
				}
				else{
					(*plr.part.a) -= 6.0f*(*plr.part.v);
				}
			}
		}

		blowUp(plr.part, dt, wrld.rkts);
		plr.part = particleLoop(plr.part, dt);
		plr.part = ground(plr.part, 0.0);

		auto yless = glm::vec2(plr.part.v->x, plr.part.v->z);

		plr.speed = sqrt(glm::dot(yless, yless));

		if(plr.speed > 1.0) {
			plr.dir->x = cos(phi)*yless.x+sin(phi)*yless.y;
			plr.dir->y = cos(phi)*yless.y-sin(phi)*yless.x;
		}
		else {
			*plr.dir = glm::vec2(0.0, 0.0);
		}
		return plr;
	}

	camera cameraLoop(camera cam, glm::vec3 target, float dt, float phi, float theta){
		*cam.part.a = -750.0f*(*cam.part.p);
		if(*cam.part.v != glm::vec3(0.0)){
			*cam.part.a -= 30.0f*glm::normalize(*cam.part.v);
			if(glm::dot(*cam.part.v, *cam.part.v) <= glm::dot(*cam.part.a*dt, *cam.part.a*dt) && glm::dot(*cam.part.v, *cam.part.a*dt) <= 0.0 && glm::dot(*cam.part.p, *cam.part.p) <= 0.5){
				*cam.part.p = glm::vec3(0.0);
				*cam.part.v = glm::vec3(0.0);
				*cam.part.a = glm::vec3(0.0);
			}
		}
		cam.part = particleLoop(cam.part, dt);
		cam.pos = target+*cam.part.p*glm::mat3_cast(cam.att);
		cam.att = glm::quat(cos(theta/2), sin(theta/2), 0.0, 0.0) * glm::quat(cos(phi/2), 0.0, sin(phi/2), 0.0);
		return cam;
	}

	knife knifeLoop(world wrld, knife knf, player plr, float dt, float phi, float theta){
		glm::vec3 arm;
		
		if(input::pressed(VK_RBUTTON)){
			if(!knf.switching){
				//*knf.part.v = glm::proj(*knf.part.v, glm::vec3(-sin(phi)*cos(theta), sin(theta), cos(phi)*cos(theta)));
				knf.part.v->x += 3.0*(knf.onRight ? 1.0 : -1.0)*cos(phi);
				knf.part.v->y = -9.0;
				knf.part.v->z += 3.0*(knf.onRight ? 1.0 : -1.0)*sin(phi);
				(*knf.part.v) += (*plr.part.v);

				knf.onRight = !knf.onRight;
			}
			knf.switching = true;
		}

		if(!knf.switching){
			if(input::pressed(VK_LBUTTON)){
				knf.angle -= 9.0*dt*knf.angle/abs(knf.angle);
				arm = 7.5f*glm::vec3(-sin(phi+knf.angle)*cos(theta), sin(theta), cos(phi+knf.angle)*cos(theta));
				//if(knf.shakeTime > 0.0){//screen shake
				//	auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
				//	//newWorld.knife.velocity = 50.0f*lookDir;//glm::normalize(randVector);
				//	*wrld.cam.part.v += /*10.0f*lookDir+*/5.0f*glm::normalize(randVector);
				//	*wrld.cam.part.v -= exp(-glm::dot((*knf.part.v), (*knf.part.v))*1.0f)*(*knf.part.v);
				//}
				knf.shakeTime -= dt;
			}
			else {
				knf.shakeTime = 0.1;
				arm = 2.5f*glm::vec3(-sin(phi+knf.angle)*cos(theta+0.5), sin(theta+0.5), cos(phi+knf.angle)*cos(theta+0.5));

				if(abs(knf.angle) < 1.04 && glm::cross((*plr.part.p+arm)-(*knf.part.p), glm::vec3(0.0, (knf.onRight ? 1.0f : -1.0f), 0.0)) != glm::vec3(0.0, 0.0, 0.0)){
					*knf.part.v += 45.0f*glm::normalize(glm::cross((*plr.part.p+arm)-(*knf.part.p), glm::vec3(0.0, (knf.onRight ? 1.0f : -1.0f), 0.0)));
				}
				/*if(abs(knf.angle) < 1.05) {
					knf.angle += 3.0*dt*knf.angle/abs(knf.angle);
				}
				else {
					knf.angle = 1.05*(knf.onRight ? 1.0 : -1.0);
				}*/
				knf.angle = 1.05*(knf.onRight ? 1.0 : -1.0);
			}

			(*knf.part.a) = 
				  165.0f*(((*plr.part.p)+arm)-(*knf.part.p))
				- 20.0f*glm::proj((*knf.part.v-*plr.part.v), ((*plr.part.p)+arm)-(*knf.part.p))
				- 10.0f*((*knf.part.v-*plr.part.v)-glm::proj((*knf.part.v-*plr.part.v), ((*plr.part.p)+arm)-(*knf.part.p)))
				;//+ 10.0f*(*plr.part.v);
		}
		else{
			(*knf.part.a) = glm::vec3(0.0, gravity, 0.0);
			if(knf.part.p->y > plr.part.p->y+arm.y-knf.part.p->y && knf.part.v->y > 0.0){//the knife has been caught
				knf.switching = false;
			}
		}

		knf.part = particleLoop(knf.part, dt);
		knf.part = ground(knf.part, -5.0);

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

	rocket createRocket(rocket rkt, glm::vec3 pos, glm::vec3 vel){
		rkt.exploded = false;
		rkt.explosionTimer = 0.5;
		*rkt.part.p = pos;
		*rkt.part.v = vel;
		return rkt;
	}

	rocket * rocketsLoop(rocket * rkts, world wrld, float dt){
		for(int i = 0; i < 100; i++){
			if(!rkts[i].exploded){
				//rkts[i].part.a->y = gravity;
				rkts[i].part = particleLoop(rkts[i].part, dt);
				float groundDist = 3.0;
				rkts[i].part = ground(rkts[i].part, -groundDist);
				auto yLess = glm::vec2(rkts[i].part.p->x, rkts[i].part.p->z);
				if(rkts[i].part.p->y >= groundDist || glm::dot(yLess, yLess) >= 1000.0*1000.0){
					rkts[i].exploded = true;
				}
				for(int e = 0; e < 100; e++){
					auto dist = *wrld.enemies[e].part.p - *rkts[i].part.p;
					if(glm::dot(dist, dist) <= pow(1.6, 2)){
						*wrld.enemies[e].part.v += *rkts[i].part.v;
						*wrld.enemies[e].health -= 20.0;
						rkts[i].exploded = true;
					}
				}
			}
			else{
				rkts[i].explosionTimer -= dt;
			}

			if(rkts[i].explosionTimer > 0.0){
				*rkts[i].att = lookQuat(glm::vec3(0.0), *rkts[i].part.v, glm::vec3(0.0, 1.0, 0.0));
				if(rkts[i].exploded){
					//screen shake
					auto dist = *rkts[i].part.p - *wrld.plr.part.p;
					auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0);
					if(randVector != glm::vec3(0.0)){
						*wrld.cam.part.v += 400.0f*dt*glm::normalize(randVector)/sqrt(glm::dot(dist, dist));
					}
					*rkts[i].modelId = 6;
				}
				else{
					*rkts[i].modelId = 4;
				}
			}
			else{
				*rkts[i].modelId = 0;
			}
		}
		return rkts;
	}

	rocketLauncher lawnChairLoop(rocketLauncher rL, world wrld, rocket * rkts, float dt, float phi, float theta){
		auto lookDir = glm::vec3(-sin(phi)*cos(theta), sin(theta), cos(phi)*cos(theta));
		if(input::pressed(VK_LBUTTON)){
			if(!rL.shot){
				for(int i = 0; i < 100; i++){
					if(rkts[i].explosionTimer < 0.0){
						auto relFactor = glm::proj(*rL.part.v, lookDir);
						if(glm::dot(lookDir, relFactor) < 0.0){
							relFactor = glm::vec3(0.0);
						}
						rkts[i] = createRocket(rkts[i], *rL.part.p, 80.0f*lookDir+relFactor);
						*rL.part.v -= 20.0f*lookDir+relFactor;
							
						auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0+glm::dot(*rL.part.v-*wrld.plr.part.v, lookDir)/sqrt(glm::dot(lookDir, lookDir)));
						if(randVector != glm::vec3(0.0)){
							*wrld.cam.part.v += 5.0f*glm::normalize(randVector);
						}
						break;
					}
				}
			}
			rL.shot = true;
		}
		else if(input::pressed(VK_RBUTTON)){
			if(!rL.shot){
				for(int i = 0; i < 100; i++){
					if(rkts[i].explosionTimer < 0.0){
						auto relFactor = glm::proj(*rL.part.v, -lookDir);
						if(glm::dot(-lookDir, relFactor) < 0.0){
							relFactor = glm::vec3(0.0);
						}
						rkts[i] = createRocket(rkts[i], *rL.part.p, 80.0f*-lookDir+relFactor);
						*rL.part.v -= -30.0f*lookDir+relFactor;
							
						auto randVector = glm::vec3((rand()%100-50)/100.0, (rand()%100-50)/100.0, (rand()%100-50)/100.0+glm::dot(*rL.part.v-*wrld.plr.part.v, lookDir)/sqrt(glm::dot(lookDir, lookDir)));
						*wrld.cam.part.v += 5.0f*glm::normalize(randVector);
						break;
					}
				}
			}
			rL.shot = true;
		}
		else{
			rL.shot = false;
		}
		//folow player
		float off = 1.57;
		auto arm = 1.0f*glm::vec3(-sin(phi)*cos(theta+off), sin(theta+off), cos(phi)*cos(theta+off));
		(*rL.part.a) = 
				  165.0f*(((*wrld.plr.part.p)+arm)-(*rL.part.p))
				- 20.0f*glm::proj((*rL.part.v), ((*wrld.plr.part.p)+arm)-(*rL.part.p))
				- 10.0f*((*rL.part.v)-glm::proj((*rL.part.v), ((*wrld.plr.part.p)+arm)-(*rL.part.p)))
				+ 10.0f*(*wrld.plr.part.v);
		
		rL.part = particleLoop(rL.part, dt);

		if(glm::dot(((*wrld.plr.part.p)+arm)-(*rL.part.p), ((*wrld.plr.part.p)+arm)-(*rL.part.p)) > pow(0.5, 2)){
			(*rL.part.p) = 0.5f*glm::normalize((*rL.part.p)-((*wrld.plr.part.p)+arm))+((*wrld.plr.part.p)+arm);
		}
		
		*rL.att = lookQuat(lookDir, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));

		return rL;
	}

	bool blowUp(particle p, float dt, rocket * rkts){
		bool blown = false;
		for(int i = 0; i < 100; i++){
			auto dist = *p.p - *rkts[i].part.p;
			if(rkts[i].exploded && rkts[i].explosionTimer > 0.0 && glm::dot(dist, dist) <= pow(7.0, 2) && dist != glm::vec3(0.0)){//TODO: make potential exponential
				(*p.a) += 360.0f*exp(-0.1f*sqrt(glm::dot(dist, dist)))*glm::normalize(dist);//40.0f*glm::normalize(dist);
				blown = true;
			}
		}
		return blown;
	}

	//TODO: ai!
	enemy * enemyLoop(world wrld, enemy * enemies, player plr, float dt, float phi, float theta){
		for(int i = 0; i < 100; i++){
			auto knifeDist = *wrld.knf.part.p - *enemies[i].part.p;
			
			float speed  = 20.0;

			if(*enemies[i].health > 0.0){
				if(*enemies[i].part.p != *plr.part.p){
					auto lookDir = glm::vec3(-sin(phi), 0.0, cos(phi));
					
					bool lookedAtMeFunny = glm::dot(glm::normalize(lookDir), glm::normalize(*enemies[i].part.p - *plr.part.p)) >= cos(0.1*i);// && glm::dot(*plr.part.p - *enemies[i].part.p, *plr.part.p - *enemies[i].part.p) <= pow(8.0, 2));
					*enemies[i].part.a = 1.0f*(speed*glm::normalize(*plr.part.p - *enemies[i].part.p) - *enemies[i].part.v);
					auto relKnfVel = *enemies[i].part.v - *wrld.knf.part.v;
					if(glm::dot(glm::normalize(relKnfVel), glm::normalize(knifeDist)) >= cos(0.1*i) && pow(glm::dot(knifeDist, knifeDist), 2)/glm::dot(relKnfVel, relKnfVel) < pow(10.0, 2)){// && lookedAtMeFunny){
						auto sideDir = glm::normalize(glm::cross(*wrld.plr.part.p - *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0)));
						*enemies[i].part.a = 2.0f*((glm::dot(knifeDist, sideDir) < 0.0 ? 1 : -1)*speed*sideDir - *enemies[i].part.v);
						*enemies[i].part.a -= 1.0f*(speed*glm::normalize(*wrld.knf.part.p - *enemies[i].part.p) - *enemies[i].part.v);//TODO: vertically challenged code
					}
					
					/*
					*enemies[i].part.a = 1.0f*(speed*glm::normalize(*plr.part.p - *enemies[i].part.p) - *enemies[i].part.v);
					bool lookedAtMeFunny = (glm::dot(glm::normalize(lookDir), glm::normalize(*enemies[i].part.p-*plr.part.p)) <= 0.005*i+0.5 && glm::dot(*plr.part.p - *enemies[i].part.p, *plr.part.p - *enemies[i].part.p) <= pow(8.0, 2));
					if(glm::dot(knifeDist, knifeDist) < pow(1.6+1.0, 2) || lookedAtMeFunny){//improve knife dodging
						*enemies[i].part.a = 2.0f*((i & 1) ? 1 : -1)*(speed*glm::normalize(glm::cross(*wrld.plr.part.p - *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0)) - *enemies[i].part.v));
						if(lookedAtMeFunny){
							*enemies[i].part.a -= 1.0f*(speed*glm::normalize(*plr.part.p - *enemies[i].part.p) - *enemies[i].part.v);
						}
					}
					*/

					glm::vec3 rockPos = glm::vec3(0.0);
					bool dodgeRocket = false;
					for(int r = 0; r < 100; r++){//eats frames //TODO:make list of living rockets at the start of the main loop or in the rocket loop, maybe a run length encoded list// or mabey just decrease the number of rockets
						auto rocketDist = *wrld.rkts[r].part.p - *enemies[i].part.p;
						if(rocketDist != glm::vec3(0.0) && *wrld.rkts[r].part.v != glm::vec3(0.0) && wrld.rkts[r].explosionTimer > 0.0 && glm::dot(glm::normalize(-rocketDist), glm::normalize(*wrld.rkts[r].part.v)) > cos(0.01*i)){
							dodgeRocket = true;
							if(glm::dot(rocketDist, rocketDist) < glm::dot(rockPos - *enemies[i].part.p, rockPos - *enemies[i].part.p)){
								rockPos = *wrld.rkts[r].part.p;
								break;
							}
						}
					}
					if(dodgeRocket){
						auto sideDir = glm::normalize(glm::cross(*wrld.plr.part.p - *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0)));
						*enemies[i].part.a = 2.0f*((glm::dot(rockPos - *enemies[i].part.p, sideDir) < 0.0 ? 1 : -1)*speed*sideDir - *enemies[i].part.v);
					}
				}

				if(glm::dot(knifeDist, knifeDist) < pow(1.6, 2) && knifeDist != glm::vec3(0.0)){
					//*enemies[i].health = 0.0;
					float dmg = sqrt(glm::dot(*wrld.knf.part.v, *wrld.knf.part.v));
				
					*enemies[i].part.p -= sqrt(pow(1.6f, 2.0f) - glm::dot(knifeDist, knifeDist)) * glm::normalize(knifeDist);
					*enemies[i].part.v +=
							log(1.0f+0.01f*glm::dot(*wrld.knf.part.v, *wrld.knf.part.v))
						* glm::proj(*wrld.knf.part.v, *wrld.knf.part.p - *enemies[i].part.p)
						- glm::proj(*enemies[i].part.v, *plr.part.p - *enemies[i].part.p);
					enemies[i].part.v->y = -0.5*dmg;

					*enemies[i].health -= 0.5*dmg;

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
					plr.part.v->y -= 10.0f*dt;//TODO: move to player?
				}

				enemies[i].part.a->y = 5.75;

				if(blowUp(enemies[i].part, dt, wrld.rkts)){
					*enemies[i].health -= 1.0*dt;
				}
				enemies[i].part = particleLoop(enemies[i].part, dt);
				enemies[i].part = ground(enemies[i].part, 0.0);
			
				*enemies[i].att = lookQuat(*plr.part.p, *enemies[i].part.p, glm::vec3(0.0, 1.0, 0.0));
			}
			else {
				*enemies[i].part.a = glm::vec3(0.0);

				if(enemies[i].part.p->y < 0.0){
					*enemies[i].att = lookQuat(*enemies[i].part.v, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
				}
				else{
					*enemies[i].part.a = -0.6f*(*enemies[i].part.v);
					if(*enemies[i].part.v != glm::vec3(0.0)){
						*enemies[i].att = lookQuat(glm::normalize(*enemies[i].part.v), glm::vec3(0.0, -10.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
					}
				}

				enemies[i].part.a->y = 5.75;

				if(glm::dot(*enemies[i].part.v, *enemies[i].part.v) >= 0.01) {//bugs under some framerates/accelerations
					blowUp(enemies[i].part, dt, wrld.rkts);
					enemies[i].part = particleLoop(enemies[i].part, dt);
				}
				enemies[i].part = ground(enemies[i].part, -4.6);
			}
		}

		return enemies;
	}

	//TODO: fix issues at lower framerates(including 60 Hz) #fixthejiggle
	world loop(world oldWorld, float dt){
		world newWorld = oldWorld;

		*newWorld.slowMoTimer -= dt;

		if(*newWorld.slowMoTimer >= 0.0){
			dt *= 0.2;
		};

		//particle * player = &newWorld.parts[0];

		float theta = input::mouse().y;
		float phi = input::mouse().x;

		//temp
		if(theta <= -3.14159265358979323846264338327950/2){
			theta = -3.14159265358979323846264338327950/2;
			input::moose.y = -3.14159265358979323846264338327950/2;
		}

		if(theta >= 3.14159265358979323846264338327950/2){
			theta = 3.14159265358979323846264338327950/2;
			input::moose.y = 3.14159265358979323846264338327950/2;
		}
		//\temp
	
		newWorld.plr = playerLoop(newWorld, newWorld.plr, dt, phi, theta);

		newWorld.cam = cameraLoop(newWorld.cam, *newWorld.plr.part.p, dt, phi, theta);

		if(input::pressed('1')){
			*newWorld.knf.part.p = *newWorld.plr.part.p+glm::vec3(0.0, 2.0, 0.0);//TODO: make the knife initializer
			newWorld.wpn = 0;
		}
		if(input::pressed('2')){
			*newWorld.rL.part.p = *newWorld.plr.part.p+glm::vec3(0.0, 0.0, 0.0);
			newWorld.wpn = 1;
		}

		if(newWorld.wpn == 0){
			newWorld.knf = knifeLoop(newWorld, newWorld.knf, newWorld.plr, dt, phi, theta);
		}
		else {
			*newWorld.knf.part.p = glm::vec3(0.0, 100.0, 0.0);
		}

		if(newWorld.wpn == 1){
			newWorld.rL = lawnChairLoop(newWorld.rL, newWorld, newWorld.rkts, dt, phi, theta);
		}
		else {
			*newWorld.rL.part.p = glm::vec3(0.0, 100.0, 0.0);
		}

		newWorld.enemies = enemyLoop(newWorld, newWorld.enemies, newWorld.plr, dt, phi, theta);

		newWorld.rkts = rocketsLoop(newWorld.rkts, newWorld, dt);

		return newWorld;
	}
}