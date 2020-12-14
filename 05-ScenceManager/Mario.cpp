#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Portal.h"

#include "Goomba.h"
#include "Koopas.h"
#include "Brick.h"
#include "CBlock.h"
#include "Items.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);
	
		for (int i = 0; i < bullets.size(); i++) {
			if (bullets[i]->GetDie() == true)
				bullets.erase(bullets.begin() + i);
			else
				bullets[i]->Update(dt, coObjects);
		}
	if (run == false || (run == true && runwalk == false))
	{
		if (nx == 1)
		{
			if (runwalk == true)
			{
				if (vx < MARIO_SPEED_MAX)
					vx += MARIO_ACCELERATION_X * dx;
				else vx = MARIO_SPEED_MAX;
				DebugOut(L"[INFO] vx: %d\n", vx);
			}
			else
			{
				if (vx != 0)
				{
					vx -= MARIO_ACCELERATION_X * dx;
					if (vx <= MARIO_WALKING_SPEED)
					{
						vx = 0;
						SetState(MARIO_STATE_IDLE);
					}
				}

			}
		}
		else if (nx == -1)
		{
			if (runwalk == true)
			{
				if (vx > -MARIO_SPEED_MAX)
				{
					vx = vx + MARIO_ACCELERATION_X * dx;
				}
				else vx = -MARIO_SPEED_MAX;
				DebugOut(L"[INFO] vx: %d\n", vx);
			}
			else
			{
				if (vx != 0)
				{
					vx -= MARIO_ACCELERATION_X * dx;
					if (vx >= -MARIO_WALKING_SPEED)
					{
						vx = 0;
						SetState(MARIO_STATE_IDLE);
					}
				}
			}
		}
	}
	else if (run == true && runwalk == true )
	{
		if (GetState() == MARIO_STATE_WALKING_RIGHT)
		{

			if (vx < MARIO_SPEED_RUN_MAX)
				vx += MARIO_ACCELERATION_X * dx;
			else vx = MARIO_SPEED_RUN_MAX;
			DebugOut(L"[INFO] vx: %d\n", vx);
		}
		if (GetState() == MARIO_STATE_WALKING_LEFT)
		{
			if (vx > -MARIO_SPEED_RUN_MAX && vx < 0)
			{
				vx = vx + MARIO_ACCELERATION_X * dx;
			}
			else vx = -MARIO_SPEED_RUN_MAX;
			DebugOut(L"[INFO] vx: %d\n", vx);
		}
	}
	//hold koopa
	if (this->koopa != NULL)
	{
		if (hold == true && Runhold == true && this->koopa->GetState() == KOOPA_STATE_DIE_HOLD)
		{
			if (level != MARIO_LEVEL_RACCOON)
			{
				if (nx == 1)
					this->koopa->SetPosition(x + 18.0f, y + 3.0f);
				else if (nx == -1)
					this->koopa->SetPosition(x - 18.0f, y + 3.0f);
			}
			else
			{
				if (nx == 1)
					this->koopa->SetPosition(x + 22.0f, y + 3.0f);
				else if (nx == -1)
					this->koopa->SetPosition(x - 18.0f, y + 3.0f);
			}
		}
		if ((Runhold == false && hold == false && this->koopa->GetState() == KOOPA_STATE_DIE_HOLD) || run ==false)
		{
			if (nx == 1)
				this->koopa->SetState(KOOPA_STATE_DIE_RUN_RIGHT);
			else if (nx == -1)
				this->koopa->SetState(KOOPA_STATE_DIE_RUN_LEFT);
			koopa = NULL;
		}

	}

	// Simple fall down
	if (GetTickCount()- fly >= MARIO_FLY_GRAVITY_TIME )
		vy += MARIO_GRAVITY * dt;

	if (vx == 0 && GetState()!= MARIO_STATE_DIE)
		SetState(MARIO_STATE_IDLE);

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed // bat tu
	if (GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{

		x += dx;
		y += dy;
		this->ny = 1;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float y0, x0;
		x0 = x;
		y0 = y;


		x = x0 + min_tx * dx + nx * 0.4f;
		y = y0 + min_ty * dy + ny * 0.4f;

		//if (nx != 0) vx = 0;
		if (ny < 0)
		{
			vy = 0;
			if (BolFLy == true)
				BolFLy = false;
			if (BolFlyDo == false)
				BolFlyDo = true;
			if (fly_start != NULL)
				fly_start = NULL;
			if (GetState() == MARIO_STATE_FLY_RIGHT || GetState() == MARIO_STATE_FLY_LEFT)
			{
					SetState(MARIO_STATE_IDLE);
			}
		}
		this->ny = ny;

		// Collision logic with Goombas
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{

			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj)) // if e->obj is Goomba 
			{


				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (goomba->GetState() == GOOMBA_STATE_WALKING)
				{
					if (e->ny < 0)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						//vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else if (e->nx != 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							LevelDie();
						}
					}
				}

				 

			}
			else
			{
				//KOOPA
				if (dynamic_cast<CKoopas*>(e->obj)) // if e->obj is Goomba 
				{


					CKoopas* koopa = dynamic_cast<CKoopas*>(e->obj);

					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (koopa->GetState() != KOOPA_STATE_DIE && koopa->GetState() != KOOPA_STATE_DIE_RUN_RIGHT && koopa->GetState() != KOOPA_STATE_DIE_RUN_LEFT)
						{
							koopa->SetState(KOOPA_STATE_DIE);
							//vy = -MARIO_JUMP_DEFLECT_SPEED;

						}
						else if (koopa->GetState() == KOOPA_STATE_DIE_RUN_RIGHT || koopa->GetState() == KOOPA_STATE_DIE_RUN_LEFT)
						{
							LevelDie();
						}

					}
					else if (e->nx != 0)
					{
						if (untouchable == 0)
						{
							if (koopa->GetState() != KOOPA_STATE_DIE && koopa->GetState() != KOOPA_STATE_DIE_HOLD)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level = level - 1;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
							else if (hold == true && run == true)
							{
								if (koopa->GetState() != KOOPA_STATE_DIE_HOLD)
								{
									koopa->SetState(KOOPA_STATE_DIE_HOLD);
									//koopa->SetVX(-vx);
									koopa->SetPosition(x + 18, y + 3);
									this->koopa = dynamic_cast<CKoopas*>(e->obj);
								}
							}
							else {
								if (nx == 1)
									koopa->SetState(KOOPA_STATE_DIE_RUN_LEFT);
								else if (nx == -1)
									koopa->SetState(KOOPA_STATE_DIE_RUN_RIGHT);
							}
						}
					}
				}
				else
				{
					//BLOCK
					if (dynamic_cast<CBlock*>(e->obj))
					{
						x = x + dx - min_tx * dx - nx * 0.4f;
						if (e->ny > 0)
							y = y + dy - min_ty * dy - ny * 0.4f;
						CBlock* block = dynamic_cast<CBlock*>(e->obj);

						if (e->ny < 0)
						{
							vy = 0;
							y = y0 + min_ty * dy + ny * 0.4f;
						}

					}
					else
					{   // Items
						if (dynamic_cast<Items*>(e->obj))
						{
							Items* item = dynamic_cast<Items*>(e->obj);
							if (e->ny > 0)
							{
								if (item->GetA() == 1)
								{
									if (item->GetB() == 0)
									{
										if (item->GetState() != ITEMS_STATE_STAR)
										{
											item->SetState(ITEMS_STATE_STAR);
											//vy = -MARIO_JUMP_DEFLECT_SPEED;
										}
									}
									else
									{
										if (item->GetB() == 1)
										{
											if (level > MARIO_LEVEL_SMALL)
											{
												if (item->GetState() != ITEMS_STATE_LEAF)
												{
													item->SetState(ITEMS_STATE_LEAF);
													//vy = -MARIO_JUMP_DEFLECT_SPEED;
												}
											}
											else
											{
												if (item->GetState() != ITEMS_STATE_MUSHROOM)
												{
													item->SetState(ITEMS_STATE_MUSHROOM);
													//vy = -MARIO_JUMP_DEFLECT_SPEED;
												}
											}
										}
									}
								}
								if (item->GetA() == 0)
								{


									if (item->GetState() != ITEMS_STATE_DIE)
									{
										item->SetState(ITEMS_STATE_DIE);
										//vy = -MARIO_JUMP_DEFLECT_SPEED;
									}

								}
							}
							if (e->nx != 0 || e->ny >= 0)
							{
								if (item->GetState() == ITEMS_STATE_MUSHROOM)
								{
									if (level == MARIO_LEVEL_SMALL)
									{
										level = MARIO_LEVEL_BIG;
										StartUntouchable();
									}
								}
							}
						}
					}
				}
			}


		}
	}
	// clean up collision events
}

void CMario::Render()
{
	for (int i = 0; i < bullets.size(); i++) {
			bullets[i]->Render();
	}

	int ani=-1;
	

	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
		if (level == MARIO_LEVEL_BIG)
		{
			if (vx == 0)
			{
				if (nx > 0) ani = MARIO_ANI_BIG_IDLE_RIGHT;
				else ani = MARIO_ANI_BIG_IDLE_LEFT;
			}
			else if (vx > 0)
				ani = MARIO_ANI_BIG_WALKING_RIGHT;
			else ani = MARIO_ANI_BIG_WALKING_LEFT;
		}
		else if (level == MARIO_LEVEL_SMALL)
		{
			if (vx == 0)
			{
				if (nx > 0) ani = MARIO_ANI_SMALL_IDLE_RIGHT;
				else ani = MARIO_ANI_SMALL_IDLE_LEFT;
			}
			else if (vx > 0)
				ani = MARIO_ANI_SMALL_WALKING_RIGHT;
			else ani = MARIO_ANI_SMALL_WALKING_LEFT;
		}
		else if (level == MARIO_LEVEL_RACCOON)
		{
			if (state == MARIO_STATE_FLY_RIGHT || ((GetTickCount() - fly_start < MARIO_FLY_TIME) && nx == 1))
				ani = MARIO_ANI_FLY_RIGHT;
			else if (state == MARIO_STATE_FLY_LEFT || ((GetTickCount() - fly_start < MARIO_FLY_TIME) && nx ==-1 ))
				ani = MARIO_ANI_FLY_LEFT;
			else if (vx == 0)
			{
				if (nx > 0) ani = MARIO_ANI_RACCOON_IDLE_RIGHT;
				else ani = MARIO_ANI_RACCOON_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (vx > MARIO_SPEED_MAX_FLY)
					ani = MARIO_ANI_RUN_FLY_RIGHT;
				else ani = MARIO_ANI_RACCOON_WALKING_RIGHT;

			}
			else if (vx < 0)
			{
				if (vx < -MARIO_SPEED_MAX_FLY )
					ani = MARIO_ANI_RUN_FLY_LEFT;
				else ani = MARIO_ANI_RACCOON_WALKING_LEFT;

			}
		}
		else if (level == MARIO_LEVEL_FIRE)
		{
			if (vx == 0)
			{
				if (nx > 0) ani = MARIO_ANI_FIRE_IDLE_RIGHT;
				else ani = MARIO_ANI_FIRE_IDLE_LEFT;
			}
			else if (vx > 0)
				ani = MARIO_ANI_FIRE_WALKING_RIGHT;
			else ani = MARIO_ANI_FIRE_WALKING_LEFT;
		}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(x, y, alpha);

	//RenderBoundingBox();
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		vx = MARIO_WALKING_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		vx = -MARIO_WALKING_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_RUN_RIGHT:
		vx = MARIO_WALKING_SPEED;
		nx = 1;
		break;
	case MARIO_STATE_RUN_LEFT:
		vx = -MARIO_WALKING_SPEED;
		nx = -1;
		break;
	case MARIO_STATE_JUMP:
		vy = -MARIO_JUMP_SPEED_Y;
		break;
	case MARIO_STATE_IDLE:
		vx = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_FLY_RIGHT:
		vx = MARIO_FLY_X;
		vy = -MARIO_FLY_SPEED_Y;
		nx = 1;
		break;
	case MARIO_STATE_FLY_LEFT:
		vx = -MARIO_FLY_X;
		vy = -MARIO_FLY_SPEED_Y;
		nx = -1;
		break;
	}
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		right = x + MARIO_RACCOON_BBOX_WIDTH;
		bottom = y + MARIO_RACCOON_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		right = x + MARIO_FIRE_BBOX_WIDTH;
		bottom = y + MARIO_FIRE_BBOX_HEIGHT;
	}
}

void CMario::HoldKoopa()
{
	if (hold == false)
	{
		hold = true;
		if (Runhold == false)
			Runhold = true;
		else Runhold = false;

	}
	else
	{
		hold = false;
		if (Runhold == false)
			Runhold = true;
		else Runhold = false;

	}
}

void CMario::K()
{
	if(((vx > MARIO_SPEED_MAX_FLY) || (vx < -MARIO_SPEED_MAX_FLY) || BolFLy == true)&& level == MARIO_LEVEL_RACCOON)
	{
			Fly();
	}
	else if(BolFlyDo == true)
	{
		SetState(MARIO_STATE_JUMP);
		BolFLy = true;
		BolFlyDo = false;
	}
}

void CMario::Fly()
{	
	if (level == MARIO_LEVEL_RACCOON)
	{
		if ((nx == 1 && vx >= MARIO_SPEED_MAX_FLY) || (vx <= -MARIO_SPEED_MAX_FLY && nx == -1))
		{
			fly_start = GetTickCount();
			BolFLy = true;
			BolFlyUp = true;
		}
		if (BolFLy == true)
		{
			if ((GetTickCount() - fly_start < MARIO_FLY_TIME))
			{
				if (BolFlyUp == true)
				{
					if (nx == 1)
						SetState(MARIO_STATE_FLY_RIGHT);
					else if (nx == -1)
						SetState(MARIO_STATE_FLY_LEFT);
				}
			}
			else
			{
				if (nx == 1)
					SetState(MARIO_STATE_FLY_RIGHT);
				else if (nx == -1)
					SetState(MARIO_STATE_FLY_LEFT);
				vy = MARIO_WALKING_SPEED;
			}
		}
		BolFlyUp = false;
	}
	
}

void CMario::Bullet() {
	if (level == MARIO_LEVEL_FIRE) 
	{
		lastShotTime = GetTickCount();
		bullet = new CBulletFire();
		if (nx > 0) {
			bullet->SetPosition(this->x + 7.0f, this->y + 5.0f);
			bullet->SetState(BULLETFIRE_STATE_WALKING_RIGHT);
		}
		else {
			bullet->SetPosition(this->x - 7.0f, this->y + 5.0f);
			bullet->SetState(BULLETFIRE_STATE_WALKING_LEFT);
		}
		bullets.push_back(bullet);
	}
}

void CMario::LevelDie()
{
	if (untouchable == 0)
	{
		if (level > MARIO_LEVEL_SMALL)
		{
			level = level - 1;
			StartUntouchable();
		}
		else
			SetState(MARIO_STATE_DIE);
	}
}
