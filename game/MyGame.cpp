#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) :
	startScreen(400,300,"MainMenu.bmp",0),
	winBackground(400,300, "EndMenu.bmp", 0)
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	// do not run game logic when in menu mode
	if (IsMenuMode()) return;

	Uint32 t = GetTime();

	// TODO: add the game update code here
	player.Update(t);	

	for (CSprite* newShot : bulletList) newShot->Update(t);

	for (CSprite* enemyShot : enemyBulletlist) enemyShot->Update(t);

	for (CSprite* pSprite : platformList) pSprite->Update(t);

	for (CSprite* pEnemyBlue : enemyBlue) pEnemyBlue->Update(t);

	for (CSprite* pEnemyRed : enemyRed) pEnemyRed->Update(t);

	for (CSprite* pEnemyBlack : enemyBlack) pEnemyBlack->Update(t);

	for (CSprite* blueDeath : blueDeathAnimation) blueDeath->Update(t);

	for (CSprite* redDeath : redDeathAnimation) redDeath->Update(t);

	for (CSprite* blackDeath : blackDeathanimation) blackDeath->Update(t);

	winBackground.Update(t);

	if (level == 1 && score >= 100)
	{
		level = 2;
		SetupLevel2();
		isLevelComplete = false;
	}
	if (level == 2 && score >= 200)
	{
		level = 3;
		SetupLevel3();
		isLevelComplete = false;
	}

	if (currentHealth <= 0 && level == 1)
	{
		OnStartGame();
		playerLives = playerLives - 1;
	}

	if (currentHealth <= 0 && level == 2)
	{
		OnStartGame();
		playerLives = playerLives - 1;
	}

	if (currentHealth <= 0 && level == 3)
	{
		OnStartGame();
		playerLives = playerLives - 1;
	}

	if (playerLives <= 0)
	{
		GameOver();
	}
	
    
	if (player.GetY() < 0) GameOver();
	PlayerControl();

	if (player.GetLeft() <= 0 && (player.GetXVelocity() < 0))
	{
		player.SetVelocity(-player.GetXVelocity(), player.GetYVelocity());
	}

	EnemyControl();
}


void CMyGame::OnDraw(CGraphics* g)
{
	int leftScreenLimit = 300;
	int rightScreenLimit = 1900;
	
	// draw menu when in menu mode
	if (IsMenuMode())
	{
		startScreen.Draw(g);
		return;
	}

	if (level == 3 && score >= 300)
	{
		winBackground.Draw(g);
		isLevelComplete = true;
		enemyBlue.delete_all();
		enemyRed.delete_all();
		enemyBlack.delete_all();
		return;
	}

	// we scroll the whole game world according to the player position
	if (player.GetX() >= leftScreenLimit && player.GetX() <= rightScreenLimit)
	{
		g->SetScrollPos(leftScreenLimit - player.GetX(), 0);
	}
	// we stop scrolling with the player once we have reached the right limit
	if (player.GetX() > rightScreenLimit)
	{
		g->SetScrollPos(-rightScreenLimit+leftScreenLimit, 0);
	}

	

	background.Draw(g);

	for (CSprite* bullet : bulletList) bullet->Draw(g);
	for (CSprite* pSprite : platformList) pSprite->Draw(g);
	for (CSprite* pEnemyBlue : enemyBlue) pEnemyBlue->Draw(g);
	for (CSprite* enemyShot : enemyBulletlist) enemyShot->Draw(g);
	for (CSprite* pEnemyRed : enemyRed) pEnemyRed->Draw(g);
	for (CSprite* pEnemyBlack : enemyBlack) pEnemyBlack->Draw(g);
	for (CSprite* blueDeath : blueDeathAnimation) blueDeath->Draw(g);
	for (CSprite* redDeath : redDeathAnimation) redDeath->Draw(g);
	for (CSprite* blackDeath : blackDeathanimation) blackDeath->Draw(g);

	player.Draw(g);
	  
	 // don't scroll the overlay screen
	 g->SetScrollPos(0, 0);

	 *g << font(16) << color(CColor::Red()) << xy(5, 580) << "Player Health: " << currentHealth;
	 
	 *g << font(18) << color(CColor::Red()) << xy(5, 10) << "Ammo: " << currentAmmo;
	 
	 *g << font(18) << color(CColor::Red()) << xy(700, 580) << "Score: " << score;

	 *g << font(18) << color(CColor::Red()) << xy(5, 560) << "Lives: " << playerLives;

	 *g << font(18) << color(CColor::Red()) << xy(400, 580) << "Level: " << level;

	 if (IsGameOverMode())  *g << font(40) << color(CColor::Green()) << xy(400, 300) << "Game Over";
	
}

void CMyGame::PlayerControl()
{
	float jumpSpeed = 675; // jumpin up speed
	float slowDown = 15; // speed reduction when jumping
	float walkSpeed = 200; // horizontal walking speed

	CVector oldPos = player.GetPosition();

	enum { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, JUMPLEFT, JUMPRIGHT, CROUCHRIGHT, CROUCHLEFT, ATTACKRIGHT, ATTACKLEFT } p_state;
	/*
	  player status variable is used to indicate which state the player is in

	  1: standing left looking
	  2: standing right looking
	  3: walking left
	  4: walking right
	  5: jumping/falling left
	  6: jumping/falling right

	*/
	// gravity simulation 
	if (player.GetStatus() > 4) player.SetYVelocity(player.GetYVelocity() - slowDown);

	// walking RIGHT
	if (IsKeyDown(SDLK_d) && player.GetStatus() != WALKRIGHT && player.GetStatus() < WALKRIGHT)
	{
		player.SetMotion(walkSpeed, 0);
		playerSounds.Play("PlayerRun.wav", -1);
		player.SetAnimation("walkright", 3);
		player.SetStatus(WALKRIGHT);
		player.Update(GetTime());
	}
	// standing right
	else if (!IsKeyDown(SDLK_d) && player.GetStatus() == WALKRIGHT)
	{
		player.SetMotion(0, 0);
		player.SetAnimation("standright");
		player.SetStatus(STANDRIGHT);
		player.Update(GetTime());
		playerSounds.Stop();
	}
	// walking LEFT
	if (IsKeyDown(SDLK_a) && player.GetStatus() != WALKLEFT && player.GetStatus() < WALKLEFT)
	{
		player.SetMotion(-walkSpeed, 0);
		player.SetAnimation("walkleft", 3);
		player.SetStatus(WALKLEFT);
		player.Update(GetTime());
		playerSounds.Play("PlayerRun.wav", -1);
	}
	// standing left
	else if (!IsKeyDown(SDLK_a) && player.GetStatus() == WALKLEFT)
	{
		player.SetMotion(0, 0);
		player.SetAnimation("standleft");
		player.SetStatus(STANDLEFT);
		player.Update(GetTime());
		playerSounds.Stop();
	}

	// JUMPING
	if (IsKeyDown(SDLK_w) && player.GetStatus() != JUMPLEFT && player.GetStatus() != JUMPRIGHT)
	{
		if (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT)
		{
			player.SetYVelocity(jumpSpeed);
			player.SetAnimation("jumpleft");
			player.SetStatus(JUMPLEFT);
			player.Update(GetTime());
			playerSounds.Play("PlayerJump.wav");
		}
		if (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT)
		{
			player.SetYVelocity(jumpSpeed);
			player.SetAnimation("jumpright");
			player.SetStatus(JUMPRIGHT);
			player.Update(GetTime());
			playerSounds.Play("PlayerJump.wav");
		}
	}

	// SHOOTING
	if (IsKeyDown(SDLK_j) && player.GetStatus() != ATTACKLEFT && player.GetStatus() != ATTACKRIGHT)
	{
		if (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT)
		{
			player.SetAnimation("shootleft");
			gunCounter = 30;
			reloadTimer = 61;
			if (currentAmmo <= 0)
			{
				ReloadGun();
			}
			player.Update(GetTime());
		}
		if (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT)
		{
			player.SetAnimation("shootright");
			gunCounter = 30;
			reloadTimer = 61;
			if (currentAmmo <= 0)
			{
				ReloadGun();
			}
			player.Update(GetTime());
		}
	}
	else if (!IsKeyDown(SDLK_j) && player.GetStatus() == ATTACKLEFT && player.GetStatus() == ATTACKRIGHT)
	{
		if (player.GetStatus() == ATTACKLEFT)
		{
			player.SetAnimation("standleft");
			player.SetStatus(STANDLEFT);
			player.Update(GetTime());
		}
		else if (player.GetStatus() == ATTACKRIGHT)
		{
			player.SetAnimation("standright");
			player.SetStatus(STANDRIGHT);
			player.Update(GetTime());
		}
	}

	if (gunCounter > 0)
	{
		gunCounter--;
		if (gunCounter == 1 && player.GetStatus() == STANDRIGHT)
		{
			player.SetAnimation("standright");
		}

		if (gunCounter == 1 && player.GetStatus() == STANDLEFT)
		{
			player.SetAnimation("standleft");
		}

		if (gunCounter == 25 && player.GetStatus() == STANDRIGHT)
		{
			CSprite* newShot = new CSprite(player.GetX() + 30, player.GetY(), "PlayerBulletSprite.bmp", CColor::Black(), GetTime());
			newShot->SetMotion(375, 0);
			gunSounds.Play("PlayerShoot.wav");
			currentAmmo--;
			newShot->Die(1200);
			bulletList.push_back(newShot);
		}

		if (gunCounter == 25 && player.GetStatus() == STANDLEFT)
		{
			CSprite* newShot = new CSprite(player.GetX() - 30, player.GetY(), "PlayerBulletSprite.bmp", CColor::Black(), GetTime());
			newShot->SetMotion(-375, 0);
			gunSounds.Play("PlayerShoot.wav");
			currentAmmo--;
			newShot->Die(1200);
			bulletList.push_back(newShot);
		}
	}

	// ----- testing if we are on a platform -------
	bool onPlatform = false;
	player.SetY(player.GetY() - 5); // move a little bit down
	for (CSprite* pPlatform : platformList)
	{
		if (player.HitTest(pPlatform))
		{
			onPlatform = true;
		}
	}
	player.SetY(player.GetY() + 5);

	// ---- collision with platforms
	player.Proceed(5);



	for (CSprite* pPlatform : platformList)
	{
		if (player.HitTest(pPlatform, 1))
		{
			// where did we hit the platform
			 // below platform
			if (player.GetY() < pPlatform->GetBottom())
			{
				player.SetY(player.GetY() - 5);
				player.SetYVelocity(0);
				playerSounds.Play("PlayerLand.wav");
			}
			// above platform
			else if (player.GetBottom() >= pPlatform->GetBottom() && (player.GetStatus() == JUMPLEFT || player.GetStatus() == JUMPRIGHT))
			{
				onPlatform = true;
				if (player.GetStatus() == JUMPLEFT)
				{
					player.SetMotion(0, 0);
					player.SetAnimation("standleft");
					player.SetStatus(STANDLEFT);
					player.Update(GetTime());
					playerSounds.Play("PlayerLand.wav");
					player.SetY(pPlatform->GetTop() + player.GetHeight() / 2);
				}
				if (player.GetStatus() == JUMPRIGHT)
				{
					player.SetMotion(0, 0);
					player.SetAnimation("standright");
					player.SetStatus(STANDRIGHT);
					player.Update(GetTime());
					playerSounds.Play("PlayerLand.wav");
					player.SetY(pPlatform->GetTop() + player.GetHeight() / 2);
				}

			}
			// left of platform;
			else if (player.GetX() < pPlatform->GetLeft())
			{
				player.SetXVelocity(0);
				player.SetYVelocity(0);
				player.SetPosition(oldPos);

			}
			// right of platform
			else if (player.GetX() > pPlatform->GetRight())
			{
				player.SetXVelocity(0);
				player.SetYVelocity(0);
				player.SetPosition(oldPos);
			}

		}

		for (CSprite* newShot : bulletList)
		{
			if (newShot->HitTest(pPlatform))
			{
				gunSounds.Play("BulletImpact.wav");
				newShot->Delete();
			}
		}

		for (CSprite* enemyShot : enemyBulletlist)
		{
			if (enemyShot->HitTest(pPlatform))
			{
				gunSounds.Play("BulletImpact.wav");
				enemyShot->Delete();
			}
		}

		enemyBulletlist.delete_if(deleted);
		bulletList.delete_if(deleted);
	}
		
	player.Proceed(-5);

	// if we are not on a platform then we should fall
	if (!onPlatform && (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT)) // no ground
	{
		player.SetStatus(JUMPLEFT); // make fall

	}
	// if we are not on a platform then we should fall
	if (!onPlatform && (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT)) // no ground
	{
		player.SetStatus(JUMPRIGHT); // make fall
	}
	
	player.Update(GetTime());
}

void CMyGame::ReloadGun()
{
	reloadTimer--;

	if (reloadTimer > 0)
	{
		if (reloadTimer == 60)
		{
			gunSounds.Play("PlayerReload.wav");
			currentAmmo = maxAmmo;
		}
		if (reloadTimer == 1)
		{
			gunSounds.Stop();
		}
	}

}

void CMyGame::EnemyControl()
{
	float slowdown = 15;
	float enemyWalkSpeed = 145;

	enum { NONE, ESTANDLEFT, ESTANDRIGHT, EWALKLEFT, EWALKRIGHT, EATTACKLEFT, EATTACKRIGHT};


	//Blue Enemy
	for (CSprite* pEnemyBlue : enemyBlue)
	{
		for (CSprite* newShot : bulletList)
		{
			if (newShot->HitTest(pEnemyBlue, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				newShot->Die(10000);
				newShot->Delete();
				enemyBlueCurrentHealth = enemyBlueCurrentHealth - 100;

				if (enemyBlueCurrentHealth <= 0)
				{
					CSprite* deathBlue = new CSprite(pEnemyBlue->GetX(), pEnemyBlue->GetY(), 0, 0, GetTime());
					deathBlue->LoadAnimation("EnemyBlueDeathRight.bmp", "enemybluedeath", CSprite::Sheet(8, 1).Row(0).From(0).To(7), CColor::Red());
					deathBlue->SetAnimation("enemybluedeath");
					score = score + 10;
					deathBlue->Die(800);
					pEnemyBlue->Delete();
					blueDeathAnimation.push_back(deathBlue);

				}
			}

		}

		blueDeathAnimation.delete_if(deleted);

		for (CSprite* enemyShot : enemyBulletlist)
		{
			//Enemy test hit
			if (enemyShot->HitTest(&player, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				currentHealth = currentHealth - 15;
				enemyShot->Die(10000);
				enemyShot->Delete();
			}
		}

		enemyBulletlist.delete_if(deleted);

		//if (pEnemyBlue->GetStatus() > 4) pEnemyBlue->SetYVelocity(pEnemyBlue->GetYVelocity() - slowdown);



		CVector Player;

		float dist = Distance(player.GetPosition(), pEnemyBlue->GetPosition());

		if (pEnemyBlue->GetX() < 200 && pEnemyBlue->GetXVelocity() < 0 && pEnemyBlue->GetStatus() != EATTACKRIGHT)
		{
			pEnemyBlue->SetMotion(enemyWalkSpeed, 0);
			pEnemyBlue->LoadAnimation("EnemyBlueRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
			pEnemyBlue->SetAnimation("enemywalkright");
			pEnemyBlue->SetStatus(EWALKRIGHT);
			pEnemyBlue->Update(GetTime());
		}

		if (pEnemyBlue->GetX() > 1900 && pEnemyBlue->GetXVelocity() > 0 && pEnemyBlue->GetStatus() != EATTACKLEFT)
		{
			pEnemyBlue->SetMotion(-enemyWalkSpeed, 0);
			pEnemyBlue->LoadAnimation("EnemyBlueRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
			pEnemyBlue->SetAnimation("enemywalkleft");
			pEnemyBlue->SetStatus(EWALKLEFT);
			pEnemyBlue->Update(GetTime());
		}

		if (dist < 100 && pEnemyBlue->GetStatus() == EWALKLEFT)
		{
			pEnemyBlue->SetMotion(0, 0);
			pEnemyBlue->LoadAnimation("EnemyBlueIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
			pEnemyBlue->SetAnimation("enemyidleleft");
			pEnemyBlue->SetStatus(EATTACKLEFT);
			pEnemyBlue->Update(GetTime());
		}

		if (dist < 100 && pEnemyBlue->GetStatus() == EWALKRIGHT)
		{
			pEnemyBlue->SetMotion(0, 0);
			pEnemyBlue->LoadAnimation("EnemyBlueIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
			pEnemyBlue->SetAnimation("enemyidleright");
			pEnemyBlue->SetStatus(EATTACKRIGHT);
			pEnemyBlue->Update(GetTime());
		}

		if (rand() % 500 == 0 && dist < 200 && pEnemyBlue->GetStatus() == EATTACKRIGHT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyBlue->GetX() + 30, pEnemyBlue->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
		if (rand() % 500 == 0 && dist < 200 && pEnemyBlue->GetStatus() == EATTACKLEFT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyBlue->GetX() + 30, pEnemyBlue->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(-300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
	}
	enemyBlue.delete_if(deleted);



	//Red Enemy
	for (CSprite* pEnemyRed : enemyRed)
	{
		for (CSprite* newShot : bulletList)
		{
			if (newShot->HitTest(pEnemyRed, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				newShot->Die(10000);
				newShot->Delete();
				enemyRedCurrentHealth = enemyRedCurrentHealth - 100;

				if (enemyRedCurrentHealth <= 0)
				{
					CSprite* deathRed = new CSprite(pEnemyRed->GetX(), pEnemyRed->GetY(), 0, 0, GetTime());
					deathRed->LoadAnimation("EnemyRedDeathRight.bmp", "enemyreddeath", CSprite::Sheet(8, 1).Row(0).From(0).To(7), CColor::Red());
					deathRed->SetAnimation("enemyreddeath");
					score = score + 25;
					deathRed->Die(800);
					pEnemyRed->Delete();
					redDeathAnimation.push_back(deathRed);
				}
			}
		}

		redDeathAnimation.delete_if(deleted);

		for (CSprite* enemyShot : enemyBulletlist)
		{
			//Enemy test hit
			if (enemyShot->HitTest(&player, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				currentHealth = currentHealth - 15;
				enemyShot->Die(10000);
				enemyShot->Delete();
			}
		}

		enemyBulletlist.delete_if(deleted);
		//if (pEnemyBlue->GetStatus() > 4) pEnemyBlue->SetYVelocity(pEnemyBlue->GetYVelocity() - slowdown);



		CVector Player;

		float dist = Distance(player.GetPosition(), pEnemyRed->GetPosition());

		if (pEnemyRed->GetX() < 200 && pEnemyRed->GetXVelocity() < 0 && pEnemyRed->GetStatus() != EATTACKRIGHT)
		{
			pEnemyRed->SetMotion(enemyWalkSpeed, 0);
			pEnemyRed->LoadAnimation("EnemyRedRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
			pEnemyRed->SetAnimation("enemywalkright");
			pEnemyRed->SetStatus(EWALKRIGHT);
			pEnemyRed->Update(GetTime());
		}

		if (pEnemyRed->GetX() > 1900 && pEnemyRed->GetXVelocity() > 0 && pEnemyRed->GetStatus() != EATTACKLEFT)
		{
			pEnemyRed->SetMotion(-enemyWalkSpeed, 0);
			pEnemyRed->LoadAnimation("EnemyRedRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
			pEnemyRed->SetAnimation("enemywalkleft");
			pEnemyRed->SetStatus(EWALKLEFT);
			pEnemyRed->Update(GetTime());
		}

		if (dist < 200 && pEnemyRed->GetStatus() == EWALKLEFT)
		{
			pEnemyRed->SetMotion(0, 0);
			pEnemyRed->LoadAnimation("EnemyRedIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
			pEnemyRed->SetAnimation("enemyidleleft");
			pEnemyRed->SetStatus(EATTACKLEFT);
			pEnemyRed->Update(GetTime());
		}

		if (dist < 200 && pEnemyRed->GetStatus() == EWALKRIGHT)
		{
			pEnemyRed->SetMotion(0, 0);
			pEnemyRed->LoadAnimation("EnemyRedIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
			pEnemyRed->SetAnimation("enemyidleright");
			pEnemyRed->SetStatus(EATTACKRIGHT);
			pEnemyRed->Update(GetTime());
		}

		if (rand() % 500 == 0 && dist < 200 && pEnemyRed->GetStatus() == EATTACKRIGHT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyRed->GetX() + 30, pEnemyRed->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
		if (rand() % 500 == 0 && dist < 200 && pEnemyRed->GetStatus() == EATTACKLEFT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyRed->GetX() + 30, pEnemyRed->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(-300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
	}
	enemyRed.delete_if(deleted);

	//Black Enemy
	for (CSprite* pEnemyBlack : enemyBlack)
	{
		for (CSprite* newShot : bulletList)
		{
			if (newShot->HitTest(pEnemyBlack, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				newShot->Die(10000);
				newShot->Delete();
				enemyBlackCurrentHealth = enemyBlackCurrentHealth - 20;


				if (enemyBlackCurrentHealth <= 0)
				{
					CSprite* deathBlack = new CSprite(pEnemyBlack->GetX(), pEnemyBlack->GetY(), 0, 0, GetTime());
					deathBlack->LoadAnimation("EnemyBlackDeathRight.bmp", "enemyblackdeath", CSprite::Sheet(8, 1).Row(0).From(0).To(7), CColor::Red());
					deathBlack->SetAnimation("enemyblackdeath");
					score = score + 80;
					deathBlack->Die(800);
					pEnemyBlack->Delete();
					blackDeathanimation.push_back(deathBlack);
				}
			}

		}

		blackDeathanimation.delete_if(deleted);

		for (CSprite* enemyShot : enemyBulletlist)
		{
			if (enemyShot->HitTest(&player, 1))
			{
				gunSounds.Play("BulletImpact.wav");
				enemyShot->Die(10000);
				enemyShot->Delete();
				currentHealth = currentHealth - 50;
			}
		}
		enemyBulletlist.delete_if(deleted);

		//if (pEnemyBlue->GetStatus() > 4) pEnemyBlue->SetYVelocity(pEnemyBlue->GetYVelocity() - slowdown);



		CVector Player;

		float dist = Distance(player.GetPosition(), pEnemyBlack->GetPosition());

		if (pEnemyBlack->GetX() < 200 && pEnemyBlack->GetXVelocity() < 0 && pEnemyBlack->GetStatus() != EATTACKRIGHT)
		{
			pEnemyBlack->SetMotion(enemyWalkSpeed, 0);
			pEnemyBlack->LoadAnimation("EnemyBlackRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
			pEnemyBlack->SetAnimation("enemywalkright");
			pEnemyBlack->SetStatus(EWALKRIGHT);
			pEnemyBlack->Update(GetTime());
		}

		if (pEnemyBlack->GetX() > 1900 && pEnemyBlack->GetXVelocity() > 0 && pEnemyBlack->GetStatus() != EATTACKLEFT)
		{
			pEnemyBlack->SetMotion(-enemyWalkSpeed, 0);
			pEnemyBlack->LoadAnimation("EnemyBlackRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
			pEnemyBlack->SetAnimation("enemywalkleft");
			pEnemyBlack->SetStatus(EWALKLEFT);
			pEnemyBlack->Update(GetTime());
		}

		if (dist < 200 && pEnemyBlack->GetStatus() == EWALKLEFT)
		{
			pEnemyBlack->SetMotion(0, 0);
			pEnemyBlack->LoadAnimation("EnemyBlackIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
			pEnemyBlack->SetAnimation("enemyidleleft");
			pEnemyBlack->SetStatus(EATTACKLEFT);
			pEnemyBlack->Update(GetTime());
		}

		if (dist < 200 && pEnemyBlack->GetStatus() == EWALKRIGHT)
		{
			pEnemyBlack->SetMotion(0, 0);
			pEnemyBlack->LoadAnimation("EnemyBlackIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
			pEnemyBlack->SetAnimation("enemyidleright");
			pEnemyBlack->SetStatus(EATTACKRIGHT);
			pEnemyBlack->Update(GetTime());
		}

		if (rand() % 500 == 0 && dist < 200 && pEnemyBlack->GetStatus() == EATTACKRIGHT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyBlack->GetX() + 30, pEnemyBlack->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
		if (rand() % 500 == 0 && dist < 200 && pEnemyBlack->GetStatus() == EATTACKLEFT)
		{
			CSprite* newEnemyShot = new CSprite(pEnemyBlack->GetX() + 30, pEnemyBlack->GetY(), "EnemyBlueBulletSprite.bmp", CColor::Black(), GetTime());
			newEnemyShot->SetMotion(-300, 0);
			gunSounds.Play("PlayerShoot.wav");
			// newEnemyShot->Die(500);
			enemyBulletlist.push_back(newEnemyShot);
			enemyCounter = 30;
		}
	}
	enemyBlack.delete_if(deleted);
	bulletList.delete_if(deleted);
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{   
	// loading the game map
	background.LoadImage("Background.bmp", "map1", CColor::Black());
	background.SetImage("map1"); background.SetPosition(1200, 360);

	// configuring the animations for the player sprite
	player.LoadAnimation("PlayerIdleRight.bmp", "standright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Blue());
	player.LoadAnimation("PlayerRunRight.bmp", "walkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Blue());
	player.LoadAnimation("PlayerJumpRight.bmp", "jumpright", CSprite::Sheet(2, 1).Row(0).From(0).To(1), CColor::Blue());
	player.LoadAnimation("PlayerIdleLeft.bmp", "standleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Blue());
	player.LoadAnimation("PlayerRunLeft.bmp", "walkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Blue());
	player.LoadAnimation("PlayerJumpLeft.bmp", "jumpleft", CSprite::Sheet(2, 1).Row(0).From(1).To(0), CColor::Blue());
	player.LoadAnimation("PlayerShootRight.bmp", "shootright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Blue());
	player.LoadAnimation("PlayerShootLeft.bmp", "shootleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Blue());


	player.SetImage("jumpright");
	player.SetStatus(6);
	currentAmmo = maxAmmo;
	score = 0;

	enemyBlackCurrentHealth = 250;
	enemyBlueCurrentHealth = 100;
	enemyRedCurrentHealth = 150;



}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{   
	playerLives = 3;

	platformList.delete_all();
	enemyBlue.delete_all();
	enemyRed.delete_all();
	enemyBlack.delete_all();
	mainMenuMusic.Play("MainMenu.wav");
	endMusic.Stop();
	levelMusic.Stop();

	if (level == 1)
	{
		SetupLevel1();
	}

	if (level == 2)
	{
		SetupLevel2();
	}

	if (level == 3)
	{
		SetupLevel3();
	}
	
	//StartGame();
}


void CMyGame::SetupLevel1()
{
	level = 1;
	enemyBlue.delete_all();
	enemyRed.delete_all();
	enemyBlack.delete_all();
	platformList.delete_all();
	player.SetPosition(100, 100);

	int type, x, y;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level1.txt", ios_base::in);

	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				CSprite* pSprite = new CSprite(x, y, "Ground.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 2)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 3)
			{
				CSprite* pSprite = new CSprite(x, y, "Wall2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 4)
			{
				CSprite* pEnemyBlue = new CSprite(x, y, 0, 0, GetTime());
				pEnemyBlue->LoadAnimation("EnemyBlueIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
				pEnemyBlue->LoadAnimation("EnemyBlueRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
				pEnemyBlue->SetAnimation("enemywalkright");
				pEnemyBlue->SetMotion(50, 0);
				enemyBlue.push_back(pEnemyBlue);
			}
			if (type == 5)
			{
				CSprite* pEnemyBlue = new CSprite(x, y, 0, 0, GetTime());
				pEnemyBlue->LoadAnimation("EnemyBlueIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
				pEnemyBlue->LoadAnimation("EnemyBlueRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
				pEnemyBlue->SetAnimation("enemywalkleft");
				pEnemyBlue->SetMotion(-50, 0);
				enemyBlue.push_back(pEnemyBlue);
			}

		}
	} while (neof);
	myfile.close();
}

// setting up a level 
void CMyGame::SetupLevel2()
{
	level = 2;
	score = 0;

	enemyBlue.delete_all();
	enemyRed.delete_all();
	enemyBlack.delete_all();
	platformList.delete_all();
	player.SetPosition(100, 100);

	int type, x, y;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level2.txt", ios_base::in);

	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				CSprite* pSprite = new CSprite(x, y, "Ground.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 2)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 3)
			{
				CSprite* pSprite = new CSprite(x, y, "Wall2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 4)
			{
				CSprite* pEnemyRed = new CSprite(x, y, 0, 0, GetTime());
				pEnemyRed->LoadAnimation("EnemyRedIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
				pEnemyRed->LoadAnimation("EnemyRedRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
				pEnemyRed->SetAnimation("enemywalkright");
				pEnemyRed->SetMotion(50, 0);
				enemyRed.push_back(pEnemyRed);
			}
			if (type == 5)
			{
				CSprite* pEnemyRed = new CSprite(x, y, 0, 0, GetTime());
				pEnemyRed->LoadAnimation("EnemyRedIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
				pEnemyRed->LoadAnimation("EnemyRedRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
				pEnemyRed->SetAnimation("enemywalkleft");
				pEnemyRed->SetMotion(-50, 0);
				enemyRed.push_back(pEnemyRed);
			}

		}
	} while (neof);
	myfile.close();
}

void CMyGame::SetupLevel3()
{
	level = 3;
	score = 0;

	enemyBlue.delete_all();
	enemyRed.delete_all();
	enemyBlack.delete_all();
	platformList.delete_all();
	player.SetPosition(100, 100);

	int type, x, y;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level3.txt", ios_base::in);

	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				CSprite* pSprite = new CSprite(x, y, "Ground.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 2)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 3)
			{
				CSprite* pSprite = new CSprite(x, y, "Wall2.bmp", CColor::Red(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 4)
			{
				CSprite* pEnemyBlack = new CSprite(x, y, 0, 0, GetTime());
				pEnemyBlack->LoadAnimation("EnemyBlackIdleRight.bmp", "enemyidleright", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Red());
				pEnemyBlack->LoadAnimation("EnemyBlackRunRight.bmp", "enemywalkright", CSprite::Sheet(6, 1).Row(0).From(0).To(5), CColor::Red());
				pEnemyBlack->SetAnimation("enemywalkright");
				pEnemyBlack->SetMotion(50, 0);
				enemyBlack.push_back(pEnemyBlack);
			}
			if (type == 5)
			{
				CSprite* pEnemyBlack = new CSprite(x, y, 0, 0, GetTime());
				pEnemyBlack->LoadAnimation("EnemyBlackIdleLeft.bmp", "enemyidleleft", CSprite::Sheet(5, 1).Row(0).From(4).To(0), CColor::Red());
				pEnemyBlack->LoadAnimation("EnemyBlackRunLeft.bmp", "enemywalkleft", CSprite::Sheet(6, 1).Row(0).From(5).To(0), CColor::Red());
				pEnemyBlack->SetAnimation("enemywalkleft");
				pEnemyBlack->SetMotion(-50, 0);
				enemyBlack.push_back(pEnemyBlack);
			}

		}
	} while (neof);
	myfile.close();
}


// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	platformList.delete_all();
	enemyBlue.delete_all();
	enemyBlack.delete_all();
	enemyRed.delete_all();

	level = 1;

	mainMenuMusic.Stop();
	endMusic.Stop();
	levelMusic.Play("LevelMusic.wav", -1);

    player.SetPosition(100, 100);
	player.SetMotion(0, 0);
	enemyCounter = 0;
	currentHealth = 100;
	score = 0;

	if (level == 1)
	{
		SetupLevel1();
	}

	if (level == 2)
	{
		SetupLevel2();
	}

	if (level == 3)
	{
		SetupLevel3();
	}

}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{	

}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_p)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	// start game with a left mouse button click
	if (IsMenuMode()) StartGame();
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
