#ifndef StormForge_Matt_HUD
#define StormForge_Matt_HUD

#include "TextureMgr.h"
#include "Direct3D.h"
#include "Game.h"
#include "humanoid.h"
#include <stdio.h>
#include "d3dxfont.h"

#define HALF_PI 1.5708f

class HUD
{
	static int iHudTexture;
	static int iHealthBar;
	static int iChargeBar;
	static int iQuiver;
	static int iFrost;
	static int iFlame;
public:

	static float fX, fY;

public:
	 static void Initialize(void)
	 {
			 iHudTexture = TextureMgr::LoadTexture("textures\\hudleaf.png");
			 iHealthBar = TextureMgr::LoadTexture("textures\\hudbar_health.png");
			 iChargeBar = TextureMgr::LoadTexture("textures\\hudbar_charge.png");
			 iQuiver = TextureMgr::LoadTexture("textures\\hud_quiver.png");
			 iFrost = TextureMgr::LoadTexture("textures\\hud_frost.png", FloatColor(0,0,0,1));
			 iFlame = TextureMgr::LoadTexture("textures\\hud_flame.png", FloatColor(0,0,0,1));

			 // Place the hud
				fX = Direct3D::GetScreenWidth() - TextureMgr::GetBmpWidth(iHudTexture);
				fY = Direct3D::GetScreenHeight() - TextureMgr::GetBmpHeight(iHudTexture);

	 }


	 static void Draw(int iPlayer = 1)
	 {
			// Get values
			int iHP = Game::pPlayer1->GetHP();
			int iMHP = Game::pPlayer1->GetMaxHP();
			float fHP = (float(iHP) / float(iMHP)) * 0.7f;
			float fCPct = Game::pPlayer1->GetChargePct() * 0.7f;

			//int iHP2 = Game::pPlayer2->GetHP();
			//int iMHP2 = Game::pPlayer2->GetMaxHP();
			//float fCPct2 = Game::pPlayer2->GetChargePct();

			if (iPlayer == 1)
			{
				
				// Draw hud background
				TextureMgr::Draw(iHudTexture, fX, fY);

				// Hud position, - 30 (offset), - (size of bar, so it grows up, not down)

				// Health Bar
				float fYBarPos = fY + 110 - (TextureMgr::GetBmpHeight(iHealthBar) * fHP);
				TextureMgr::Draw(iHealthBar, fX + 45, fYBarPos, NULL, 1.0f, fHP);
				
				// Charge bar
				fYBarPos = fY + 110 - (TextureMgr::GetBmpHeight(iChargeBar) * fCPct);
				TextureMgr::Draw(iChargeBar, fX + 68, fYBarPos, NULL, 1.0f, fCPct);

				// Quiver
				float fQX = fX + 13;
				float fQY = fY + 60;
				TextureMgr::Draw(iQuiver, fQX, fQY, NULL, 0.5f, 0.5f);

				// Fire/frost
				Humanoid *pHuman = (Humanoid*)Game::pPlayer1;
				if (pHuman->GetSelectedArrow() == 1)
					TextureMgr::Draw(iFlame, fQX, fQY + 20, NULL, 0.75f, 0.75f);
				else if (pHuman->GetSelectedArrow()  == 2)
					TextureMgr::Draw(iFrost, fQX, fQY + 20, NULL, 0.75f, 0.75f);
				
			}

	 }


};		// end class HUD

#endif 