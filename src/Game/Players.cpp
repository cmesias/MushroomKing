/*
 * Players.cpp
 *
 *  Created on: Dec 18, 2016
 *      Author: Carl
 */

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_mouse.h>

//#include "Input.h"

#include "Players.h"

// Check collision between 2 objects
bool Players::checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2){
	bool collide;
	if (x+w > x2 && x < x2 + w2 && y+h > y2 && y < y2 + h2){
		collide = true;
	}else{
		collide = false;
	}
	return collide;
}

// Load asteroid resources
void Players::Load(SDL_Renderer* gRenderer){

	// open joystick i
    joy = SDL_JoystickOpen(1);

    // load textures
	gPlayer.loadFromFile(gRenderer, "resource/gfx/player/player.png");
	gShield.loadFromFile(gRenderer, "img/shield.png");
	rPlayer[0] = {0,0,48,48};
	rPlayer[1] = {48,0,48,48};
	rPlayer[2] = {96,0,48,48};
	rPlayer[3] = {144,0,48,48};

	rPlayer[4] = {0,48,48,48};
	rPlayer[5] = {48,48,62,48};
	rPlayer[6] = {0,96,48,48};
	rPlayer[7] = {144,48,48,55};		// down attack
	for (int i=0; i<7; i++){setClips(rShield[i], i*48, 0, 48, 48);}

	// Load audio
	sCast 			= Mix_LoadWAV("sounds/snd_cast.wav");
	sSlash 			= Mix_LoadWAV("sounds/snd_slash.wav");
	sDownStab		= Mix_LoadWAV("sounds/snd_downStab.wav");
	sParry			= Mix_LoadWAV("sounds/snd_parry.wav");

	// Other classes
	LoadFonts();
}

// Free asteroid resources
void Players::Free(){
	// Free joystick
    SDL_JoystickClose(joy);

    // Free textures
	gPlayer.free();
    gShield.free();

    // Free audio
	Mix_FreeChunk(sCast);
	Mix_FreeChunk(sSlash);
	Mix_FreeChunk(sDownStab);
	Mix_FreeChunk(sParry);
	sCast 			= NULL;
	sSlash 			= NULL;
	sDownStab		= NULL;
	sParry			= NULL;

	// Other classes
	FreeFonts();
}

// Reset game
void Players::Init(float spawnX, float spawnY, std::string newName, bool respawn){
	x 					= spawnX;
	y 					= spawnY;
	name				= newName;
	accuracy			= 0.0;
	hits				= 0.0;
	totalShot			= 0.0;
	vX 					= 0.0;
	vY 					= 0.0;
	delayT 				= 0;
	health				= 100;
	delay 				= false;
	initialshot 		= false;
	thrust 				= false;
	deathScreen 		= false;
	alive 				= true;
	returned			= false;
	indexSaved 			= -1;
	invurnerableFrame	= 1;
	invurnerableTick	= 0;
	invurnerableT		= 300;
	invurnerable		= true;

	controls 			= 0;
	A 					= false;
	B 					= false;
	X 					= false;
	Y 					= false;
	D_UP 				= false;
	D_DOWN 				= false;
	D_LEFT 				= false;
	D_RIGHT 			= false;
	LB 					= false;
	RB 					= false;
	BACK 				= false;
	START 				= false;
	L3 					= false;
	R3 					= false;
	LAngle 				= 0.0;
	RAngle 				= 0.0;
	zAxisLeft 			= 0.0;
	zAxisRight 			= 0.0;
	leftclick 			= false;
	rightclick 			= false;
	test 				= false;
	trigger 			= false;
	moving				= false;
	renderFlash 		= false;
	playSFXDownStabHittingFloor	= true;
	facing				= "right";
	jumpstate			= "falling";
	flipW				= SDL_FLIP_NONE;
	grav 				= 0.6;
	jumps				= 1;
	walkTimer 			= 0;
	attackTimer 		= 0;
	attackFrame 		= 0;
	sprite_index 		= 0;
	jump 				= false;
	attack 				= false;
	attackType			= -1;
	stunned 			= false;
	stunTimer 			= 0;
	parry 				= false;
	playSlash 			= false;
	clash 				= false;
	attack 				= false;
	spawnAttack 		= false;
	manaRegenTimer		= 0;

	if (!respawn){
		score 			= 0;
		wave 			= 0;
		increment	 	= 35;
		lives 			= 1;
	}

	// Always reset these
	parryTimer 			= 0;
	parryCDTimer 		= 0;

	// TODO (3-9-2022) [ ] - Save these player stats in a file. This will be the players save data.
	this->AttackSpeed 			= 6.87;
	this->maxMana				= 100;
	this->mana					= this->maxMana;
	this->manaRegenSpeed		= 19.87;
	this->damage				= 25;
	this->damageMultipler		= 1;
	this->castDamage			= 13;
	this->knockBackPower		= 1.58;
	this->parryCDMax			= 60*3;

}

// Player shoot
void Players::fire(Particle particle[], Particle &p_dummy, Mix_Chunk* sCastSFX, int mx, int my){

	// Determine controls
	if (controls==0){
		trigger = initialshot;
	}else if(controls==1){
		trigger = A;
	}else if(controls==2){
		trigger = initialshot;
	}

	// Get shoot trigger
	if (controls==0){
		trigger = initialshot;
	}else if(controls==1){
		trigger = A;
		A = false;
	}else if(controls==2){
		trigger = initialshot;
		initialshot = false;
	}


	// Get player angle based on mouse coordinates
	angle = atan2(my - y-h/2,mx - x-w/2);
	angle = angle * (180 / 3.1416);
	//Set player angle max limits
	if (angle < 0) {
		angle = 360 - (-angle);
	}

	// Get radiant from angle
	radians = (3.1415926536/180)*(angle);
	radianCos = floor(cos(radians)*10+0.5)/10;
	radianSin = floor(sin(radians)*10+0.5)/10;

	// get player new center position
	//x2 = (x+w/2 + (41 * (-radianCos)) - radius);
	//y2 = (y+h/2 + (41 * (-radianSin)) - radius);
	/////int newW = 41 * (-radianCos);
	//////int newH = 41 * (-radianSin);
	x2 = x+w/2;
	y2 = y+h/2;

	/* If the object is at a rotation of 0.0 (facing right),
	 * then this will be the distance the particle
	 * will spawn from the center of the object,
	 *
	 * This will provide the correct distance from the center
	 * when the object is rotated from its center.
	 *
	 * With this distance you may add this to your objects x and y center,
	 * and this will be where your particle will spawn
	 */
	barrelW  = (bulletW * cos(radians) ) - (bulletH * sin(radians) );	// add this to center of zombie (this will give us the guns barrel position)
	barrelH  = (bulletW * sin(radians) ) + (bulletH * cos(radians) );

	int wDifference = realw - w;
	int hDifference = realh - h;

	barrelX = x+realw/2-wDifference/2 - particleW/2 + barrelW;
	barrelY = y+realh/2-hDifference/2 - particleH/2 + barrelH;

	/*int newmx = mx-particleW/2;
	int newmy = my-particleH/2;
	float distance = sqrt((newmx - barrelX) * (newmx - barrelX)+
						  (newmy - barrelY) * (newmy - barrelY));
	if (distance <= 1) {
		distance = 1;
	}
	float newAngle = atan2(newmy - barrelY,newmx - barrelX);
	newAngle = newAngle * (180 / 3.1416);
	//Set player angle max limits
	if (newAngle < 0) {
		newAngle = 360 - (-newAngle);
	}*/

	// Shoot particle
	if (trigger)
	{
		// If we have enough mana
		if (this->mana >= 25) {
			if (!delay)
			{
				delay = true;

				if (powerup == 1)
				{
					// recoil of gun
					recoilX = 11 * radianCos;
					recoilY = 11 * radianSin;

					// count up total shots
					totalShot++;

					// play audio
					Mix_PlayChannel(1, sCastSFX, 0);

					// spawn particle
					float tempParticleW = 20;
					float tempParticleH = 20;

					// Which way player is facing
					int angleToGo = 0;
					if (facing == "right") {
						angleToGo = 3.0;
					} else {
						angleToGo = 177.0;
					}

					// Cast spell
					// Spawn particle damage
					p_dummy.spawnParticleAngle(particle, 0,
							x+w/2,
							y+h/2 - 10,
							tempParticleW, tempParticleH,
							angleToGo, 15,
						   this->castDamage,					// particle spawning damage
						   {255, 255,0}, 1,
						   1, 1,
						   255, 0,
						   100, 10,
						   false, 0);

					// Subtract mana
					this->mana -= 25;

					// depracated
					/*// spawn particle
					p_dummy.spawnParticleAngle(particle, 0,
							barrelX,
							barrelY,
							particleW, particleH,
						   angle, 65,
						   25,
						   {255, 255,0}, 1,
						   1, 1,
						   255, 0,
						   100, 2,
						   false, 0);*/

					// muzzle flash
					renderFlash = true;
				}
			}
		}
	}

	// Shoot delay
	if (delay) {
		delayT += AttackSpeed;
		if (delayT > 60) {
			delay = false;
			delayT = 0;
		}
	}

	/*bool trigger = false;
	// Player shoot
	if (controls==0){
		trigger = initialshot;
	}else if(controls==1){
		trigger = A;
	}*/

	//if (test){
		//if (!delay) {
			//if (A) {
				//A = false;
				//trigger = false;
				//delay = true;

				// Remove shield
				/*if (shield){
					shield 	= false;
					shieldT 	= 300;
				}*/

				// Shoot particle
				/*if (powerup == "LAZER") {
					totalShot++;
					Mix_PlayChannel(-1, sLazer, 0);

					p_dummy.spawnParticleAngle(particle,
							   x+w/2-4/2+(cos(radians)*9)+(sin(radians)*12),
							   y+h/2-4/2+(sin(radians)*9)-(cos(radians)*12),
							   4, 4,
							   angle, 21,
							   255, 0,
							   100, 2,
							   100, "LAZER",
							   {255, 255,0}, 1,
							   1, 1,
							   false, 0);
					p_dummy.spawnParticleAngle(particle,
							   x+w/2-4/2+(cos(radians)*9)-(sin(radians)*12),
							   y+h/2-4/2+(sin(radians)*9)+(cos(radians)*12),
							   4, 4,
							   angle, 21,
							   255, 0,
							   100, 2,
							   100, "LAZER",
							   {255, 255,0}, 1,
							   1, 1,
							   false, 0);
				}*/
			//}
		/*}
		//Start delay timer after our first shot!
		if (delay) {
			delayT += AttackSpeed;
			if (delayT > 60) {
				delay = false;
				delayT = 0;
			}
		}*/
	//}

	// Handle gun recoil
	if (recoilX < 0) {
		recoilX += 2.5;
	}
	if (recoilX > 0) {
		recoilX -= 2.5;
	}
	if (recoilY < 0) {
		recoilY += 2.5;
	}
	if (recoilY > 0) {
		recoilY -= 2.5;
	}

}

// Update Player
void Players::update(Map &map,
					Asteroid asteroid[], Asteroid &a_dummy,
					Enemy enemy[], Enemy &e_dummy,
					Particle particle[], Particle &p_dummy,
					Tile &tl, Tile tile[],
					Tilec &tlc, Tilec tilec[],
					Object &obj, Object object[],
					int mx, int my, int camx, int camy,
					float spawnX, float spawnY,
					LWindow gWindow, SDL_Renderer* gRenderer,
					LTexture gText, TTF_Font *gFont, SDL_Color color,
					Mix_Chunk *sAtariBoom, Mix_Chunk* sManySFX[10],
					int LevelWeLoaded)
{
	// Reset upon leaving pause menu
	if (returned){
		returned 		= false;
		leftclick 	= false;
		initialshot 	= false;
		thrust		= false;
		A				= false;
		RB			= false;
	}

	// Player alive
	if (alive)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		//----------------------------------- Player Move -------------------------------------------------//
		//-------------------------------------------------------------------------------------------------//
		bool trigger = false;
		// Get Angle
		if (this->controls==0){
			trigger = this->thrust;
		}else if(this->controls==1){
			this->angle = this->LAngle;
			trigger = this->RB;
		}else if(this->controls==2){
			trigger = this->thrust;
		}

		// Player angle by rotation
		if (this->rotateLeft){
			//angle -= 5;
		}else if (this->rotateRight){
			//angle += 5;
		}

		// Move left
		if ((this->moveleft && !this->attack && !parry) || (this->moveleft && this->jumpstate == "jump")) {
			this->vX -= 1.0;
			this->moving = true;
	        if (!this->shift) {
	        	this->facing = "left";
	        }
		}
		// Move right
		if ((this->moveright && !this->attack && !this->parry) || (this->moveright && this->jumpstate == "jump")) {
			this->vX += 1.0;
			this->moving = true;
	        if (!this->shift) {
	        	this->facing = "right";
	        }
		}

		// Velocity limit
		if (this->vX > 5) {
			this->vX = 5;
		}
		if (this->vX < -5) {
			this->vX = -5;
		}

		// Jumping
		if (this->jump) {
			this->vY -= 3;
			if (this->vY < -6) {
				this->vY -= 6;
			}

			if (this->vY < -12) {
				this->jump = false;
			}
		}else{

		}

		//------------------------------------- NEW -------------------------------------//
		// Player Velocity X Axis
		this->x += this->vX;

		// Update collision with Tiles
		tl.checkCollisionX(tile, this->x, this->y, this->w, this->h, this->jumpstate, this->vX, this->jumps);

		// Player Velocity Y Axis
		this->y += this->vY;

		// Update collision with Tiles
		tl.checkCollisionY(tile, this->x, this->y, this->w, this->h, this->jumpstate, this->vY, this->jumps, this->jump);

	    // Gravity
		this->vY += this->grav;
		//------------------------------------- NEW -------------------------------------//

	    // Location of sword when down-stabbing
		this->swordX = this->x;
		this->swordY = this->y+35;

		//------------------------------------- OLD (2-25-2022) -------------------------------------//
		// Player Velocity X Axis
		//x += vX;

		// Update collision with Tiles
		//tl.checkCollisionX(tile, x, y, w, h, jumpstate, vX, jumps);

		// Player Velocity Y Axis
		///y += vY;

		// Update collision with Tiles
		//tl.checkCollisionY(tile, x, y, w, h, jumpstate, vY, jumps, jump);

	    // Gravity
	    //vY += grav;
		//------------------------------------- OLD (2-25-2022) -------------------------------------//

	    // comment this in to enable invisible ground for player
	    /* if (y+h > 500) {
	        vY = 0.0;
	        jumpstate = "ground";
	        jumps = 1;
	    }*/

		// Rustle timer
		// player left and right swaying
		if (moving) {
			rustleW += 1 * rustleDirX;
			rustleH += 1 * rustleDirY;
		}
		if (rustleW > 4) {
			rustleDirX = rustleDirX * -1;
		}
		if (rustleW < -4) {
			rustleDirX = rustleDirX * -1;
		}
		if (rustleH > 4) {
			rustleDirY = rustleDirY * -1;
		}
		if (rustleH < -4) {
			rustleDirY = rustleDirY * -1;
		}

		// Player not moving
		if (!moveleft && !moveright) {
			moving = false;
	        vX = vX - vX * 0.2;
		}

	    // Idle animation
	    if (!moving) {
	        if (jumpstate=="jump"){
	            sprite_index = 1;
	        }else{
	            sprite_index = 0;
	        }
	    // Moving animation
	    }else{
	    	// Jump animation (its just 1 frame)
	        if (jumpstate=="jump"){
	            sprite_index = 1;
	        }
	        // Walk animation
	        else{
	        	// Increment animation timer
	            walkTimer += 10;
	            // Increment current animation frame
	            if (walkTimer > 60) {
	            	// Reset timer
	            	walkTimer = 0;
	            	// Go to next animation frame
	            	sprite_index++;
	            }
	            if (sprite_index > 3) {
	            	sprite_index = 0;
	            }
	        }
	    }

		// Facing direction for flip
	    if (facing == "left") {
	        flipW = SDL_FLIP_HORIZONTAL;
	    }
	    if (facing == "right") {
	        flipW = SDL_FLIP_NONE;
	    }

	    // Attack animation
	    if (attack) {
        	// disable moving if not in the air
	    	if (jumpstate != "jump") {
	        	vX = 0;
	    	}
	    	// Handle slash Attack
	    	if (attackType == 0)
	    	{
		        // Attack timer
		        attackTimer++;

		        // If attack timer below 15 frames
		        if (attackTimer < 15)
		        {
	                // Set pre-slashing sprite
		            sprite_index = 4;
		        }

		        // At frame 16, spawn attack collision
		        else{

		        	// If are spawning an attack-object
		            if (spawnAttack) {

		            	// Immediatly stop attacking
		                spawnAttack = false;

		                // If facing right
		                int width;
		                if (facing == "right") {

		                	// Set attack object's x pos to the right of player's body
			                width = 38;
		                }else{

		                	// Set attack object's x pos to the left of player's body
			                width = -21;
		                }
		                // Attack-object's width and height
		                int tempWidth = 38;
		                int tempHeight = 64;

		                // Spawn attack object (it will appear in the world for 1 frame then remove itself)
		                obj.spawn(object, x+w/2-38/2+width,
		                				  y-16,
										  tempWidth, tempHeight,
										  0);
		            }
		            // Play slash sound effect
		            if (playSlash) {
		            	playSlash = false;
		                Mix_PlayChannel(-1, sSlash, 0);
		            }
	                // Set slashing sprite
		            sprite_index = 5;
		        }
		        // Attack over
		        if (attackTimer > 20) {
		        	// Reset attack-type
		        	attackType = -1;
		            attackTimer = 0;
		            attack = false;
		        }
	    	}

	    	// Handle Down-Stab attack
	    	else if (attackType == 1)
	    	{
                // Set animation until Player hits the ground
	            sprite_index = 7;

	        	// If are spawning an attack-object
	            if (spawnAttack) {
	            	spawnAttack = false;

		            // Spawn down-stab object
	                obj.spawnFollow(object, x, y, 28, 20, 1, &swordX, &swordY);
	            }

                // If player is on the ground
	            if (jumpstate=="ground") {
	            	//attack = false;

	                // If we can play the sound effect
	                if (playSFXDownStabHittingFloor) {
	                	// Stop playing sound effect
	                	playSFXDownStabHittingFloor = false;

	                	// Player sound effect
	                	//Mix_PlayChannel(-1, sDownStab, 0);
	                }

			        // Attack timer
			        attackTimer++;

			        // After 0.25 second or 15 frames
		        	// Attack over
			        if (attackTimer > 15)
			        {
			        	// Reset attack-type
			        	attackType = -1;

			        	// Enable play sound effect next time we down stab
	                	playSFXDownStabHittingFloor = true;

	                	// Stop attack
			            attackTimer = 0;
			            attack = false;
			        }
	            }
	    	}
	    }

	    // Parrying animation
	    if (this->parry) {
        	// disable moving if not in the air
	    	if (this->jumpstate != "jump") {
	    		this->vX = 0;
	    	}
	        // Determine direction
	        if (this->facing == "left") {
	        	this->sprite_index = 6;
	        }
	        if (this->facing == "right") {
	        	this->sprite_index = 6;
	        }

	        // Start Parrying timer
	        this->parryTimer++;

	        // Parry for 15 frames
	        if (this->parryTimer > 15){
	        	this->StopParrying();
	        }
	    // Parry cool-down
	    }else{
	        if (this->parryCDTimer > 0) {
	        	this->parryCDTimer -= 1;
	        }
	    }

	    // Stunned
	    if (this->stunned) {
	        if (this->facing == "left") {
	        	this->sprite_index = 3;
	        }
	        if (facing == "right") {
	        	this->sprite_index = 3;
	        }
	        this->stunTimer += 1;
	        if (this->stunTimer > 90) {
	        	this->stunTimer = 0;
	            this->stunned = false;
	        }
	    }
		//----------------------------------- Player Move -------------------------------------------------//
		//-------------------------------------------------------------------------------------------------//
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// Player shoot
		fire(particle, p_dummy, sCast, mx+camx, my+camy);

		// Player shield
		if (invurnerable){
			invurnerableT-=1;
			invurnerableTick += 20;
			if (invurnerableTick>60){
				invurnerableTick = 0;
				invurnerableFrame += 1;
			}
			if (invurnerableFrame>7){
				invurnerableFrame = 1;
				invurnerableTick = 0;
			}
			if (invurnerableT<=0){
				invurnerableT= 300;
				invurnerable = false;
			}
		}

		// Check high score MAX
		if (score > 999999999){
			score = 999999999;
		}

		// Player death
		if (health <=0)
		{
			// Save high score
			SaveHighScore(LevelWeLoaded);

			//Spawn explosion after asteroid death
			// spawn blood particle effect
			for (double i=0.0; i< 360.0; i+=rand() % 10 + 10){
				int rands = rand() % 9 + 2;
				float newX = x+w/2;
				float newY = y+h/2;
				p_dummy.spawnParticleAngle(particle, 2,
									newX-rands/2,
									newY-rands/2,
								   rands, rands,
								   i, randDouble(2.1, 5.1),
								   0.0,
								   {255, 0, 0, 255}, 1,
								   1, 1,
								   rand() % 100 + 150, rand() % 2 + 5,
								   rand() % 50 + 90, 0,
								   true, 0.11,
								   rand() % 9 + 2, 1);
			}

			// Take away lives
			lives-= 1;

			// Reset Player
			Init(spawnX, spawnY, name, true);

			// Player ran out of lives, que Death Screen
			if (lives<=0){

				// Reset some accumulated stuff
				e_dummy.ENEMY_SPAWN = 1;
				e_dummy.mileScore	= 0;
				e_dummy.SPAWN_ENEMY	= false;

				// Set variables
				invurnerableT 	= 300;
				invurnerable 	= true;
				alive 			= false;
				deathScreen 	= true;

				// SAVE HIGH SCORE
			}
		}

		// Update Player score and Wave
		if (this->score > this->highscore){
			this->highscore = this->score;
		}
		/*
		if (wave > HW){
			HW 	= wave;
		}*/

		// Player level boundaries
		if( this->x < map.x ){
			this->x = map.x;
		}
		if( this->y < map.y ){
			this->vY = 0.0;
			this->y = map.y;
		}
		if( this->x+this->realw > map.w ){
			this->x = map.w-this->realw;
		}
		if( this->y+realh > map.h ){
			this->vY = 0.0;
			this->y = map.y+map.h - this->realh;
			this->jumpstate = "ground";
			this->jumps = 1;
		}

		// Every frame player regenerates mana
		if (this->mana < this->maxMana)
		{
			// Start timer
			this->manaRegenTimer += 1;

			// After 1 second or 60 frames
			if (this->manaRegenTimer > 60) {

				// Reset timer
				this->manaRegenTimer = 0;

				// Increase mana based on mana regen speed
				this->mana += this->manaRegenSpeed;
			}
		}

		// Check if mana spilling over max
		if (this->mana > this->maxMana)
		{
			this->mana = this->maxMana;
		}

		// Every frame reduce damage multiplier by 2%
		if (this->damageMultipler > 1.0025) {
			this->damageMultipler -= 0.0025;
		}


	}else{
		// High-Score moving
		this->travel += 0.05 * this->dir;
		if (this->travel > 10){
			this->dir = -1;
		}
		if (this->travel < -10){
			this->dir = 1;
		}

		// Continue YES or NO Screen
		if (this->deathScreen)
		{
			SDL_ShowCursor(true);
			// Set button position
			continueButton[0] = {0 + screenWidth/2 -96/2-100, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[1] = {0 + screenWidth/2 -96/2+100, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[2] = {0 + screenWidth/2 -256/2 , screenHeight-gText.getHeight()-72, 256, 39};

			// High Score display position
			position  = 0+travel;
			position2 = continueButton[1].x+continueButton[1].w-gText.getWidth()+travel;

			// Left click
			if (leftclick)
			{
				// Option: Yes, reset everything
				if (checkCollision(mx, my, 1, 1, continueButton[0].x, continueButton[0].y, continueButton[0].w, continueButton[0].h))
				{
					leftclick			= false;

					// Reset Player
					std::string newName;
					newName="AAA";
					/////////input.getInput(gameLoop, quit, newName, gWindow, gRenderer);
					Init(spawnX, spawnY, newName, false);

					// Clear Asteroids & Enemies
					a_dummy.clear(asteroid);
					e_dummy.clear(enemy);
					SDL_ShowCursor(false);
				}

				// Option: No, go to Main Menu
				if (checkCollision(mx, my, 1, 1, continueButton[1].x, continueButton[1].y, continueButton[1].w, continueButton[1].h))
				{
					score = 0;
					leftclick  = false;
					deathScreen = false;
				}

				// Option: Reset high scores
				if (checkCollision(mx, my, 1, 1, continueButton[2].x, continueButton[2].y, continueButton[2].w, continueButton[2].h))
				{
					// Reset high scores
					ResetHighScore(LevelWeLoaded);
				}
			}
		// Menu Screen
		}else{
			SDL_ShowCursor(true);
			// Set button position
			continueButton[0] = {screenWidth/2-96/2, screenHeight/2-gText.getHeight()/2, 96, 33};
			continueButton[2] = {screenWidth/2-256/2 , screenHeight-gText.getHeight()-72, 256, 39};

			// High Score display position
			position 	= continueButton[0].w/2-gText.getWidth()/2-100+travel;
			position2 = continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2+100+travel;

			// Left click
			if (leftclick)
			{
				// Option: Play
				if (checkCollision(mx, my, 1, 1, continueButton[0].x, continueButton[0].y, continueButton[0].w, continueButton[0].h))
				{
					// Reset Player
					std::string newName;
					newName="AAA";
					///////////input.getInput(gameLoop, quit, newName, gWindow, gRenderer);
					Init(spawnX, spawnY, newName, false);

					// Clear Asteroids & Enemies
					a_dummy.clear(asteroid);
					e_dummy.clear(enemy);
					SDL_ShowCursor(false);
				}

				// Option: Reset high scores
				if (checkCollision(mx, my, 1, 1, continueButton[2].x, continueButton[2].y, continueButton[2].w, continueButton[2].h))
				{
					// Reset high scores
					ResetHighScore(LevelWeLoaded);
				}
			}
		}
	}
}

// Render Player
void Players::Render(int mx, int my, int camx, int camy, LWindow gWindow, SDL_Renderer* gRenderer,
					 SDL_Color color, int &PARTICLES, LTexture gText) {
	gText.setAlpha(255);
	// If alive
	if (alive){

		/* Get difference of real size and collision size,
		 * with this you can center the player by subtracting
		 * this with the player's position
		 */
		int wDifference = realw - w;				// get difference between player's collision size and actual size in pixels
		int hDifference = realh - h;
		int newX = x-wDifference/2;					// player starting position
		int newY = y-hDifference/2;
		int newRustleW = rustleW * radianCos;		// make swaying motions while moving
		int newRustleH = rustleH * radianSin;

		// render player
		//gPlayer.render(gRenderer, newX-recoilX+newRustleW-camx, newY-recoilY+newRustleH-camy, realw, realh, &rPlayer[sprite_index], 0.0);
		// Adjust player sprite if on attack animation
		// The '-9", in the y coordinate is adjusting the sprite to the appropriiate position
		if (sprite_index == 5) {
			if (facing == "right") {
				gPlayer.render(gRenderer, x-camx, y-camy, 62, 48, &rPlayer[sprite_index], 0.0, NULL, flipW);
			}else{
				gPlayer.render(gRenderer, x-18-camx, y-camy, 62, 48, &rPlayer[sprite_index], 0.0, NULL, flipW);
			}
		}else if (sprite_index == 7) {
			if (facing == "right") {
				gPlayer.render(gRenderer, x-camx, y-camy, 48, 55, &rPlayer[sprite_index], 0.0, NULL, flipW);
			}else{
				gPlayer.render(gRenderer, x-camx, y-camy, 48, 55, &rPlayer[sprite_index], 0.0, NULL, flipW);
			}
		}else{
			gPlayer.render(gRenderer, x-12-camx, y-camy, realw, realh, &rPlayer[sprite_index], 0.0, NULL, flipW);
		}

		// muzzle flash
		if (renderFlash) {
			renderFlash = false;
			//double barrelW  = (((bulletW+45) * cos(radians) ) - (bulletH * sin(radians) )) - 120/2;
			//double barrelH  = (((bulletW+45) * sin(radians) ) + (bulletH * cos(radians) )) - 120/2;
			// render muzzle flash

		}


		/*SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 255);
		SDL_RenderDrawLine(gRenderer, x+w/2 + barrelW-camx,
									  y+h/2 + barrelH-camy,
									  mx, my);*/

		// Player sight to mouse
		//SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
		//SDL_RenderDrawLine(gRenderer, x+w/2-camx, y+h/2-camy, mx, my);

		// Render Shield
		if (invurnerable){

			gShield.render(gRenderer, x+w/2 - 96/2 - camx, y+h/2 - 96/2 + 2 - camy, 96, 96, &rShield[invurnerableFrame], 180);

			/*SDL_Rect tempRect = {X-camx, Y-16-camy, (w*shieldT)/300, 3};
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
			SDL_RenderFillRect(gRenderer, &tempRect);*/
		}

		// Render number of lives left
		/*for (int i=0; i<lives; i++){
			//gPlayer.render(gRenderer, screenWidth-32-i*16, 90, 16, 16);
			spr_player_head.render(gRenderer, screenWidth/2+i*16, 72, 16, 16);
		}*/


		// reticle
		//double wedth = 21 * radianSin;
		//double hedth = 19 * radianCos;
		//SDL_SetRenderDrawColor(gRenderer, 255, 255, 0, 255);
		//SDL_RenderDrawLine(gRenderer, mx-16-wedth, my+hedth, mx+16-wedth, my+hedth);
		//SDL_RenderDrawLine(gRenderer, mx-wedth, my-16+hedth, mx-wedth, my+16+hedth);

		/*SDL_Rect tempRect = {mx-24-wedth, my-24+hedth, 48, 48};
		SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);*/
	}else{
		// Continue YES or NO Screen
		if (deathScreen)
		{
			// Render Text
			gText.loadFromRenderedText(gRenderer, "You have died. Continue?", color, gFont24);
			gText.render(gRenderer, screenWidth/2-gText.getWidth()/2, screenHeight/2-gText.getHeight()/2-50, gText.getWidth(), gText.getHeight());

			// Render buttons: Yes
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[0]);

			// Render buttons: No
			SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[1]);

			// Render buttons: ResetHighScore
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[2]);

			// Render button texts: Yes or No
			gText.loadFromRenderedText(gRenderer, "Yes", color, gFont24);
			gText.render(gRenderer,  continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2,
									 continueButton[0].y+continueButton[0].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			gText.loadFromRenderedText(gRenderer, "No", color, gFont24);
			gText.render(gRenderer,  continueButton[1].x+continueButton[1].w/2-gText.getWidth()/2,
									 continueButton[1].y+continueButton[1].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			// Render Text
			gText.loadFromRenderedText(gRenderer, "Reset High Scores", color, gFont24);
			gText.render(gRenderer, continueButton[2].x+continueButton[2].w/2-gText.getWidth()/2,
									 continueButton[2].y+continueButton[2].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());
		// Player Menu screen
		}else{

			// Render buttons: Play
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[0]);

			// Render buttons: ResetHighScore
			SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
			SDL_RenderDrawRect(gRenderer, &continueButton[2]);

			// Render Text
			gText.loadFromRenderedText(gRenderer, "PLAY", color, gFont24);
			gText.render(gRenderer, continueButton[0].x+continueButton[0].w/2-gText.getWidth()/2,
									 continueButton[0].y+continueButton[0].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());

			// Render Text
			gText.loadFromRenderedText(gRenderer, "Reset High Scores", color, gFont24);
			gText.render(gRenderer, continueButton[2].x+continueButton[2].w/2-gText.getWidth()/2,
									 continueButton[2].y+continueButton[2].h/2-gText.getHeight()/2,
									 gText.getWidth(), gText.getHeight());
		}

		// Render High Score text
		for (int i=0; i<10; i++){
			std::stringstream tempString(highList[i].c_str());
			std::string line;
			while (getline(tempString, line)) {
				std::stringstream iss(line);
				std::string temps[2];
				iss >> temps[0] >> temps[1];

				// Show Player where they are ranked
				if (indexSaved==i){
					gText.loadFromRenderedText(gRenderer, temps[0].c_str(), {244,144,20}, gFont20);
					gText.setAlpha(255-i*10);
					gText.render(gRenderer, continueButton[0].x+position,
							continueButton[0].y+continueButton[0].h+20+i*14,
							gText.getWidth(), gText.getHeight());
				}else{
					gText.loadFromRenderedText(gRenderer, temps[0].c_str(), color, gFont20);
					gText.setAlpha(255-i*10);
					gText.render(gRenderer, continueButton[0].x+position,
							continueButton[0].y+continueButton[0].h+20+i*14,
							gText.getWidth(), gText.getHeight());
				}

				gText.loadFromRenderedText(gRenderer, temps[1].c_str(), color, gFont20);
				gText.setAlpha(255-i*10);
				gText.render(gRenderer, position2,
						continueButton[1].y+continueButton[1].h+20+i*14,
						gText.getWidth(), gText.getHeight());
			}
		}
	}
}

void Players::RenderUI(SDL_Renderer *gRenderer, int camX, int camY)
{
	// Player UI
	{
		// Health bar
		int uiX = screenWidth * 0.95 - 100 - 10;
		int uiY = screenHeight * 0.96 - 20 - 10 - 24 - 6;

		// Render health, bg
		int manaBarWidth = 150;
		SDL_Rect tempRect = {uiX, uiY, (manaBarWidth*this->healthMax)/this->healthMax, 24};
		SDL_SetRenderDrawColor(gRenderer, 60, 60, 60, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render health
		tempRect = {uiX + 1, uiY + 1, ((manaBarWidth*this->health)/this->healthMax) - 2, 24-2};
		SDL_SetRenderDrawColor(gRenderer, 70, 0, 0, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render health, border
		tempRect = {uiX, uiY, (manaBarWidth*this->healthMax)/this->healthMax, 24};
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);

		// Mana bar
		uiX = screenWidth * 0.95 - 100 - 10;
		uiY = screenHeight * 0.96 - 20 - 10;

		// Render mana, bg
		manaBarWidth = 150;
		tempRect = {uiX, uiY, (manaBarWidth*this->maxMana)/this->maxMana, 24};
		SDL_SetRenderDrawColor(gRenderer, 60, 60, 60, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render mana
		tempRect = {uiX + 1, uiY + 1, ((manaBarWidth*this->mana)/this->maxMana) - 2, 24-2};
		SDL_SetRenderDrawColor(gRenderer, 40, 40, 200, 255);
		SDL_RenderFillRect(gRenderer, &tempRect);

		// Render mana, border
		tempRect = {uiX, uiY, (manaBarWidth*this->maxMana)/this->maxMana, 24};
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(gRenderer, &tempRect);
	}

	// Highscore text
	std::stringstream tempsi;
	tempsi.str( std::string() );
	tempsi << "Highscore: " << highscore;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {244, 144, 20}, gFont24);
	gText.render(gRenderer, screenWidth-gText.getWidth()-15, 5, gText.getWidth(), gText.getHeight());

	tempsi.str( std::string() );
	tempsi << "Score: " << score;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255, 255, 255}, gFont24);
	gText.render(gRenderer, screenWidth-gText.getWidth()-15, 30, gText.getWidth(), gText.getHeight());

	tempsi.str( std::string() );
	tempsi << "dmg x " << this->damageMultipler;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255,255,255}, gFont13);
	gText.render(gRenderer, this->x-camX, this->y-gText.getHeight()-camY, gText.getWidth(), gText.getHeight());

}

void Players::RenderDebug(SDL_Renderer *gRenderer, int camX, int camY)
{
	////////////////////////////////////////////////////////////
	//--------------------- Render boxes ---------------------//
	//--------------------------------------------------------//

	// Original box, untouched
	SDL_Rect tempRect = {x-camX, y-camY, w, h};
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(gRenderer, &tempRect);

	////////////////////////////////////////////////////////////
	//---------------------- Render text ---------------------//
	//--------------------------------------------------------//

	std::stringstream tempsi;
	tempsi << "attack: " << attack;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255,255,255}, gFont24);
	gText.render(gRenderer, 0, 31, gText.getWidth(), gText.getHeight());

	tempsi.str( std::string() );
	tempsi << "attackType: " << attackType;
	gText.loadFromRenderedText(gRenderer, tempsi.str().c_str(), {255,255,255}, gFont24);
	gText.render(gRenderer, 0, 57, gText.getWidth(), gText.getHeight());

	/*tempsi.str( std::string() );
	tempsi << "Health: " << health;
	gText.loadFromRenderedText(tempsi.str().c_str(), {0,255,0}, gFont24, gRenderer);
	gText.render(gRenderer, screenWidth-gText.getWidth()-15, 58, gText.getWidth(), gText.getHeight());*/

	// Check what keys are pressed
	/*for (int i=0; i<25; i++){
		std::std::stringstream tempss;
		tempss << "i: " << i << " -        " << SDL_JoystickGetButton(joy, i);
		gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
		gText.render(gRenderer, 5, 5+i*15, gText.getWidth(), gText.getHeight());
	}

	std::std::stringstream tempss;
	tempss.str(std::string());
	tempss << "Axis: " << 0 << " -        " << SDL_JoystickGetAxis(joy, 0);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 5, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 1 << " -        " << SDL_JoystickGetAxis(joy, 1);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 20, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 2 << " -        " << SDL_JoystickGetAxis(joy, 2);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 35, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 3 << " -        " << SDL_JoystickGetAxis(joy, 3);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 50, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 4 << " -        " << SDL_JoystickGetAxis(joy, 4);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 65, gText.getWidth(), gText.getHeight());

	tempss.str(std::string());
	tempss << "Axis: " << 5 << " -        " << SDL_JoystickGetAxis(joy, 5);
	gText.loadFromRenderedText(tempss.str().c_str(), {255, 255, 255, 255}, gFont, gRenderer);
	gText.render(gRenderer, 60, 80, gText.getWidth(), gText.getHeight());*/
}



// Key Pressed
void Players::OnKeyDown( Players &player, SDL_Keycode sym )
{
	switch (sym)
	{
	case SDLK_a:
		player.controls = 0;
		player.moveleft = true;
		break;
	case SDLK_d:
		player.controls = 0;
		player.moveright = true;
		break;
	case SDLK_w:
		player.controls = 0;
        if (!player.attack && !player.stunned && !player.parry) {
    		player.moveup = true;
        }
		break;
	case SDLK_s:
		player.controls = 0;
        if (!player.attack && !player.stunned && !player.parry) {
    		player.movedown = true;
        }
		break;
	case SDLK_h:
		//debug = ( !debug );
		break;
	case SDLK_y:
		player.camlocked = ( !player.camlocked );
		break;
	case SDLK_SPACE: 					// Jump
        if (!player.attack && !player.stunned && !player.parry) {
            if ( player.jumps > 0 ){
            	player.jumps--;
            	//player.y -= 5;
            	player.vY = -5;
            	player.jumpstate = "jump";
            	player.jump = true;
            }
        }
		break;
	case SDLK_n:						// Attack - Slash
		//player.initialshot = true;
		SlashAttack();
		break;
	case SDLK_m:						// Dodge
		// Future code to dodge
	case SDLK_j:						// Attack (spell attack? This is not finalized.)
		player.initialshot = true;
		break;
	case SDLK_b:						// Parry
        if (!player.parry && player.parryCDTimer <= 0 && !player.stunned)
        {
        	// enable parrying
        	player.parry = true;

        	// Play sound effect
        	Mix_PlayChannel(-1, sParry, 0);
        }
		break;
	case SDLK_LSHIFT:
		player.shift = true;
		break;
	}
}

// Key Released
void Players::OnKeyUp( Players &player, SDL_Keycode sym )
{
	switch (sym){
	case SDLK_a:
		player.moveleft = false;
		break;
	case SDLK_d:
		player.moveright = false;
		break;
	case SDLK_w:
		player.moveup = false;
		break;
	case SDLK_s:
		player.movedown = false;
		break;
	case SDLK_j:
		player.initialshot = false;
		break;
	case SDLK_LSHIFT:
		player.shift 		= false;
		break;
	case SDLK_SPACE: 					// Jump
    	player.jump = false;
		break;
	}
}

// Player Mouse Pressed
void Players::mouseClickState(Players &player, SDL_Event &e){
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			player.controls = 0;
			player.leftclick = true;
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			player.controls = 0;
			player.rightclick = true;
			player.thrust = true;
		}
	}else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			player.leftclick = false;
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			player.rightclick = false;
			player.thrust = false;
		}
	}
}

// Update XBOX 360 controls
void Players::updateJoystick(Players &player, SDL_Event &e){

	/* Xbox 360 Controls */

	// Get Left Analog Angle
	if (((SDL_JoystickGetAxis(player.joy, 0) < -JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetAxis(player.joy, 0) > JOYSTICK_DEAD_ZONE)) ||
		((SDL_JoystickGetAxis(player.joy, 1) < -JOYSTICK_DEAD_ZONE) || (SDL_JoystickGetAxis(player.joy, 1) > JOYSTICK_DEAD_ZONE))){
		player.LAngle = atan2(SDL_JoystickGetAxis(player.joy, 1), SDL_JoystickGetAxis(player.joy, 0)) * ( 180.0 / M_PI );
	}

	// Xbox 360 Controls
	if (e.type == SDL_JOYBUTTONDOWN && e.jbutton.state == SDL_PRESSED && e.jbutton.which == 0){
		switch(e.jbutton.button){
		case 0:
			player.D_UP = true;
			break;
		case 1:
			player.D_DOWN = true;
			break;
		case 2:
			player.D_LEFT = true;
			break;
		case 3:
			player.D_RIGHT = true;
			break;
		case 4:
			player.START = true;
			break;
		case 5:
			player.BACK = true;
			break;
		case 6:
			player.L3 = true;
			break;
		case 7:
			player.R3 = true;
			break;
		case 8:
			player.LB = true;
			break;
		case 9:
			player.RB = true;
			break;
		case 10:
			player.A = true;
			break;
		case 11:
			player.B = true;
			break;
		case 12:
			player.X = true;
			break;
		case 13:
			player.Y = true;
			break;
		}
	}else if (e.type == SDL_JOYBUTTONUP && e.jbutton.state == SDL_RELEASED && e.jbutton.which == 0){
		switch(e.jbutton.button){
		case 0:
			player.D_UP = false;
			break;
		case 1:
			player.D_DOWN = false;
			break;
		case 2:
			player.D_LEFT = false;
			break;
		case 3:
			player.D_RIGHT = false;
			break;
		case 4:
			player.START = false;
			break;
		case 5:
			player.BACK = false;
			break;
		case 6:
			player.L3 = false;
			break;
		case 7:
			player.R3 = false;
			break;
		case 8:
			player.LB = false;
			break;
		case 9:
			player.RB = false;
			break;
		case 10:
			player.A = false;
			break;
		case 11:
			player.B = false;
			break;
		case 12:
			player.X = false;
			break;
		case 13:
			player.Y = false;
			break;
		}
	}
}

void Players::SlashAttack() {
	if (!this->attack && !this->stunned && !this->parry) {
		// Set attack parameters
		this->playSlash = true;
		this->clash = false;
		this->attack = true;
		this->spawnAttack = true;
		// Do normal Slash Attack
		if (!this->movedown) {
			this->attackType = 0;
		}
		// Do Stab-Down attack
		else {
			this->attackType = 1;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MUTATOR FUNCTIONS ///////////////////////////////

// Applies a shield to Player
void Players::ActivateInvurnerability(){
	invurnerableFrame			= 1;
	invurnerableTick			= 0;
	invurnerableT				= 300;
	invurnerable				= true;
}

void Players::StopParrying()
{
	// Reset parry CD
	this->parryTimer = 0;
	this->parry = false;
	this->parryCDTimer = this->parryCDMax;
}

void Players::ResetParry()
{
	// Reset parry CD
	this->parryTimer = 0;
	this->parry = false;
	this->parryCDTimer = 0;

	// Increase damage multiplier
	this->damageMultipler += 1.5;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// GETTER FUNCTIONS ////////////////////////////////

float Players::getDamage() {
	return this->damage * this->damageMultipler;
}

float Players::getCastDamage() {
	return this->castDamage;
}

float Players::getCenterX() {
	return this->x + this->w/2;
}


float Players::getCenterY() {
	return this->y + this->h/2;
}


float Players::getKnockBackPower() {
	return this->knockBackPower;
}


float Players::getInvurnerableStatus() {
	return this->invurnerable;
}

float Players::getParryStatus() {
	return this->parry;
}

void Players::ResetHighScore(int LevelWeLoaded){

	// Set a file path for score.txt
	std::stringstream filePath;
	filePath << "data/maps/highscore";
	filePath << LevelWeLoaded;
	filePath << ".txt";

	// Overwrite "highscores.txt"
	std::ofstream fileSave;
	fileSave.open(filePath.str().c_str());
	fileSave << 0;
	fileSave.close();

	// Reset scores
	this->highscore = 0;
	this->score = 0;
}

void Players::ApplyHighScore(float previousHighScore) {
	this->highscore = previousHighScore;
}

void Players::SaveHighScore(int LevelWeLoaded) {

	// Open highscore first to check value
	unsigned int tempScore = -1;

	// Open file, and get previous High Score
	{
		std::stringstream filePath;
		filePath << "data/maps/highscore";
		filePath << LevelWeLoaded;
		filePath << ".txt";

		std::fstream fileOpen(filePath.str().c_str());

		// If a file exist
		if (fileOpen.is_open()) {
			// Store previous High score in this variable
			fileOpen >> tempScore;


			// Check if current Score is higher than previous High score
			if (this->score > tempScore) {

				// Save current Score as new High Score
				std::stringstream filePath;
				filePath << "data/maps/highscore";
				filePath << LevelWeLoaded;
				filePath << ".txt";

				std::ofstream fileSave;
				fileSave.open(filePath.str().c_str());
				fileSave << this->score;
				fileSave.close();
			}
		}

		// If no file exists, create new highscore
		else {

			std::cout<< "File does not exist on Saving Highscore, creating new file.\n";
			{
				std::stringstream filePath;
				filePath << "data/maps/highscore";
				filePath << LevelWeLoaded;
				filePath << ".txt";

				std::ofstream fileSave;
				fileSave.open(filePath.str().c_str());
				fileSave << this->score;
				fileSave.close();
			}
		}
		fileOpen.close();
	}
}

















