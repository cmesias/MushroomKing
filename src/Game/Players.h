/*
 * Players.h
 *
 *  Created on: Dec 18, 2016
 *      Author: Carl
 */

#ifndef LOCAL_PLAYERS_H_
#define LOCAL_PLAYERS_H_

#include <SDL2/SDL_ttf.h>

#include "../LWindow.h"
#include "../LTexture.h"
#include "../Particless.h"
#include "Asteroids.h"
#include "Enemies.h"
#include "../Helper.h"
#include "Maps.h"
#include "Tiles.h"
#include "Tilec.h"
#include "Object.h"
#include "TextNFont.h"

//Player
class Players: public Helper, public TextNFont {
public:
	enum Result { QUIT, LOADMENU, PLAYGAME, QUITPLAYGAME };
public:
	// Joystick Sensitivity
	const int JOYSTICK_DEAD_ZONE = 8000;
	void OnKeyDown( Players &player, SDL_Keycode sym );
	void OnKeyUp( Players &player, SDL_Keycode sym );
	void mouseClickState(Players &player, SDL_Event &e);
	void updateJoystick(Players &player, SDL_Event &e);

public:	// resources
	// Local resources
	LTexture gPlayer;
	LTexture gShield;
	SDL_Rect rPlayer[8];
	SDL_Rect rShield[7];

	// Audio
	Mix_Chunk *sCast 			= NULL;
	Mix_Chunk *sSlash 			= NULL;
	Mix_Chunk *sDownStab		= NULL;
	Mix_Chunk *sParry			= NULL;

	/*
	 * 0-3: Walk
	 * 4:   Before slash
	 * 5:   Slash
	 * 7:   Down Stab
	 */
	int sprite_index;

public:	// variables
	int timer = 0;
	//int w = 208, h = 208;
	int realw = 48, realh = 48;			// object size in actual pixel size
	int w = 24, h = 48;					// object size in collision pixels
	//float x = map.x+map.w/2-32/2,
	//	  y = map.y+map.h/2-32/2;
	float x = 0,
		  y = 0;
	float x2, y2;					// player circle center
	float swordX;					// x pos Location of sword when down-stab attack
	float swordY;					// y pos Location of sword when down-stab attack
	int radius = w/2;				// player radius
	int panTimer 		= 0;
	int shoottimer 		= 0;
	float angle 		= 0.0,
		   radians 		= 0.0,
		   radianCos 	= 0.0,
		   radianSin 	= 0.0;
	float vX = 0.0,
		  vY = 0.0,
		  thrustSpeed 	= 0.0;
	float vYMax			= 10;
	bool camlocked 		= true;
	bool moveleft 		= false,
		 moveright 		= false,
		 moveup 		= false,
		 movedown		= false,
		 rotateLeft 	= false,
		 rotateRight 	= false;
	std::string side 		= "r";
	std::string travelMode 	= "travel";
	std::string name;
	bool thrust 		= false;
	int thrustT 		= 0;
	bool shift 			= false;
	int timer2 			= 0;
	double time 		= 0;
	bool deathScreen	= false;
	bool alive;
	bool returned		= false;
	Uint8 alpha			= 255;
	unsigned int score	= 0;
	unsigned int highscore = 0;
	int HW				= 0;
	int increment		= 35;
	int wave			= 0;
	double accuracy		= 0.0;
	float totalShot		= 0;		// Total shot
	float hits			= 0;		// Total hits
	int bg 				= 1;

	// Fire-rate
	//string power-up 		= "LAZER";
	int newmx			= 0;	// new mouse position
	int newmy			= 0;	// new mouse position
	int powerup 		= 1;
	double barrelX		= 0;	// gun barrel position
	double barrelY		= 0;
	double barrelW		= 0;	// whether to add width or subtract from gun barrel position
	double barrelH		= 0;
	double bulletW 		= 92;	// position the bullet will come out of the player ONLY if player
	double bulletH 		= 21;	// is centered (have angle at 0.0 to test)
	double particleW 	= 30;
	double particleH 	= 30;
	float delayT 		= 0;
	bool delay 			= false;
	bool initialshot 	= false;
	bool trigger 		= false;
	bool renderFlash	= false; // shuts off right after 1 frame
	bool playSFXDownStabHittingFloor	= true;
	double recoilX = 0;			 // gun recoil
	double recoilY = 0;

	// Lives
	int lives = 1;
	int health			= 100;
	const int healthMax		= 100;

	double tempc, temps;
	SDL_Rect continueButton[3];

	// High-Score display
	std::string highList[10];
	int indx 		= 0;
	float travel 	= 0.0;
	int position	= 0;
	int position2	= 0;
	int dir 		= 1;
	int indexSaved 	= -1;

	/* This will bring the player
	 * to life. At least, that's what
	 * it'll look like. This will rustle
	 * the player's armor as they move
	 */
	float rustleW = 0;
	float rustleH = 0;
	float rustleDirX = 1;
	float rustleDirY = 1;

	bool moving = false;

public: // attack variables
	std::string facing;
	std::string jumpstate;
	SDL_RendererFlip flipW;
	double grav;
	int jumps;
	int jumpTimer;
	int attackTimer;
	int attackFrame;
	int attackType;	// 0: slash, 1: down stab
	double walkTimer;
	int stunTimer;
	bool jump;
	bool attack;
	bool stunned;
	bool playSlash;
	bool clash;
	bool spawnAttack;
    //float *tempXX;
    //float *tempYY;

public:	// controls
	// Player controls
	int controls;		// [0] Keyboard, [1] Xbox 360 Controller
	SDL_Joystick *joy;
	bool A;
	bool B;
	bool X;
	bool Y;
	bool D_UP;
	bool D_DOWN;
	bool D_LEFT;
	bool D_RIGHT;
	bool LB;
	bool RB;
	bool BACK;
	bool START;
	bool L3;
	bool R3;
	double LAngle;
	double RAngle;
	double zAxisLeft;
	double zAxisRight;
	bool leftclick;
	bool rightclick;
	bool test;

public:	// functions
	// Functions
	void Load(SDL_Renderer* gRenderer);
	void Free();
	void Init(float spawnX, float spawnY, std::string newName, bool respawn);
	bool checkCollision(int x, int y, int w, int h, int x2, int y2, int w2, int h2);

	// Get audio files
	//void getAudio(Mix_Chunk* sLazer);

	// Player shoot
	void fire(Particle particle[], Particle &p_dummy, Mix_Chunk* sCastSFX, int mx, int my);

	void update(Map &map,
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
				int LevelWeLoaded);

	void Render(int mx, int my, int camX, int camY, LWindow gWindow,
				SDL_Renderer* gRenderer,
				SDL_Color color, int &PARTICLES, LTexture gText);

	void RenderUI(SDL_Renderer *gRenderer, int camX, int camY);

	void RenderDebug(SDL_Renderer* gRenderer, int camX, int camY);

private:	// Private variables

	// Invurnerable
	int invurnerableFrame;
	int invurnerableTick;
	int invurnerableT;
	bool invurnerable;

	// Parry
	bool parry;

private:	// Private variables
			// Save these player stats in a file. This will be the players save data.

	float AttackSpeed;		// Attack speed

	float maxMana;			// Max mana
	float mana;				// Mana
	float manaRegenSpeed;	// Mana
	float manaRegenTimer;	// 1 second timer for mana regen;

	float damage;			// Sword damage
	float damageMultipler;	// Parrying will increase damage multiplier
	float castDamage;		// Cast damage

	float knockBackPower;	// Knock back power

	int parryTimer;			// Parry timer
	int parryCDTimer;		// Parry cooldown timer tracker
	int parryCDMax;			// Parry cooldown

public:	// Functions to do stuff?

	void SlashAttack();

public:	// Mutator functions

	// Applies invurnerability to Player
	void ActivateInvurnerability();

	// Stops parry but goes on cool down
	void StopParrying();

	// Stops parry but reset cool down to 0
	void ResetParry();

public:	// Accessor functions

	// Get player damage
	float getDamage();

	// Get player cast damage
	float getCastDamage();

	// Get x center of player
	float getCenterX();

	// Get y center of player
	float getCenterY();

	// Get knockback power
	float getKnockBackPower();

	// Get status of invurnerability
	float getInvurnerableStatus();

	// Get status of parrying
	float getParryStatus();

public:

	// Reset High Scores
	void ResetHighScore(int LevelWeLoaded);

	// Load previous high score from PlayGame.cpp
	void ApplyHighScore(float previousHighScore);

	// Save current highs-core for current Level
	void SaveHighScore(int LevelToLoad);

};

#endif /* LOCAL_PLAYERS_H_ */
