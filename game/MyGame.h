#pragma once

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSprite player;	// player sprite
	CSprite startScreen;
    CSprite background;
	CSprite winBackground;
	CSpriteList platformList; // platforms and obstacles are kept in a sprite list
	CSpriteList bulletList;
	CSpriteList enemyBulletlist;
	CSpriteList enemyBlue;
	CSpriteList enemyRed;
	CSpriteList enemyBlack;
	CSpriteList blueDeathAnimation;
	CSpriteList redDeathAnimation;
	CSpriteList blackDeathanimation;

	CSoundPlayer playerSounds;
	CSoundPlayer mainMenuMusic;
	CSoundPlayer gunSounds;
	CSoundPlayer levelMusic;
	CSoundPlayer endMusic;

	int level;
	int gunAmmo;
	int currentAmmo;
	int maxAmmo = 30;
	int score;
	 
	bool isLevelComplete;

	int gunCounter;
	int reloadTimer;
	int bulletDamage = 15;
	int playerLives;
	int currentHealth;

	//Enemy
	int enemyBlueCurrentHealth;
	int enemyRedCurrentHealth;
	int enemyBlackCurrentHealth;
	int enemyCounter;

	// my own member functions
void PlayerControl();
void EnemyControl();
void ReloadGun();

void SetupLevel1();
void SetupLevel2();
void SetupLevel3();

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
