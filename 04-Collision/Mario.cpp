#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Koopa.h"
#include "Brick.h"
#include "CBlock.h"
#include "Items.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->isDisable)
			bullets[i]->Update(dt, coObjects);
	}

	for (int i = 0; i < bullets.size(); i++) {
		if (bullets[i]->isDisable) {
			CGameObject* p = bullets[i];
			bullets.erase(bullets.begin() + i);
			delete p;
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
		if (Runhold == false && hold ==false && this->koopa->GetState() == KOOPA_STATE_DIE_HOLD)
		{
			if (nx ==1)
				this->koopa->SetState(KOOPA_STATE_DIE_RUN_RIGHT);
			else if (nx==-1)
				this->koopa->SetState(KOOPA_STATE_DIE_RUN_LEFT);
		}

	}

	// Simple fall down
	vy += MARIO_GRAVITY*dt;

	//Fly
	
	if (BolFLy == true)
	{
		if (BolFlyDo == false)
		{
			if (GetTickCount() - fly_start > MARIO_FLY_TIME)
				StopFly();
			if (nx == 1)
				SetState(MARIO_STATE_FLY_RIGHT);
			else if (nx == -1)
				SetState(MARIO_STATE_FLY_LEFT);
		}
		else
		{
			if (this->ny < 0)
			{
				BolFlyDo = false;
				BolFLy = false;
			}
			else
			{
				if (nx == 1)
					SetState(MARIO_STATE_FLY_RIGHT);
				else if (nx == -1)
					SetState(MARIO_STATE_FLY_LEFT);
				vy = MARIO_FLY_Y_DO;
			}
		}
	}

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state!=MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed // bat tu
	if ( GetTickCount() - untouchable_start > MARIO_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	
	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		
		x += dx; 
		y += dy;
		this->ny = 1;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		float y0, x0;
		x0 = x;
		y0 = y;

		
		// block 
		x = x0+ min_tx * dx + nx * 0.4f;		
		y =y0+ min_ty * dy + ny * 0.4f;

		//if (nx != 0) vx = 0;
		if (ny < 0) vy = 0;
		this->ny = ny;

		// Collision logic with Goombas
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				

				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (goomba->GetState() != GOOMBA_STATE_DIE)
				{
					if (e->ny < 0)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						//vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else if (e->nx != 0)
					{
						if (untouchable == 0)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE)
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
					}
				}
				
				

			}
			else
			{
				//KOOPA
				if (dynamic_cast<Koopa*>(e->obj)) // if e->obj is Goomba 
				{


					Koopa* koopa = dynamic_cast<Koopa*>(e->obj);

					// jump on top >> kill Goomba and deflect a bit 
					if (e->ny < 0)
					{
						if (koopa->GetState() != KOOPA_STATE_DIE  && koopa->GetState() != KOOPA_STATE_DIE_RUN_RIGHT && koopa->GetState() != KOOPA_STATE_DIE_RUN_LEFT)
						{
							koopa->SetState(KOOPA_STATE_DIE);
							//vy = -MARIO_JUMP_DEFLECT_SPEED;
							
						}
						else if (untouchable == 0)
						{
							if (koopa->GetState() == KOOPA_STATE_DIE_RUN_RIGHT || koopa->GetState() == KOOPA_STATE_DIE_RUN_LEFT)
							{
								if (level > MARIO_LEVEL_SMALL)
								{
									level--;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
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
									level = level-1;
									StartUntouchable();
								}
								else
									SetState(MARIO_STATE_DIE);
							}
							else if (hold == true)
							{
								if (koopa->GetState() != KOOPA_STATE_DIE_HOLD)
								{
									koopa->SetState(KOOPA_STATE_DIE_HOLD);
									//koopa->SetVX(-vx);
									koopa->SetPosition(x + 18, y + 3);
									this->koopa = dynamic_cast<Koopa*>(e->obj);
								}
							}
							else {
								if( nx ==1)
									koopa->SetState(KOOPA_STATE_DIE_RUN_LEFT);
								else if(nx == -1)
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
						if(e->ny >0)
							y = y + dy - min_ty * dy - ny * 0.4f;
						CBlock* block = dynamic_cast<CBlock*>(e->obj);

						if (e->ny < 0 ) 
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
							if (e->nx != 0||e->ny>=0)
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
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CMario::Render()
{
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->isDisable)
			bullets[i]->Render();
	}

	int ani;
	int alpha = 255;
	if (untouchable) alpha = 128;

	if (state == MARIO_STATE_DIE)
		ani = MARIO_ANI_DIE;
	else
	if (level == MARIO_LEVEL_BIG)
	{
		if (vx == 0)
		{
			if (nx>0) ani = MARIO_ANI_BIG_IDLE_RIGHT;
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
			if (nx>0) ani = MARIO_ANI_SMALL_IDLE_RIGHT;
			else ani = MARIO_ANI_SMALL_IDLE_LEFT;
		}
		else if (vx > 0)
			ani = MARIO_ANI_SMALL_WALKING_RIGHT;
		else ani = MARIO_ANI_SMALL_WALKING_LEFT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		if (state == MARIO_STATE_FLY_RIGHT )
			ani = MARIO_ANI_FLY_RIGHT;
		else if (state == MARIO_STATE_FLY_LEFT)
			ani = MARIO_ANI_FLY_LEFT;
		else if (vx == 0)
		{
			if (nx >0) ani = MARIO_ANI_RACCOON_IDLE_RIGHT;
			else ani = MARIO_ANI_RACCOON_IDLE_LEFT;
		}
		else if (vx > 0)
		{
			if (vx > MARIO_SPEED_MAX)
				ani = MARIO_ANI_RUN_FLY_RIGHT;
			else ani = MARIO_ANI_RACCOON_WALKING_RIGHT;
			
		}
		else { 
			if (vx < -MARIO_SPEED_MAX)
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

	
	animations[ani]->Render(x, y, alpha);


	RenderBoundingBox();
}

void CMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (vx == 0) vx = MARIO_WALKING_SPEED;
		else
		if (vx > 0 && vx < MARIO_SPEED_MAX)
		{
			               
			 vx = vx + MARIO_ACCELERATION_X * dt;
		}
		else if(vx>0) vx = MARIO_SPEED_MAX;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (vx == 0) vx = -MARIO_WALKING_SPEED;
		else
		if (vx < 0 && vx > -MARIO_SPEED_MAX)
		{
			
			 vx = vx - MARIO_ACCELERATION_X * dt;
		}
		else vx = -MARIO_SPEED_MAX;
		nx = -1;
		break;
	case MARIO_STATE_RUN_RIGHT:
		if (vx == 0) vx = MARIO_WALKING_SPEED;
		else
		if (vx >= 0 && vx < MARIO_SPEED_RUN_MAX)
		{
			
			vx = vx + MARIO_ACCELERATION_X * dt;
		}
		else vx = MARIO_SPEED_RUN_MAX;
		nx = 1;
		break;
	case MARIO_STATE_RUN_LEFT:
		if (vx == 0) vx = -MARIO_WALKING_SPEED;
		else
		if (vx <= 0 && vx > -MARIO_SPEED_RUN_MAX)
		{
			
			vx = vx - MARIO_ACCELERATION_X * dt;
		}
		else vx = -MARIO_SPEED_RUN_MAX;
		nx = -1;
		break;
	case MARIO_STATE_JUMP: 
		vy = -MARIO_JUMP_SPEED_Y;
	case MARIO_STATE_IDLE: 
		vx = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_FLY_RIGHT:
		vx = MARIO_FLY_X;
		vy = -MARIO_FLY_Y;
		nx = 1;
		break;
	case MARIO_STATE_FLY_LEFT:
		vx = -MARIO_FLY_X;
		vy = -MARIO_FLY_Y;
		nx = -1;
		break;
	}
}

void CMario::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==MARIO_LEVEL_BIG)
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
		if (Runhold==false)
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

void CMario::Fly()
{
	if (level == MARIO_LEVEL_RACCOON)
	{
		if (vx >= MARIO_SPEED_RUN_MAX || vx <= -MARIO_SPEED_RUN_MAX)
		{
			BolFLy = true;
			fly_start = GetTickCount();
		}
		else if (ny >= 0)
		{
			BolFLy = true;
			BolFlyDo = true;
		}
	}
}

void CMario::Shot() {
	if (GetTickCount() - lastShotTime >= MARIO_SHOT_COOLDOWN_TIME && this->level == MARIO_LEVEL_FIRE) {
		lastShotTime = GetTickCount();
		bullet = new CBulletFire();
		bullet->AddAnimation(508);
		bullet->AddAnimation(508);
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
