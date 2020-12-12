
#include "Game.h"
#include "GameMap.h"
#include "Textures.h"
#include <iostream>
#include <fstream>
#include "Utils.h"



void GameMap::LoadMap(LPCWSTR mappath)
{

	ifstream f;
	f.open(mappath);
	int i = 0;
	char str[MAX_LINE];
	while (f.getline(str, MAX_LINE))
	{
		string line(str);
		vector<string> tokens = split(line);
		if (tokens.size() < 2) return;
		for (int j = 0; j < tokens.size(); j++)
		{
			game_map.tile[i][j] = atoi(tokens[j].c_str());
			int val = atoi(tokens[j].c_str());
			DebugOut(L"[INFO] val: %d\n", val);
			if (!(val > 0))
				return;
		}
		if (game_map.MAP_X < tokens.size()) game_map.MAP_X = tokens.size();
		i++;
		game_map.MAP_Y = i;
		DebugOut(L"[INFO] map y: %d\n", game_map.MAP_Y);
		DebugOut(L"[INFO] map x: %d\n", game_map.MAP_X);
	}

		/*	for (int i = 0; i < MAX_MAP_Y; i++)
			{
				for (int j = 0; j < MAX_MAP_X; j++)
				{
					fscanf_s(f, "%d", &game_map.tile[i][j]);
					int val = game_map.tile[i][j];

					if (val != NULL)
					{
						datamap.push_back(val);
					}
				}
			}
			game_map.MAP_X = (game_map.MAP_X + 1) * TILE_ZISE_X;
			game_map.MAP_Y = (game_map.MAP_Y + 1) * TILE_ZISE_Y;
			*/

	f.close();
}



void GameMap::DrawTiles(int val, float x, float y)
{
	float x0, y0;

	x0 = int((val - 1) % TILE_ZISE_X) * 16;
	y0 = int((val - 1) / TILE_ZISE_X) * 16;

	CGame::GetInstance()->Draw(x, y, TileSetImg, x0, y0, x0 + 16, y0 + 16);
}




void GameMap::Render()
{
	float x0, y0;
	for (int i = 0; i < game_map.MAP_Y; i++)
	{
		for (int j = 0; j < game_map.MAP_X; j++)
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