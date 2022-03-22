/*
 * PlayGame.h
 *
 *  Created on: May 15, 2017
 *      Author: Carl
 */

#ifndef PLAYGAME_H_
#define PLAYGAME_H_

#include "Game/Asteroids.h"
#include "Game/Enemies.h"
#include "Game/Maps.h"
#include "Game/Players.h"
#include "Game/Spawners.h"
#include "Game/Zombie.h"
#include "Game/Slime.h"
#include "Game/Object.h"
#include "Game/TileBar.h"
#include "Game/Tiles.h"
#include "Game/Tilec.h"
#include "Helper.h"
#include "Options.h"

class PlayGame : public Helper, public Options {
public:	// Global World Variables
	double gravity = 0.6;
	int LevelToLoad;

public: // global functions
	bool checkCollisionRect( SDL_Rect a, SDL_Rect b );

public:	// globals
	bool shift;
	bool editor;
	bool debug;
	/* 0: Textured tiles
	 * 1: Collision Tiles
	 * 2: Slime objects
	 *
	 */
	int place_type;
	int newMx, newMy;
	int mex, mey;
	bool camUp = false;
	bool camDown = false;
	bool camLeft = false;
	bool camRight = false;
	bool leftClick = false;
	bool rightClick = false;

public: // Tile Variables
	SDL_Rect rTiles[168];

public: // System variables
	enum Result { Back, Nothing, StartGame, ShowingMenu, Exit };
	void Show(LWindow &gWindow, SDL_Renderer *gRenderer,
			  PlayGame::Result &result, int LevelToLoad);
	PlayGame::Result mousePressed(SDL_Event event);
	PlayGame::Result mouseReleased(SDL_Event event);

public:	// Scene textures
	LTexture gBG;
	LTexture gTargetTexture;
	LTexture gCircle;

	// Mouse cursor
	LTexture gCursor;

	class Text {
	public:
		int count;
		float x;
		float y;
		int w;
		int h;
		int alpha;
		int timer;
		bool alive;
		float vX;
		float vY;

		/*
		 * 0: General information logs like debug logs
		 * 1: These will have an x & y coordinate somewhere
		 */
		int type;

		std::string textfield;
		void init(Text text[]) {
			count =0;
			for (int i=0; i<100; i++) {
				text[i].alive = false;
			}
		}
		void spawn(Text text[], float x, float y, float vX, float vY, int alpha, std::string textfield, int type) {
			for (int i=0; i<100; i++) {
				if (!text[i].alive) {
					//std::cout << "Spawning text object." << std::endl;
					text[i].type = type;
					text[i].x = x;
					text[i].y = y;
					text[i].w = 139;
					text[i].h = 96;
					text[i].vX = vX;
					text[i].vY = vY;
					text[i].timer = timer;
					text[i].alpha = alpha;
					text[i].textfield = textfield;
					text[i].alive = true;
					count++;
					break;
				}
			}
		}
		void updateDebugText(Text text[]) {
			for (int i=0; i<100; i++) {
				if (text[i].alive && text[i].type == 0) {
					text[i].x += text[i].vX;
					text[i].y += text[i].vY;

					text[i].alpha -= 2;
					if (text[i].alpha <= 0) {
						text[i].alive = false;
						count--;
					}
				}
			}
		}

		void updateDamageText(Text text[]) {
			for (int i=0; i<100; i++) {
				if (text[i].alive && text[i].type == 1) {
					text[i].x += text[i].vX;
					text[i].y += text[i].vY;

					text[i].alpha -= 2;
					if (text[i].alpha <= 0) {
						text[i].alive = false;
						count--;
					}
				}
			}
		}
	};

	Text tex;
	Text text[100];

public:	// Other classes
	Map map;
	Particle part;
	Asteroid aste;
	Spawner spaw;
	Enemy enem;
	//Pause pau;
	//Pause::Result pauseResult;
	Players player;

	// Class instance, multiple
	Particle particles[512];
	Asteroid asteroid[128];
	Enemy enemy[8];
	Spawner spawner[200];
	//Mixed mix;
	//Controls cont;
	//static Items items[10];

	// Tiles
	Tile tl;
	Tile tile[5000];

	// Collision Tiles
	Tilec tlc;
	Tilec tilec[1000];

	// Tilebar
	TileBar tb;
	TileBar tilebar[168];

	// Objects (Player attack collisions
	Object obj;
	Object object[10];

	// Zombie
	Zombie zom;
	Zombie zombie[100];

	// Slime
	Slime sli;
	Slime slime[100];

public:	// Player variables
	// Player spawn point
	float spawnX = 0, spawnY = 0;
	std::string saveSpawnPoint();
	void loadSpawnPoint();
public:
    // camera
    int camx;
    int camy;
    bool camlock;

public:	// Initial functions

	// Initialize
	void Init();

	// Load resources
	void Load(LWindow &gWindow, SDL_Renderer *gRenderer);

	// Free resources
	void Free();

public:	// Core functions
	// Update everything
	void Update(LWindow &gWindow, SDL_Renderer *gRenderer);

	// Render foreground
	void RenderFG(SDL_Renderer *gRenderer, LWindow &gWindow);

	// Render game objects
	void Render(SDL_Renderer *gRenderer, LWindow &gWindow);

	// Render UI
	void RenderUI(SDL_Renderer *gRenderer, LWindow &gWindow);

	// Render debug information
	void RenderDebug(SDL_Renderer *gRenderer);

	// Render text
	void RenderText(SDL_Renderer *gRenderer, LWindow &gWindow);

public:	// Functions mixed with other classes
	// Check collision between Particle & Asteroid
	void checkCollisionPlayerAsteroid(Players &player, Asteroid asteroid[]);

	// Check collision between Particle & Asteroid
	void checkCollisionParticleAsteroid(Particle particle[], Particle &part, Asteroid asteroid[], Players &player);

	// Check collision between Particle & Enemy
	void checkCollisionParticleEnemy(Particle particle[], Particle &part, Enemy enemy[], Players &player);

	// Check collision between Particle & Zombie
	void checkCollisionParticleZombie();

	// Check collision between Particle & Tile
	void checkCollisionParticleTile();

	// Check collision between Particle & Slime
	void checkCollisionParticleSlime();

	// Check collision between Slime & Tile
	void checkSlimeTileCollision();

	// Check collision between Player attacks & Slime
	void checkPlayerAttacksCollisionSlime();

	// Check collision between Player attacks & Tile
	void checkPlayerAttacksTileCollision();

	// Check collision between Slime attacks & Player
	void checkSlimeAttacksCollisionPlayer();

	// Check collision between Particle & Player
	void checkCollisionParticlePlayer(Particle particle[], Particle &part, Players &player);

	// Spawn Asteroids upon destroying all Asteroids
	void spawnAsteroidsNow2();

public:	// Controls
	// Editor controls
	void editorOnKeyDown(SDL_Keycode sym);
	void editorOnKeyUp(SDL_Keycode sym);

private:	// used for some debugging
	int mx, my;
	bool quit;
	SDL_Event event;

private:	// Load level

	// Load config files
	void LoadCFG();

	// Load previous high-score for Level
	void LoadHighScore();

	// Load level
	void LoadLevel();
};

#endif /* PLAYGAME_H_ */
