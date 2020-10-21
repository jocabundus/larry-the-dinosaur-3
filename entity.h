//- Larry The Dinosaur III Entity Class Header File
//- November, 2002 - Created by Joe King
//=================================================

#ifndef ENTITY_H
#define ENTITY_H

#include "slx_win32types.h"

class ENTITY
{
public:

					ENTITY();
					~ENTITY();

		void		AssignID(int ID);
		int 		Fall(int *Map, int MapWidth, float diff);
		int			CheckWallHit(int *Map, int MapWidth);
		void		Move(float diff);
		void		Init();
			
		float		x, y;
		float		ox, oy;
		int			startx, starty;
		float		velocity;
		float		oldvelocity;
		float		gravity;
		float		speed;
		float		oldspeed;
		float		oldspeed2;
		float		friction;
		float		airfriction;
		float		groundfriction;
		float		life;
		float		maxspeed;
		bool        xFlip;
		bool		shoot;
		bool		nocheck;
		float		armor;
		
		float		uani;
		float		lani;
		float       ustart, uend;
		float       lstart, lend;

		int			id;
		int			aiState;
		float		count, count2, count3;
		int			target;
		float		setspeed;
		bool		attacking;
		int			spriteset;
		bool		dontfall;
		int			ScoreValue;
		float		inertialx;
		int			BloodCol;
		bool		visible;

		RECT		HitBox;		

private:
};

#endif
