#pragma once
#include "GameObject.h"
#include "Textures.h"


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define TILE_ZISE_X 11
#define TILE_ZISE_Y 12

#define MAX_MAP_Y 27
#define MAX_MAP_X 176

#define MAX_TILES 150

typedef struct Map
{
	int start_x;
	int start_y;

	int MAP_X;
	int MAP_Y;

	int tile[MAX_MAP_Y][MAX_MAP_X];
	char* file_name;
};

#define GAME_MAP_H


class GameMap : public CGameObject
{
public: 
	GameMap(int TileSetID, char* name) 
	{
		this->game_map.file_name = "";
		this->game_map.start_x = 0;
		this->game_map.start_y = 0;
		this->game_map.MAP_X = 0;
		this->game_map.MAP_Y = 0;
		for (int i = 0; i < MAX_MAP_Y; i++)
		{
			for (int j = 0; j < MAX_MAP_X; j++)
			{
				this->game_map.tile[i][j] = NULL;
			}
		}
		
		TileSetImg = CTextures::GetInstance()->Get(TileSetID);
		LoadMap(name);
	}
	~GameMap() { ; }

	void LoadMap(char* name);
	//void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL) ;
	void DrawTiles(int val, float x, float y);
	void Render() ;
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) { return; }
	float getMapWidth();
	float getMapHeight();
	
private: 
	Map game_map;
	LPDIRECT3DTEXTURE9 TileSetImg;
	int TotalRowsOfMap, TotalColumnsOfMap;
	vector<int> datamap;

};



