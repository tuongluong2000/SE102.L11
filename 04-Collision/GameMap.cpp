
#include "Game.h"
#include "GameMap.h"
#include "Textures.h"


void GameMap::LoadMap(char* name)
{
	FILE* fp = NULL;
	fopen_s(&fp, name, "rb");
	if (fp == NULL)
	{
		return;
	};

	for (int i = 0; i < MAX_MAP_Y; i++)
	{
		for (int j = 0; j < MAX_MAP_X; j++)
		{
			fscanf_s(fp, "%d", &game_map.tile[i][j]);
			int val = game_map.tile[i][j];

			if (val != NULL)
			{
				datamap.push_back(val);
			}
		}
	}
	game_map.MAP_X = (game_map.MAP_X + 1) * TILE_ZISE_X;
	game_map.MAP_Y = (game_map.MAP_Y + 1) * TILE_ZISE_Y;

	game_map.file_name = name;
	fclose(fp);
}



void GameMap::DrawTiles(int val, float x, float y  )
{
	float x0,y0;

	x0 = int((val - 1) % TILE_ZISE_X) * 16;
	y0 = int((val - 1) / TILE_ZISE_X) * 16;

	CGame::GetInstance()->Draw(x, y, TileSetImg, x0, y0, x0 + 16, y0 + 16);
}
	



void GameMap::Render() 
{
	float x0, y0;
	for (int i = 0; i < MAX_MAP_Y; i ++)
	{
		for (int j = 0; j < MAX_MAP_X; j ++)
		{
			

			x0 = j * 16;
			y0 = i * 16;
			int val = game_map.tile[i][j];
			DrawTiles(val, x0, y0);
		}
	}

}
float GameMap::getMapWidth() {
	return MAX_MAP_X * TILE_WIDTH;
}

float GameMap::getMapHeight() {
	return MAX_MAP_Y * TILE_HEIGHT;
}