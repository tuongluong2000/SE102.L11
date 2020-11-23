/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102
	
	SAMPLE 04 - COLLISION

	This sample illustrates how to:

		1/ Implement SweptAABB algorithm between moving objects
		2/ Implement a simple (yet effective) collision frame work

	Key functions: 
		CGame::SweptAABB
		CGameObject::SweptAABBEx
		CGameObject::CalcPotentialCollisions
		CGameObject::FilterCollision

		CGameObject::GetBoundingBox
		
================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"

#include "Mario.h"
#include "Brick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "GameMap.h"
#include "CBlock.h"
#include "Items.h"

#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"04 - Collision"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_ENEMY1 20
#define ID_TEX_ENEMY2 30
#define ID_TEX_MISC 40
#define ID_MAP 50

CGame *game;

CMario *mario;
CGoomba *goomba;
Koopa* koopa;
GameMap *map;
Items* item;

vector<LPGAMEOBJECT> objects;
vector<LPGAMEOBJECT> objectsbrick;

class CSampleKeyHander: public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler; 

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	if (mario->state != MARIO_STATE_DIE) {
		switch (KeyCode)
		{
		case DIK_SPACE:
			mario->SetState(MARIO_STATE_JUMP);
			break;
		case DIK_A: // reset
			mario->SetState(MARIO_STATE_IDLE);
			mario->LevelUp();
			mario->SetPosition(50.0f, 0.0f);
			mario->SetSpeed(0, 0);
			break;
		case DIK_S: // reset
			mario->SetState(MARIO_STATE_IDLE);
			mario->LevelDo();
			mario->SetPosition(50.0f, 0.0f);
			mario->SetSpeed(0, 0);
			break;
		case DIK_C: //hold
			mario->HoldKoopa();
			break;
		case DIK_X:
			mario->Fly();
			break;
		case DIK_Z:
			mario->Fire();
			break;

		}
	}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
	switch (KeyCode)
	{
	}
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE) return;
	if (game->IsKeyDown(DIK_Z))
	{
		mario->SetState(MARIO_STATE_FIRE);
	}
	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (game->IsKeyDown(DIK_Z))
		{
			mario->SetState(MARIO_STATE_FIRE_RIGHT);
		}
		if (game->IsKeyDown(DIK_LSHIFT))
		{
				if(mario->GetState()!= MARIO_STATE_FLY_RIGHT&& mario->GetState() != MARIO_STATE_FLY_LEFT)
				mario->SetState(MARIO_STATE_RUN_RIGHT);
		}
		else
		mario->SetState(MARIO_STATE_WALKING_RIGHT);
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{ 
		
		if (game->IsKeyDown(DIK_X))
		{
			mario->SetState(MARIO_STATE_FLY_LEFT);
		}
		if (game->IsKeyDown(DIK_LSHIFT))
		{
			if (mario->GetState() != MARIO_STATE_FLY_LEFT&& mario->GetState() != MARIO_STATE_FLY_RIGHT)
			mario->SetState(MARIO_STATE_RUN_LEFT);
		}
		else
		mario->SetState(MARIO_STATE_WALKING_LEFT);
	}
	else
		mario->SetState(MARIO_STATE_IDLE);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
	Load all game resources 
	In this example: load textures, sprites, animations and mario object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	textures->Add(ID_TEX_MARIO, L"textures\\mario.png",D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_TEX_MISC, L"textures\\misc.png", D3DCOLOR_XRGB(176, 224, 248));
	textures->Add(ID_TEX_ENEMY, L"textures\\enemies.png", D3DCOLOR_XRGB(3, 26, 110));
	textures->Add(ID_TEX_ENEMY1, L"textures\\enemies1.png", D3DCOLOR_XRGB(255,255,255));
	textures->Add(ID_TEX_ENEMY2, L"textures\\enemies2.png", D3DCOLOR_XRGB(255, 255, 255));


	textures->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));
	textures->Add(ID_MAP, L"Map\\map.png", D3DCOLOR_XRGB(176, 224, 248));



	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	LPDIRECT3DTEXTURE9 texMario = textures->Get(ID_TEX_MARIO);

	// big
	sprites->Add(10001, 246, 154, 260, 181, texMario);		// idle right

	sprites->Add(10002, 275, 154, 290, 181, texMario);		// walk
	sprites->Add(10003, 304, 154, 321, 181, texMario);

	sprites->Add(10011, 186, 154, 200, 181, texMario);		// idle left
	sprites->Add(10012, 155, 154, 170, 181, texMario);		// walk
	sprites->Add(10013, 125, 154, 140, 181, texMario);

	sprites->Add(10099, 215, 120, 231, 135, texMario);		// die 

	// small
	sprites->Add(10021, 247, 0, 259, 15, texMario);			// idle small right
	sprites->Add(10022, 275, 0, 291, 15, texMario);			// walk 
	sprites->Add(10023, 306, 0, 320, 15, texMario);			// 

	sprites->Add(10031, 187, 0, 198, 15, texMario);			// idle small left

	sprites->Add(10032, 155, 0, 170, 15, texMario);			// walk
	sprites->Add(10033, 125, 0, 139, 15, texMario);			// 

	//RACCOON
	sprites->Add(10041, 243, 634, 264, 662, texMario);		// idle right
	sprites->Add(10042, 272, 634, 294, 662, texMario);		// walk
	sprites->Add(10043, 303, 634, 324, 662, texMario);

	sprites->Add(10051, 182, 634, 203, 662, texMario);		// idle left
	sprites->Add(10052, 152, 634, 174, 662, texMario);		// walk
	sprites->Add(10053, 122, 634, 142, 662, texMario);

	sprites->Add(10151, 331, 634, 355, 662, texMario);		// run fly right
	sprites->Add(10152, 362, 634, 385, 662, texMario);		// 
	sprites->Add(10153, 393, 634, 414, 662, texMario);

	sprites->Add(10251, 91, 634, 115, 662, texMario);		// run fly left
	sprites->Add(10252, 61, 634, 84, 662, texMario);		// 
	sprites->Add(10253, 32, 634, 53, 662, texMario);
	
	sprites->Add(10351, 361, 714, 385, 742, texMario);		// Fly right
	sprites->Add(10352, 331, 714, 355, 741, texMario);
	sprites->Add(10353, 361, 754, 385, 781, texMario);
	sprites->Add(10354, 331, 714, 355, 741, texMario);

	sprites->Add(10451, 61, 714, 85, 742, texMario);		// Fly left
	sprites->Add(10452, 91, 714, 115, 741, texMario);
	sprites->Add(10453, 61, 754, 85, 781, texMario);
	sprites->Add(10454, 91, 714, 115, 741, texMario);

	//FIRE
	sprites->Add(10061, 246, 394, 260, 421, texMario);		// idle right
	sprites->Add(10062, 275, 394, 290, 421, texMario);		// walk
	sprites->Add(10063, 304, 394, 321, 421, texMario);

	sprites->Add(10071, 186, 394, 200, 421, texMario);		// idle left
	sprites->Add(10072, 155, 394, 170, 421, texMario);		// walk
	sprites->Add(10073, 125, 394, 140, 421, texMario);

	sprites->Add(10081, 240, 554, 248, 562, texMario);		//fire
	sprites->Add(10082, 258, 553, 266, 562, texMario);
	sprites->Add(10083, 258, 574, 266, 582, texMario);
	sprites->Add(10084, 240, 573, 247, 582, texMario);
	


	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 408, 225, 424, 241, texMisc);
	//items
	sprites->Add(20002, 300, 117, 316, 133, texMisc);
	sprites->Add(20003, 318, 117, 334, 133, texMisc);
	sprites->Add(20004, 336, 117, 352, 133, texMisc);
	sprites->Add(20005, 354, 117, 370, 133, texMisc);
	//Goo
	LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY);
	LPDIRECT3DTEXTURE9 texEnemy1 = textures->Get(ID_TEX_ENEMY1);
	LPDIRECT3DTEXTURE9 texEnemy2 = textures->Get(ID_TEX_ENEMY2);

	sprites->Add(30001, 5, 14, 21, 29, texEnemy);
	sprites->Add(30002, 25, 14, 41, 29, texEnemy);

	sprites->Add(30003, 45, 21, 61, 29, texEnemy); // die sprite
	//Koopa
	sprites->Add(40001, 170, 192, 186, 218, texEnemy2);//wal right
	sprites->Add(40002, 187, 192, 203, 218, texEnemy2);
	sprites->Add(40011, 222, 192, 238, 218, texEnemy2);//wal left
	sprites->Add(40012, 205, 192, 221, 218, texEnemy2);

	sprites->Add(40021, 152, 202, 168, 218, texEnemy2);//hold

	sprites->Add(40031, 256, 202, 272, 218, texEnemy2);//die run
	sprites->Add(40032, 273, 202, 289, 218, texEnemy2);
	sprites->Add(40033, 290, 202, 306, 218, texEnemy2);

	sprites->Add(40003, 239, 202, 255, 218, texEnemy2); // koopa die 

	//star
	sprites->Add(50001, 303, 99, 313, 115, texMisc);
	sprites->Add(50002, 322, 99, 330, 115, texMisc);
	sprites->Add(50003, 341, 99, 347, 115, texMisc);

	//mushroom
	sprites->Add(60001, 300, 189, 316, 205, texMisc);

	//Items die
	sprites->Add(70001, 372, 117, 388, 133, texMisc);

	// leaf
	sprites->Add(80001, 300, 208, 316, 222, texMisc);


	// map
	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_MAP);
	// LOAD BLOCK GROUND duong di tren
	sprites->Add(0, 96, 160, 112, 176, texMap);
	sprites->Add(1, 112, 160, 128, 176, texMap);
	sprites->Add(2, 128, 160, 144, 176, texMap);

	// LOAD BLOCK BOX gach bay
	sprites->Add(3, 16, 64, 32, 80, texMap);

	// LOAD BLOCK PIPE miệng cống
	sprites->Add(4, 112, 0, 128, 16, texMap);
	sprites->Add(5, 128, 0, 144, 16, texMap);
	//than cong
	sprites->Add(6, 160, 0, 176, 16, texMap);
	sprites->Add(7, 0, 16, 16, 32, texMap);

// LOAD BLOCK 1 SIDE
// LOAD BLOCK GREEN dau gach mau xanh
	sprites->Add(9, 128, 32, 144, 48, texMap);
	sprites->Add(10, 144, 32, 160, 48, texMap);
	sprites->Add(11, 160, 32, 176, 48, texMap);

	// LOAD BLOCK PINK dau gach mau cam
	sprites->Add(12, 16, 16, 32, 32, texMap);
	sprites->Add(13, 32, 16, 48, 32, texMap);
	sprites->Add(14, 48, 16, 64, 32, texMap);

	// LOAD BLOCK BLUE dau gach xanh duong
	sprites->Add(15, 112, 64, 128, 80, texMap);
	sprites->Add(16, 128, 64, 144, 80, texMap);
	sprites->Add(17, 144, 64, 160, 80, texMap);

	// LOAD BLOCK WHITE gach mau trang
	sprites->Add(18, 128, 48, 144, 64, texMap);
	sprites->Add(19, 144, 48, 160, 64, texMap);
	sprites->Add(20, 160, 48, 176, 64, texMap);


	
	LPANIMATION ani;
	
	ani = new CAnimation(100);	// idle big right
	ani->Add(10001);
	animations->Add(400, ani);

	ani = new CAnimation(100);	// idle big left
	ani->Add(10011);
	animations->Add(401, ani);

	ani = new CAnimation(100);	// idle small right
	ani->Add(10021);
	animations->Add(402, ani);

	ani = new CAnimation(100);	// idle small left
	ani->Add(10031);
	animations->Add(403, ani);

	ani = new CAnimation(100);	// idle raccoon right
	ani->Add(10041);
	animations->Add(404, ani);

	ani = new CAnimation(100);	// idle raccoon left
	ani->Add(10051);
	animations->Add(405, ani);

	ani = new CAnimation(100);	// raccoon run fly right
	ani->Add(10151);
	ani->Add(10152);
	ani->Add(10153);
	animations->Add(411, ani);

	ani = new CAnimation(100);	// raccoon run fly left
	ani->Add(10251);
	ani->Add(10252);
	ani->Add(10253);
	animations->Add(412, ani);

	ani = new CAnimation(100);	// raccoon fly right
	ani->Add(10351);
	ani->Add(10352);
	ani->Add(10353);
	ani->Add(10354);
	animations->Add(413, ani);

	ani = new CAnimation(100);	// raccoon fly left
	ani->Add(10451);
	ani->Add(10452);
	ani->Add(10453);
	ani->Add(10454);
	animations->Add(414, ani);

	ani = new CAnimation(100);	// idle fire right
	ani->Add(10061);
	animations->Add(406, ani);

	ani = new CAnimation(100);	// idle fire left
	ani->Add(10071);
	animations->Add(407, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	animations->Add(500, ani);

	ani = new CAnimation(100);	// // walk left big
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	animations->Add(501, ani);

	ani = new CAnimation(100);	// walk right small
	ani->Add(10021);
	
	ani->Add(10023);
	
	animations->Add(502, ani);

	ani = new CAnimation(100);	// walk left small
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	ani->Add(10031);
	ani->Add(10032);
	ani->Add(10033);
	animations->Add(503, ani);

	ani = new CAnimation(100);	// walk right raccoon
	ani->Add(10041);
	ani->Add(10042);
	ani->Add(10043);
	animations->Add(504, ani);

	ani = new CAnimation(100);	// // walk left raccoon
	ani->Add(10051);
	ani->Add(10052);
	ani->Add(10053);
	animations->Add(505, ani);

	ani = new CAnimation(100);	// walk right fire
	ani->Add(10061);
	ani->Add(10062);
	ani->Add(10063);
	animations->Add(506, ani);

	ani = new CAnimation(100);	// // walk left fire
	ani->Add(10071);
	ani->Add(10072);
	ani->Add(10073);
	animations->Add(507, ani);

	ani = new CAnimation(100);	//  fire
	ani->Add(10081);
	ani->Add(10082);
	ani->Add(10083);
	ani->Add(10084);
	animations->Add(508, ani);


	ani = new CAnimation(100);		// Mario die
	ani->Add(10099);
	animations->Add(599, ani);

	

	ani = new CAnimation(100);		// brick
	ani->Add(20001);
	animations->Add(601, ani);

	ani = new CAnimation(300);		// Goomba walk
	ani->Add(30001);
	ani->Add(30002);
	animations->Add(701, ani);

	ani = new CAnimation(1000);		// Goomba dead
	ani->Add(30003);
	animations->Add(702, ani);

	//koopa
	ani = new CAnimation(300);	//	right
	ani->Add(40001);
	ani->Add(40002);
	animations->Add(901, ani);

	ani = new CAnimation(300); //left
	ani->Add(40011);
	ani->Add(40012);
	animations->Add(902, ani);

	ani = new CAnimation(1000);		// Koopa dead
	ani->Add(40003);
	animations->Add(903, ani);

	ani = new CAnimation(1000);		// Koopa hold
	ani->Add(40021);
	animations->Add(904, ani);

	ani = new CAnimation(300); //die run
	ani->Add(40031);
	ani->Add(40032);
	ani->Add(40033);
	animations->Add(905, ani);

	//Item
	ani = new CAnimation(100);	
	ani->Add(20002);
	ani->Add(20003);
	ani->Add(20004);
	ani->Add(20005);
	animations->Add(801, ani);

	//star
	ani = new CAnimation(100);
	ani->Add(50001);
	ani->Add(50002);
	ani->Add(50003);
	animations->Add(802, ani);

	//mushroom
	ani = new CAnimation(100);
	ani->Add(60001);
	animations->Add(803, ani);

	//items die
	ani = new CAnimation(100);
	ani->Add(70001);
	animations->Add(804, ani);

	//leaf
	ani = new CAnimation(100);
	ani->Add(80001);
	animations->Add(805, ani);

	//ani map duong di 
	ani = new CAnimation(100);
	ani->Add(0);
	animations->Add(0, ani);

	ani = new CAnimation(100);
	ani->Add(1);
	animations->Add(1, ani);

	ani = new CAnimation(100);
	ani->Add(2);
	animations->Add(2, ani);

	// ani map gach bay 
	ani = new CAnimation(100);
	ani->Add(3);
	animations->Add(3, ani);


	//cong
	ani = new CAnimation(100);
	ani->Add(4);
	animations->Add(4, ani);

	ani = new CAnimation(100);
	ani->Add(5);
	animations->Add(5, ani);

	ani = new CAnimation(100);
	ani->Add(6);
	animations->Add(6, ani);

	ani = new CAnimation(100);
	ani->Add(7);
	animations->Add(7, ani);

	ani = new CAnimation(100);
	ani->Add(9);
	ani->Add(10);
	ani->Add(11);
	animations->Add(9, ani);

	ani = new CAnimation(100);
	ani->Add(12);
	ani->Add(13);
	ani->Add(14);
	animations->Add(12, ani);

	ani = new CAnimation(100);
	ani->Add(15);
	ani->Add(16);
	ani->Add(17);
	animations->Add(15, ani);

	ani = new CAnimation(100);
	ani->Add(18);
	ani->Add(19);
	ani->Add(20);
	animations->Add(18, ani);

	
	
	map = new GameMap(ID_MAP, "Map/map1-1.txt");


	mario = new CMario();
	mario->AddAnimation(400);		// idle right big
	mario->AddAnimation(401);		// idle left big
	mario->AddAnimation(402);		// idle right small
	mario->AddAnimation(403);		// idle left small
	mario->AddAnimation(404);       // right raccoon
	mario->AddAnimation(405);		// left raccoon
	mario->AddAnimation(406);		//fire right
	mario->AddAnimation(407);		// fire left

	mario->AddAnimation(500);		// walk right big
	mario->AddAnimation(501);		// walk left big
	mario->AddAnimation(502);		// walk right small
	mario->AddAnimation(503);		// walk left big
	mario->AddAnimation(504);		// walk right raccoon
	mario->AddAnimation(505);		// walk left 
	mario->AddAnimation(506);		// fire right
	mario->AddAnimation(507);		// fire left

	mario->AddAnimation(599);		// die
	mario->AddAnimation(411);		// run fly right
	mario->AddAnimation(412);		// run fly left
	mario->AddAnimation(413);		//  fly right
	mario->AddAnimation(414);		//  fly left
	mario->AddAnimation(508);		//fire

	mario->SetPosition(50.0f, 0);
	objects.push_back(mario);


	// đường đi
	//1
	CBrick* brick1 = new CBrick();
	brick1->AddAnimation(0);
	brick1->SetPosition(0 , 416.0f);
	objects.push_back(brick1);
	for (int i = 0; i < 37; i++)
	{
		CBrick* brick1 = new CBrick();
		brick1->AddAnimation(1);
		brick1->SetPosition(16 + i * 16.0f, 416.0f);
		objects.push_back(brick1);
		
	}
	CBrick* brick2 = new CBrick();
	brick2->AddAnimation(2);
	brick2->SetPosition(608, 416.0f);
	objects.push_back(brick2);
	//2
	brick1 = new CBrick();
	brick1->AddAnimation(0);
	brick1->SetPosition(624, 400.0f);
	objects.push_back(brick1);
	for (int i = 0; i < 28; i++)
	{
		CBrick* brick1 = new CBrick();
		brick1->AddAnimation(1);
		brick1->SetPosition(640 + i * 16.0f, 400.0f);
		objects.push_back(brick1);

	}
	brick2 = new CBrick();
	brick2->AddAnimation(2);
	brick2->SetPosition(1072, 400.0f);
	objects.push_back(brick2);
	//3
	brick1 = new CBrick();
	brick1->AddAnimation(0);
	brick1->SetPosition(1152, 416.0f);
	objects.push_back(brick1);
	for (int i = 0; i < 20; i++)
	{
		CBrick* brick1 = new CBrick();
		brick1->AddAnimation(1);
		brick1->SetPosition(1168 + i * 16.0f, 416.0f);
		objects.push_back(brick1);

	}
	brick2 = new CBrick();
	brick2->AddAnimation(2);
	brick2->SetPosition(1488, 416.0f);
	objects.push_back(brick2);
	//4
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		CBrick* brick2 = new CBrick();
		brick->AddAnimation(0);
		brick1->AddAnimation(1);
		brick2->AddAnimation(2);
		brick->SetPosition(1536 + i * 48.0f, 416.0f);
		brick1->SetPosition(1552 + i * 48.0f, 416.0f);
		brick2->SetPosition(1568 + i * 48.0f, 416.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
		objects.push_back(brick2);
	}
	//5
	brick1 = new CBrick();
	brick1->AddAnimation(0);
	brick1->SetPosition(1664, 416.0f);
	objects.push_back(brick1);
	for (int i = 0; i < 34; i++)
	{
		CBrick* brick1 = new CBrick();
		brick1->AddAnimation(1);
		brick1->SetPosition(1680 + i * 16.0f, 416.0f);
		objects.push_back(brick1);

	}
	brick2 = new CBrick();
	brick2->AddAnimation(2);
	brick2->SetPosition(2224, 416.0f);
	objects.push_back(brick2);
	//6
	brick1 = new CBrick();
	brick1->AddAnimation(0);
	brick1->SetPosition(2256, 416.0f);
	objects.push_back(brick1);
	for (int i = 0; i < 32; i++)
	{
		CBrick* brick1 = new CBrick();
		brick1->AddAnimation(1);
		brick1->SetPosition(2272 + i * 16.0f, 416.0f);
		objects.push_back(brick1);

	}
	brick2 = new CBrick();
	brick2->AddAnimation(2);
	brick2->SetPosition(2800, 416.0f);
	objects.push_back(brick2);

	// gạch tren cao
	
	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(3);
		brick->SetPosition(1504 + i * 16.0f, 336);
		objects.push_back(brick);

	}

	for (int i = 0; i < 3; i++)
	{
		int j = 2;
		while (j >= i)
		{
			CBrick* brick = new CBrick();
			brick->AddAnimation(3);
			brick->SetPosition(1568 + j * 16.0f, 400- i*16);
			objects.push_back(brick);
			j--;
		}

	}

	for (int i = 0; i < 3; i++)
	{
		int j = 0;
		while (j <= i)
		{
			CBrick* brick = new CBrick();
			brick->AddAnimation(3);
			brick->SetPosition(1664 + j * 16.0f, 368 + i * 16);
			objects.push_back(brick);
			j++;
		}

	}

	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(3);
		brick->SetPosition(2256 + i * 16.0f, 304);
		objects.push_back(brick);

	}

	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(3);
		brick->SetPosition(2256 + i * 16.0f, 368);
		objects.push_back(brick);

	}

	// block gach cam

	//1
	CBlock* block = new CBlock(12, 1);
	block->SetPosition(240, 368);
	objectsbrick.push_back(block);
	for (int i = 0; i < 1; i++) {
		block = new CBlock(13, 1);
		block->SetPosition(256 + i * 16, 368);
		objectsbrick.push_back(block);
	}
	block = new CBlock(14, 1);
	block->SetPosition(272, 368);
	objectsbrick.push_back(block);
	//2
	block = new CBlock(12, 1);
	block->SetPosition(464, 336);
	objectsbrick.push_back(block);
	for (int i = 0; i < 2; i++) {
		block = new CBlock(13, 1);
		block->SetPosition(480 + i * 16, 336);
		objectsbrick.push_back(block);
	}
	block = new CBlock(14, 1);
	block->SetPosition(512, 336);
	objectsbrick.push_back(block);
	//3
	block = new CBlock(12, 1);
	block->SetPosition(1296, 352);
	objectsbrick.push_back(block);
	for (int i = 0; i < 5; i++) {
		block = new CBlock(13, 1);
		block->SetPosition(1312 + i * 16, 352);
		objectsbrick.push_back(block);
	}
	block = new CBlock(14, 1);
	block->SetPosition(1392, 352);
	objectsbrick.push_back(block);
	//4
	block = new CBlock(12, 1);
	block->SetPosition(2144, 368);
	objectsbrick.push_back(block);
	for (int i = 0; i < 1; i++) {
		block = new CBlock(13, 1);
		block->SetPosition(2160 + i * 16, 368);
		objectsbrick.push_back(block);
	}
	block = new CBlock(14, 1);
	block->SetPosition(2176, 368);
	objectsbrick.push_back(block);

	// block gach xanh duong
	//1
	block = new CBlock(15, 1);
	block->SetPosition(272, 336);
	objectsbrick.push_back(block);
	for (int i = 0; i < 1; i++) {
		block = new CBlock(16, 1);
		block->SetPosition(288 + i * 16, 336);
		objectsbrick.push_back(block);
	}
	block = new CBlock(17, 1);
	block->SetPosition(304, 336);
	objectsbrick.push_back(block);
	//2
	block = new CBlock(15, 1);
	block->SetPosition(1328, 320);
	objectsbrick.push_back(block);
	for (int i = 0; i < 5; i++) {
		block = new CBlock(16, 1);
		block->SetPosition(1344 + i * 16, 320);
		objectsbrick.push_back(block);
	}
	block = new CBlock(17, 1);
	block->SetPosition(1424, 320);
	objectsbrick.push_back(block);

	//Block gach xanh la
	//1
	block = new CBlock(9, 1);
	block->SetPosition(400, 368);
	objectsbrick.push_back(block);
	for (int i = 0; i < 3; i++) {
		block = new CBlock(10, 1);
		block->SetPosition(416 + i * 16, 368);
		objectsbrick.push_back(block);
	}
	block = new CBlock(11, 1);
	block->SetPosition(464, 368);
	objectsbrick.push_back(block);
	//2
	block = new CBlock(9, 1);
	block->SetPosition(512, 384);
	objectsbrick.push_back(block);
	for (int i = 0; i < 4; i++) {
		block = new CBlock(10, 1);
		block->SetPosition(528 + i * 16, 384);
		objectsbrick.push_back(block);
	}
	block = new CBlock(11, 1);
	block->SetPosition(592, 384);
	objectsbrick.push_back(block);
	//3
	block = new CBlock(9, 1);
	block->SetPosition(1264, 384);
	objectsbrick.push_back(block);
	for (int i = 0; i < 5; i++) {
		block = new CBlock(10, 1);
		block->SetPosition(1280 + i * 16, 384);
		objectsbrick.push_back(block);
	}
	block = new CBlock(11, 1);
	block->SetPosition(1360, 384);
	objectsbrick.push_back(block);
	//4
	block = new CBlock(9, 1);
	block->SetPosition(2176, 272);
	objectsbrick.push_back(block);
	for (int i = 0; i < 1; i++) {
		block = new CBlock(10, 1);
		block->SetPosition(2192 + i * 16, 272);
		objectsbrick.push_back(block);
	}
	block = new CBlock(11, 1);
	block->SetPosition(2208, 272);
	objectsbrick.push_back(block);

	//Block gach trang
	block = new CBlock(18, 1);
	block->SetPosition(512, 304);
	objectsbrick.push_back(block);
	for (int i = 0; i < 2; i++) {
		block = new CBlock(19, 1);
		block->SetPosition(528 + i * 16, 304);
		objectsbrick.push_back(block);
	}
	block = new CBlock(20, 1);
	block->SetPosition(560, 304);
	objectsbrick.push_back(block);

	//brick cong
	// 1
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(4);
		brick1->AddAnimation(5);
		brick->SetPosition(352.0f , 368.0f);
		brick1->SetPosition(368.0f, 368.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(352.0f, 384.0f+i*16.0f);
		brick1->SetPosition(368.0f, 384.0f+i*16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}
	//2
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(4);
		brick1->AddAnimation(5);
		brick->SetPosition(1792.0f, 384.0f);
		brick1->SetPosition(1808.0f, 384.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(1792.0f, 400.0f + i * 16.0f);
		brick1->SetPosition(1808.0f, 400.0f + i * 16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}
	//3
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(4);
		brick1->AddAnimation(5);
		brick->SetPosition(1856.0f, 368.0f);
		brick1->SetPosition(1872.0f, 368.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(1856.0f, 384.0f + i * 16.0f);
		brick1->SetPosition(1872.0f, 384.0f + i * 16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}
	//4
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(4);
		brick1->AddAnimation(5);
		brick->SetPosition(2256.0f, 112.0f);
		brick1->SetPosition(2272.0f, 112.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 11; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(2256.0f, 128.0f + i * 16.0f);
		brick1->SetPosition(2272.0f, 128.0f + i * 16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 2; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(2256.0f, 384.0f + i * 16.0f);
		brick1->SetPosition(2272.0f, 384.0f + i * 16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}
	//5
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(4);
		brick1->AddAnimation(5);
		brick->SetPosition(2320.0f, 384.0f);
		brick1->SetPosition(2336.0f, 384.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		CBrick* brick1 = new CBrick();
		brick->AddAnimation(6);
		brick1->AddAnimation(7);
		brick->SetPosition(2320.0f, 400.0f + i * 16.0f);
		brick1->SetPosition(2336.0f, 400.0f + i * 16.0f);
		objects.push_back(brick);
		objects.push_back(brick1);
	}

	//Items
	//1
	

	for (int i = 0; i < 2; i++)
	{
		item = new Items(1-i,0);
		item->AddAnimation(801);
		item->AddAnimation(802);
		item->AddAnimation(803);
		item->AddAnimation(804);
		item->SetPosition(176 , 352);
		item->SetState(ITEMS_STATE_WALKING);
		objects.push_back(item);
	}
	//
	for (int i = 0; i < 2; i++)
	{
		item = new Items(1-i, 0);
		item->AddAnimation(801);
		item->AddAnimation(802);
		item->AddAnimation(803);
		item->AddAnimation(804);
		item->SetPosition(192, 352);
		item->SetState(ITEMS_STATE_WALKING);
		objects.push_back(item);
	}

	//2
	for (int i = 0; i < 2; i++)
	{
		item = new Items(1 - i, 0);
		item->AddAnimation(801);
		item->AddAnimation(802);
		item->AddAnimation(803);
		item->AddAnimation(804);
		item->AddAnimation(805);
		item->SetPosition(224, 304);
		item->SetState(ITEMS_STATE_WALKING);
		objects.push_back(item);
	}
	for (int i = 0; i < 2; i++)
	{
		item = new Items(1 - i, 1-i);
		item->AddAnimation(801);
		item->AddAnimation(802);
		item->AddAnimation(803);
		item->AddAnimation(804);
		item->AddAnimation(805);
		item->SetPosition(240, 304);
		item->SetState(ITEMS_STATE_WALKING);
		objects.push_back(item);
	}
	//3
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(801);
		brick->SetPosition(416 + i * 16, 320);
		objects.push_back(brick);
	}
	//4
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(801);
		brick->SetPosition(656 + i * 16, 384);
		objects.push_back(brick);
	}
	//5
	for (int i = 0; i < 1; i++)
	{
		CBrick* brick = new CBrick();
		brick->AddAnimation(801);
		brick->SetPosition(704 + i * 16, 352);
		objects.push_back(brick);
	}

	// and Goombas 
	for (int i = 0; i < 1; i++)
	{
		goomba = new CGoomba(0);
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(224 + i*60, 400);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}
	//2
	for (int i = 0; i < 1; i++)
	{
		goomba = new CGoomba(1);
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(544 + i * 60, 400);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}
	//3
	for (int i = 0; i < 2; i++)
	{
		goomba = new CGoomba(2);
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(832 + i * 48, 383);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}

	//Koopa
	for (int i = 0; i < 1; i++)
	{
		koopa = new Koopa(1);
		koopa->AddAnimation(901);
		koopa->AddAnimation(902);
		koopa->AddAnimation(903);
		koopa->AddAnimation(904);
		koopa->AddAnimation(905);
		koopa->SetPosition(576 + i * 48, 359);
		koopa->SetState(KOOPA_STATE_WALKING_RIGHT);
		objects.push_back(koopa);
	}

}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);// thêm từ 1 vào 
	}
	for (int i = 0; i < objectsbrick.size(); i++)
	{
		coObjects.push_back(objectsbrick[i]);// thêm từ 1 vào 
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt,&coObjects);
	}


	// Update camera to follow mario
	float cx, cy;
	mario->GetPosition(cx, cy);
	CGame* game = CGame::GetInstance();
	cx -= game->GetScreenWidth() / 2;
	cy -= game->GetScreenHeight() / 2;
	if (cy < 0) cy = 0;
	if (cy > map->getMapHeight() - game->GetScreenHeight()) cy = map->getMapHeight() - game->GetScreenHeight();
	if (cx < 0) cx = 0;
	if (cx > map->getMapWidth() - game->GetScreenWidth()) cx = map->getMapWidth() - game->GetScreenWidth();
	CGame::GetInstance()->SetCamPos(cx, cy);
}

/*
	Render a frame 
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
		map->Render();

		for (int i = 0; i < objectsbrick.size(); i++)
			objectsbrick[i]->Render();
		for (int i = 0; i < objects.size(); i++)
			objects[i]->Render();
		
		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();
			
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);	
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
 {
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);


	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}