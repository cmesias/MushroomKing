/*
 * Zombie.h
 *
 *  Created on: May 31, 2017
 *      Author: Carl
 */

#ifndef GAME_ZOMBIE_H_
#define GAME_ZOMBIE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Helper.h"
#include "Players.h"
#include "Maps.h"
#include "TextNFont.h"

class Zombie : public Helper, public TextNFont {
public:	// Media
	LTexture gTexture;
	LTexture gCircle;
	int count;
	const int max = 100;
	bool asteroidsClear;

public:
	float x2, y2;
	int radius;
	float x,y,w,h;
	float angle;
	float vX,vY;
	float speed;
	int type;
	int health;
	int damage;
	int timer;			// Used for shooting
	float distance;
	bool alive;
	bool collision;
	bool onScreen;

public:
	void Init(Zombie zombie[]);
	void Load(SDL_Renderer* gRenderer);
	void Free();

	void clear(Zombie zombie[]);
	void spawn(Zombie zombie[], float x, float y, float w, float h, float angle, float speed, int type);
	void update(Zombie zombie[], Particle particle[], Particle &p_dummy,
				Map &map, Players &player, Mix_Chunk* sLazer,
				int camx, int camy);

	void Render(SDL_Renderer* gRenderer, Zombie zombie[], int camx, int camy);

	void RenderDebug(SDL_Renderer* gRenderer, Zombie zombie[], int camx, int camy);
};

#endif /* GAME_ZOMBIE_H_ */
