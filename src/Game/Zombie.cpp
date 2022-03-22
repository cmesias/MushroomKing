/*
 * Zombie.cpp
 *
 *  Created on: May 31, 2017
 *      Author: Carl
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Zombie.h"

#include "../Particless.h"



// Clear all zombie
void Zombie::clear(Zombie zombie[]){
	count = 0;
	for (int i = 0; i < max; i++) {
		zombie[i].alive = false;
	}
}

void Zombie::Init(Zombie zombie[]){
	count = 0;
	asteroidsClear	= true;
	for (int i = 0; i < max; i++) {
		zombie[i].x 				= 0;
		zombie[i].y 				= 0;
		zombie[i].w 				= 0;
		zombie[i].h 				= 0;
		zombie[i].vX 				= 0.0;
		zombie[i].vY 				= 0.0;
		zombie[i].speed 			= 0.0;
		zombie[i].angle 			= 0.0;
		zombie[i].health 			= 100;
		zombie[i].damage 			= 5;
		zombie[i].distance 			= 1;
		zombie[i].alive 			= false;
		zombie[i].collision 		= false;
		zombie[i].onScreen 			= false;
		zombie[i].type 				= 0;
	}
}

void Zombie::Load(SDL_Renderer* gRenderer){
	gTexture.loadFromFile(gRenderer, "resource/gfx/player_clone.png");
	gCircle.loadFromFile(gRenderer, 	"resource/gfx/circle.png");

	// Other classes
	LoadFonts();
}

void Zombie::Free(){
	gTexture.free();
	gCircle.free();

	// Other classes
	FreeFonts();
}



// Create an zombie
void Zombie::spawn(Zombie zombie[], float x, float y, float w, float h, float angle, float speed, int type) {
	for (int i = 0; i < max; i++) {
		if (!zombie[i].alive) {
			zombie[i].x 				= x;
			zombie[i].y 				= y;
			zombie[i].w 				= w;
			zombie[i].h 				= h;
			zombie[i].radius 			= w/2;
			zombie[i].x2 				= x + w/2;
			zombie[i].y2 				= y + h/2;
			zombie[i].speed 			= speed;
			zombie[i].vX 				= cos( (3.14159265/180)*(angle) );
			zombie[i].vY 				= sin( (3.14159265/180)*(angle) );
			zombie[i].angle 			= 0.0;
			//------------------------------------------------------------------------//
			/* initial centering of zombie's circle collision shape */
			float radians 				= (3.1415926536/180)*(0.0);
			float Cos 					= floor(cos(radians)*10+0.5)/10;
			float Sin 					= floor(sin(radians)*10+0.5)/10;
			int newW 					= 11 * (-Cos);
			int newH 					= 11 * (-Sin);
			zombie[i].x2 				= x+w/2 + newW - zombie[i].radius;
			zombie[i].y2 				= y+h/2 + newH - zombie[i].radius;
			//------------------------------------------------------------------------//
			zombie[i].health			= 100;
			zombie[i].damage			= 5;
			zombie[i].type 				= type;
			zombie[i].distance 			= 1;
			zombie[i].collision 		= false;
			zombie[i].onScreen 			= false;
			zombie[i].alive 			= true;
			count++;
			break;
		}
	}
}
// Update zombie
void Zombie::update(Zombie zombie[], Particle particle[], Particle &p_dummy,
		Map &map, Players &player, Mix_Chunk* sLazer,
		int camx, int camy) {
	for (int i = 0; i < max; i++)
	{
		if (zombie[i].alive)
		{
			// zombie target
			float bmx2 = zombie[i].x+zombie[i].w/2;
			float bmy2 = zombie[i].y+zombie[i].h/2;
			float bmx  = player.x2+player.radius;
			float bmy  = player.y2+player.radius;
			// zombie distance from target
			zombie[i].distance = sqrt((bmx - bmx2) * (bmx - bmx2) + (bmy - bmy2) * (bmy - bmy2));
			if (zombie[i].distance <= 1) {
				zombie[i].distance = 1;
			}

			// zombie moves if target is less than 768 pixels and greater than 384 pixels away
			if (zombie[i].distance > 384 && zombie[i].distance < 880) {
				//zombie[i].vX = zombie[i].speed * (bmx - bmx2) / zombie[i].distance;
				//zombie[i].vY = zombie[i].speed * (bmy - bmy2) / zombie[i].distance;
				zombie[i].x += zombie[i].speed * (bmx - bmx2) / zombie[i].distance;
				zombie[i].y += zombie[i].speed * (bmy - bmy2) / zombie[i].distance;

				zombie[i].angle = atan2(bmy - bmy2,bmx - bmx2);
				zombie[i].angle = zombie[i].angle * (180 / 3.1416);

				if (zombie[i].angle < 0) {
					zombie[i].angle = 360 - (-zombie[i].angle);
				}
			}

			// zombie shoots if target is 768 pixels away
			if (zombie[i].distance <= 880) {
				zombie[i].angle = atan2(bmy - bmy2,bmx - bmx2);
				zombie[i].angle = zombie[i].angle * (180 / 3.1416);
				if (zombie[i].angle < 0) {
					zombie[i].angle = 360 - (-zombie[i].angle);
				}

				// calculate some things for shooting
				float radians = (3.1415926536/180)*(zombie[i].angle);
				float Cos = floor(cos(radians)*10+0.5)/10;
				float Sin = floor(sin(radians)*10+0.5)/10;

				// get player new center position
				int newW = 11 * (-Cos);
				int newH = 11 * (-Sin);
				zombie[i].x2 = zombie[i].x+zombie[i].w/2 + newW - zombie[i].radius;
				zombie[i].y2 = zombie[i].y+zombie[i].h/2 + newH - zombie[i].radius;

				double barrelW  = (57 * cos(radians) ) - (16 * sin(radians) );				// add this to center of zombie (this will give us the guns barrel position)
				double barrelH  = (57 * sin(radians) ) + (16 * cos(radians) );
				int realw = 128;															// zombie actual size in pixels
				int realh = 128;
				double particleW = 24;														// particle collision size
				double particleH = 24;
				int wDifference = realw - zombie[i].w;										// get difference between zombie collision size and actual size (used to center bullets)
				int hDifference = realh - zombie[i].h;
				double barrelX = zombie[i].x+realw/2-wDifference/2 - particleW/2 + barrelW;	// get final position to spawn a particle
				double barrelY = zombie[i].y+realh/2-hDifference/2 - particleH/2 + barrelH;

				// Enemy shooting
				zombie[i].timer += 1;
				if (zombie[i].timer > 15){
					//zombie[i].vX = Cos * 11 - rand() % 8 + 5;
					//zombie[i].vY = Sin * 11 - rand() % 8 + 5;
					// restart shoo ttimer
					zombie[i].timer = 0;
					// play shoot sound effect
					Mix_PlayChannel(3, sLazer, 0);
					// spawn particle
					p_dummy.spawnParticleAngle(particle, 1,
							barrelX,
							barrelY,
							particleW, particleH,
							zombie[i].angle, 11,
						   5,
						   {255, 255,0}, 1,
						   0.0, 0.0,
						   255, 0,
						   100, 1,
						   false, 0);
				}
			}

			//-----------------------------------------------------------------------------------//

			// zombie movement
			zombie[i].x += zombie[i].vX;
			zombie[i].y += zombie[i].vY;

			// velocity decay
			zombie[i].vX = zombie[i].vX - zombie[i].vX * 0.02;
			zombie[i].vY = zombie[i].vY - zombie[i].vY * 0.02;

			// zombie screen check
			if (zombie[i].x + zombie[i].w >= camx && zombie[i].x <= camx + screenWidth &&
				zombie[i].y + zombie[i].h >= camy && zombie[i].y <= camy + screenHeight) {
				zombie[i].onScreen = true;
			}else{
				zombie[i].onScreen = false;
			}

			// zombie death
			if (zombie[i].health <= 0)
			{
				player.score += 20;
				// spawn blood particle effect
				for (double h=0.0; h< 360.0; h+=rand() % 10 + 10){
					int rands = rand() % 9 + 2;
					float newX = zombie[i].x+zombie[i].w/2;
					float newY = zombie[i].y+zombie[i].h/2;
					p_dummy.spawnParticleAngle(particle, 2,
										newX-rands/2,
										newY-rands/2,
									   rands, rands,
									   h, randDouble(2.1, 5.1),
									   0.0,
									   {255, 0, 0, 255}, 1,
									   1, 1,
									   rand() % 100 + 150, rand() % 2 + 5,
									   rand() % 50 + 90, 0,
									   true, 0.11,
									   rand() % 9 + 2, 1);
				}
				zombie[i].alive = false;
				count--;
			}

			// zombie and Map collision
			/*if (zombie[i].x+zombie[i].w < map.x) {
				zombie[i].x = map.x+map.w;
			}
			if (zombie[i].x > map.x+map.w) {
				zombie[i].x = map.x-zombie[i].w;
			}
			if (zombie[i].y+zombie[i].h < map.y) {
				zombie[i].y = map.y+map.h;
			}
			if (zombie[i].y > map.y+map.h) {
				zombie[i].y = map.y-zombie[i].h;
			}*/

			// Camera level bounds
			if( zombie[i].x < 0 ){
				zombie[i].x = 0;
			}
			if( zombie[i].y < 0 ){
				zombie[i].y = 0;
			}
			if( zombie[i].x+zombie[i].w > map.w ){
				zombie[i].x = map.w-zombie[i].w;
			}
			if( zombie[i].y+zombie[i].h > map.h ){
				zombie[i].y = map.h-zombie[i].h ;
			}

			// zombie circle collision check with other zombies
			for (int j = 0; j < max; j++) {
				if (i !=j) {
					if (zombie[j].alive) {
						float bmx = zombie[j].x+zombie[j].w/2;
						float bmy = zombie[j].y+zombie[j].h/2;
						float bmx2 = zombie[i].x+zombie[i].w/2;
						float bmy2 = zombie[i].y+zombie[i].h/2;
						float angle = atan2(bmy - bmy2,bmx - bmx2);
						angle = angle * (180 / 3.1416);
						if (angle < 0) {
							angle = 360 - (-angle);
						}
						float radians = (3.1415926536/180)*(angle);
						float Cos = floor(cos(radians)*10+0.5)/10;
						float Sin = floor(sin(radians)*10+0.5)/10;
						float distance = sqrt((bmx - bmx2) * (bmx - bmx2)+
											  (bmy - bmy2) * (bmy - bmy2));
						if (distance <= 1) {
							distance = 1;
						}
						if (distance < zombie[i].w/2 + zombie[j].w/2) {
							zombie[i].x -= 1 * Cos;
							zombie[i].y -= 1 * Sin;
						}
					}
				}
			}
		}
	}
}

//Render zombie
void Zombie::Render(SDL_Renderer* gRenderer, Zombie zombie[], int camx, int camy) {
	for (int i = 0; i < max; i++) {
		if (zombie[i].alive) {
			int wDifference = 128 - zombie[i].w;
			int hDifference = 128 - zombie[i].h;
			gTexture.render(gRenderer, zombie[i].x-wDifference/2 - camx, zombie[i].y-hDifference/2 - camy,
							128,128, NULL, zombie[i].angle);


		}
	}
}

//Render zombie
void Zombie::RenderDebug(SDL_Renderer* gRenderer, Zombie zombie[], int camx, int camy)
{
	for (int i = 0; i < max; i++) {
		if (zombie[i].alive) {
			// Original box, untouched
			SDL_Rect tempRect = {zombie[i].x-camx, zombie[i].y-camy, zombie[i].w, zombie[i].h};
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &tempRect);
			// Box, centered
			tempRect = {zombie[i].x2-camx, zombie[i].y2-camy, zombie[i].w, zombie[i].h};
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &tempRect);

			// Render circle
			gCircle.setColor(255,255,255);
			gCircle.setAlpha(180);
			gCircle.render(gRenderer, zombie[i].x2-camx,
					zombie[i].y2-camy,
					zombie[i].w, zombie[i].h);

			// Render angle Text
			std::stringstream tempss;
			tempss << zombie[i].angle;
			gText.setAlpha(255);
			gText.loadFromRenderedText(gRenderer, tempss.str().c_str(), {255,255,255}, gFont20);
			//gText.render(gRenderer, zombie[i].x - camx, zombie[i].y-gText.getHeight() - camy, gText.getWidth(), gText.getHeight());

		}
	}
}



