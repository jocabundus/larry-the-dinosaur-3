//- Larry The Dinosaur III Entity Class Processing File
//- November, 2002 - Created by Joe King
//=====================================================

//#include "main.h"
#include <math.h>
#include "defines.h"
#include "entity.h"

ENTITY::ENTITY()
{
	Init();
}

ENTITY::~ENTITY()
{
}

void ENTITY::Init()
{
	//- Initialize an entity to default values
	//----------------------------------------

	x  = 0; y  = 16;
	ox = x; oy = y;
	id = 0;
	velocity		= 0;
	gravity			= 0.09f;
	//gravity			= 0.1125f;
	speed			= 0;
	uani			= 0;
	lani			= 0;
	airfriction		= .01f;
	groundfriction	= .05f;
	friction		= groundfriction;
	life			= 100;
	maxspeed		= 2;
	lstart			= 0;
	lend            = 6;
	ustart          = 10;
	uend            = 7;
	aiState			= aiCALM;
	count			= 0;
	count2			= 0;
	count3			= 0;
	setspeed		= 0;
	attacking		= false;
	spriteset		= 0;
	dontfall		= false;
	shoot			= false;
	ScoreValue		= 0;
	inertialx		= 0;
	BloodCol		= 37;
	visible			= true;
	armor			= 1.0f;
	nocheck			= false;
	target			= -1;

	HitBox.left		= 0;
	HitBox.right	= 19;
	HitBox.top		= 0;
	HitBox.bottom	= 19;
}


void ENTITY::AssignID(int ID)
{
	//- Assign the entity's ID
	//------------------------
	id = ID;
}


int ENTITY::Fall(int *Map, int MapWidth, float diff)
{
	//- Make the entity fall
	//----------------------

	int mx, my, mp;
	int HitFloor = 0;
	int left	= HitBox.left;
	int right	= HitBox.right;
	int top		= HitBox.top;
	int bottom	= HitBox.bottom;
	int	CheckFloor = false;

	if(nocheck == false){
	velocity += gravity*diff;
	oldvelocity = velocity;	
	velocity *= diff;
	if(fabs(velocity) < 20)
		y += velocity;
	else{
		if(velocity > 0)
			y += 20;
		else
			y -= 20;
	}

	if(y-oy >= 0)
	{
		//for(int cy = 0; cy <= 19; cy += 19)
		//{
		int cy = bottom;
		if(xFlip){
			for(int cx = left; cx <= right; cx += right-left)
			{
				mx = ((int)x+(19-cx)) / 20;
				my = ((int)y+cy-4) / 20;
				mp = Map[mx+(my<<MAPXLSHIFT)+55000];
				if(mp >= 40 && mp < 256 && (int)(y+cy-4) % 20 <= velocity)
				{
					//- The entity hit the ground
					y = my*20-bottom+4;
					velocity	= 0;
					if(mp == miSlippery) groundfriction = 0.015f; else groundfriction = 0.05f;
					friction	= groundfriction;
					HitFloor	= mp;
					inertialx   = 0;
				}
			}
		}
		else{
			for(int cx = left; cx <= right; cx += right-left)
			{
				mx = ((int)x+cx) / 20;
				my = ((int)y+cy-4) / 20;
				mp = Map[mx+(my<<MAPXLSHIFT)+55000];
				if(mp >= 40 && mp < 256 && (int)(y+cy-4) % 20 <= velocity)
				{
					//- The entity hit the ground
					y = my*20-bottom+4;
					velocity	= 0;
					if(mp == miSlippery) groundfriction = 0.015f; else groundfriction = 0.05f;
					friction	= groundfriction;
					HitFloor	= mp;
					inertialx   = 0;
				}
			}
		}
	}
	else
	{
		if(xFlip){
			int cy = top;
			int cx = (left+right)>>1;
			
			mx = ((int)x+(19-cx)) / 20;
			my = ((int)y+cy-5) / 20;
			mp = Map[mx+(my<<MAPXLSHIFT)+55000];
			if(mp > 9 && mp < 40)
			{
				//- The entity hit the ceiling
				y = my*20+25;
				velocity	= 0;
				friction	= airfriction;
				HitFloor	= -1;
			}			
		}
		else{
			int cy = top;
			int cx = (left+right)>>1;
			
			mx = ((int)x+cx) / 20;
			my = ((int)y+cy-5) / 20;
			mp = Map[mx+(my<<MAPXLSHIFT)+55000];
			if(mp > 9 && mp < 40)
			{
				//- The entity hit the ceiling
				y = my*20+25;
				velocity	= 0;
				friction	= airfriction;
				HitFloor	= -1;
			}						
		}
	}
	if(HitFloor == -1)
		HitFloor = 0;
	else if(HitFloor == 0){
		friction = airfriction;
		velocity = oldvelocity;
	}
	}
	return(HitFloor);
}

int ENTITY::CheckWallHit(int *Map, int MapWidth)
{
	//- Check if the entity hit a wall
	//--------------------------------

	int mx, my, mp, mp2;
	long mpt;
	int left	= HitBox.left;//-1;
	int right	= HitBox.right;//+1;
	int top		= HitBox.top;
	int bottom	= HitBox.bottom;
	int cyadd	= -5;
	int result	= 0;
	
	if(nocheck == false){
	for(int cy = top; cy <= bottom; cy += bottom-top)
	{
		if(xFlip){
			for(int cx = left; cx <= right; cx += right-left)
			{
				mx = ((int)x+(19-cx)) / 20;
				my = ((int)y+cy+cyadd) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp  = Map[mpt];	mp2 = Map[mpt+MapWidth];
				if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40))
				{
					if((int)x % 20 < 10)
						x = mx*20-20+left;
					else
						x = mx*20+20-(19-right);
					
					lani = lstart;
					speed  = 0;
					result = mp;
				}
			}
		}
		else{
			for(int cx = left; cx <= right; cx += right-left)
			{
				mx = ((int)x+cx) / 20;
				my = ((int)y+(cy+cyadd)) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp  = Map[mpt];	mp2 = Map[mpt+MapWidth];
				if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40))
				{
					if((int)x % 20 < 10)
						x = mx*20-right;
					else
						x = mx*20+20-left;
					
					lani = lstart;
					speed  = 0;
					result = mp;
				}
			}
		}
		if(x < 0) x = 0;		
	}	
	}
	return(result);
}

void ENTITY::Move(float diff)
{
	//- Move the entity
	//-----------------
	float gf = 8/diff;//groundfriction*160;
	float anispeed;

	if(speed >  maxspeed) speed =  maxspeed;
	if(speed < -maxspeed) speed = -maxspeed;

	oldspeed = speed;
	
	x += speed*diff;
	if(friction == airfriction) x += inertialx;
	if(speed > 0) speed -= friction*diff;
	if(speed < 0) speed += friction*diff;
	if(fabs(speed) < friction*diff) speed = 0;

	anispeed = fabs(speed) / gf;

	if(friction != 0.015f){
		if((!xFlip && speed > 0) || (xFlip && speed < 0))
			lani += anispeed;
		else
			lani -= anispeed;
	}
	else
		lani += anispeed;

	if(lani >= lend) lani = lstart + 1 + anispeed;
	else if(lani < lstart+1) lani = lend - anispeed;

	//if(lani >= lend) lani = lstart + 1;
	//else if(lani < lstart+1) lani = lend;

	if(fabs(speed) < groundfriction) lani = lstart;

	if(friction == 0.015f && ((xFlip == false && speed < oldspeed2) || (xFlip == true && speed > oldspeed2)))
		lani = lstart;
	else if(friction == airfriction || velocity > 1) lani = lstart+5;
	
	if(setspeed != 0) speed = setspeed;
	oldspeed2 = speed;
}
