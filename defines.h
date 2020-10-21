#ifndef DEFINES_H
#define DEFINES_H

#include "slx_win32types.h"

#define		LARRY			1985
#define		NO_ONE_CARES	1
#define		PI				3.141592654

#define		BABY	0
#define		EASY	1
#define		NORMAL	2
#define		HARD	3
#define		UNFAIR	4
#define		JUSTICE	5

#define		OPENING		2
#define		CLOSING		1

#define		miL		256
#define		miA		260
#define		miR1	264
#define		miR2	268
#define		miY		272
#define		miHealthBottle		276
#define		miHealthBottle2X	280
#define		miSpeedBottle		284
#define		miSpeedBottle2X		288
#define		miJumpBottle		292
#define		miJumpBottle2X		296
#define		miPowerBottle		300
#define		miPowerBottle2X		304
#define		miAllInOneBottle	308
#define		miAllInOneBottle2X	312
#define		miExtraLife			316
#define		miMagazine			320
#define		miBoxOfShells		324
#define		miGreenCrystal		401
#define		miYellowCrystal		405
#define		miBlueCrystal		409
#define		miRedCrystal		413
#define		miGrenade			328
#define		miRifle				332
#define		miChopper			336
#define		miSingularity		340
#define		miSingularityAmmo	344
#define		miGreenCard			348
#define		miBlueCard			352
#define		miYellowCard		356
#define		miRedCard			360
#define		miShotgun			364
#define		miSecret1			417
#define		miSecret2			418
#define		miSecret3			419
#define		miSecret4			420
#define		miSecret5			421
#define		miSecret6			422
#define		miSecret7			423
#define		miSecret8			424
#define		miSecret9			425
#define		miScript1			426
#define		miScript2			427
#define		miScript3			428
#define		miScript4			429
#define		miScript5			430
#define		miScript6			431
#define		miScript7			432
#define		miScript8			434
#define		miScript9			444
#define		miSpikes			30
#define		miLaser				31
#define		miLaserSwitch		73
#define		miTouchPlate		76
#define		miSlippery			79
#define		miJumpFan			99
#define		miTreadmillLeft		110
#define		miTreadmillRight	120

#define		miFireCannonUp		500
#define		miFireCannonDown	501
#define		miFireCannonLeft	502
#define		miFireCannonRight	503
#define		miFallingObject		504
#define		miFallingGround		505
#define		miTouchPlateWall	506

#define		miLarryStart	510
#define		miSonicMonster	511
#define		miRockMonster	512
#define		miIcedSpikehead	513
#define		miJellyBlob		514
#define		miFly			515
#define		miRusty			517
#define		miMark			518
#define		miFletcher		519
#define		miXahn			520
#define		miBadMark		521
#define		miGeneral		522
#define		miGBAsleep		522
#define		miAlDog			523
#define		miSpider		524
#define		miGreenBlob		523
#define		miBloodHead		524
#define		miToxicBubbles_UP	525
#define		miToxicBubbles_LT	526
#define		miToxicBubbles_RT	527
#define		miSpikePlant	528
#define		miVRockMonster	529
#define		miBlobMine		530
#define		miTroop			531
#define		miMegaFly		534
#define		miRat			535
#define		miBigSpider		536
#define		miBigSpider2	537
#define		miPlatform_Lr	550
#define		miPlatform_lR	551
#define		miPlatform_Ud	552
#define		miPlatform_uD	553
#define		miPlatform_Trap	554
#define		miAnchor		555
#define		miPlatform_CL	556
#define		miPlatform_CR	557
#define		miGoCart		558
#define		miBoss			559
#define		miBoulder_Left	560
#define		miBoulder_Right	561
#define		miSmashUp		562
#define		miSmashDown		563
#define		miSwing			564
#define		miRotoPlatform_CL	565
#define		miRotoPlatform_CR	566
#define		miSpikeBall			567
#define		miCurrentUp			568
#define		miCurrentDown		569
#define		miCurrentLeft		570
#define		miCurrentRight		571
#define		miElevatorDown		572
#define		miElevatorUp		573
#define		miPositionOne		575
#define		miPositionTwo		576
#define		miGrenadeCursor		577
#define		miDrainLifeBy5		578
#define		miExplosive			579
#define		miDetonate			580
#define		miTeleport1			581
#define		miTeleport2			582
#define		miTeleport3			583
#define		miTeleport4 		584
#define		miSpikeBall2		585
#define		miSwing2			587
#define		miGreenDoor			590
#define		miBlueDoor			591
#define		miYellowDoor		592
#define		miRedDoor			593

#define		ElevatorUpArrow		154
#define		ElevatorDownArrow	155

#define		miSpikesLeft	554
#define		miSpikesRight	555
#define		miSpikesUp		574

#define		SpikeBallChain	118
#define		SpikeBall		119

#define		miMakerFlag		507
#define		miTaggedFlag	508
#define		miEndFlag		509

#define		aiCALM		1
#define		aiSCARED	2
#define		aiANGRY		3
#define		aiCRAZY		4

#define		SONIC_WAVE		1
#define		SHOTGUN			2
#define		MACHINEGUN		3
#define		YELLOW_FIRE		4
#define		FALLING_OBJECT	5
#define		FALLING_GROUND	6
#define		BALL			7
#define		BUBBLE			8
#define		TOXIC_BUBBLE	9
#define		COTTON_SPIKE	10
#define		MAC				11
#define		SINGULARITY		12
#define		SLEDGEHAMMER	13

#define		ADD			1
#define		SUBTRACT	2
#define		MULTIPLY	3
#define		DIVIDE		4

#define		FADE_SPEED  10

#define		LARRY_SPRITE_SET			0
#define		FLETCHER_SPRITE_SET			4
#define		MARK_SPRITE_SET				0
#define		DIALOG_SPRITE_SET			0
#define		GENERAL_SPRITE_SET			0
#define		XAHN_SPRITE_SET				0
#define		ICON_SPRITE_SET				3
#define		SPIDER_SPRITE_SET			1
#define		SONIC_MONSTER_SPRITE_SET	1
#define		ROCK_MONSTER_SPRITE_SET		1
#define		ICED_SPIKEHEAD_SPRITE_SET	1
#define		FLY_MONSTER_SPRITE_SET		1
#define		ALDOG_SPRITE_SET			1
#define		JELLY_BLOB_SPRITE_SET		1
#define		GREEN_BLOB_SPRITE_SET		1
#define		BLOOD_HEAD_SPRITE_SET		1
#define		VROCK_MONSTER_SPRITE_SET	2
#define		YELLOW_FIRE_SPRITE_SET		2
#define		SPIKE_PLANT_SRITE_SET		2
#define		EXPLOSION_SPRITE_SET		2
#define		RUSTY_SPRITE_SET			2
#define		BOSS_SPRITE_SET				3
#define		BLOB_MINE_SPRITE_SET		4
#define		TROOP_SPRITE_SET			4
#define		MAC_SPRITE_SET				4
#define		MEGAFLY_SPRITE_SET			4
#define		RAT_SPRITE_SET				4
#define		BIGSPIDER_SPRITE_SET		4
#define		DOORS_SPRITE_SET			4

#define		YELLOW_FIRE_UP		1
#define		YELLOW_FIRE_DOWN	2
#define		YELLOW_FIRE_LEFT	3
#define		YELLOW_FIRE_RIGHT	4
#define		GRENADE_SPRITE		7
#define		EXPLOSION_START		40

#define		UP		90
#define		DOWN	270
#define		LEFT	180
#define		RIGHT	0

#define		iconShotgun				40
#define		iconShotgunMissing		41
#define		iconMachinegun			42
#define		iconMachinegunMissing	43
#define		iconFlamethrower		44
#define		iconFlamethrowerMissing	45
#define		iconBazooka				46
#define		iconBazookaMissing		47
#define		iconPistol				50
#define		iconPistolMissing		51

#define		SKELETON		39

#define		MAPWIDTH		512
#define		MAPHEIGHT		100
#define		MAPXLSHIFT		9

//- Defines for entities
//----------------------------------------------------------------------------------
#define	NO_ONE_CARES		1		//- Why should you care what this does?
#define LarryTalk			999		//- If Larry is talking
#define SONIC_MONSTER		2		//- id for sonic monster entity
#define ROCK_MONSTER		3		//- id for rock monster entity
#define PLATFORM_lR			4		//- id for platform left-right entity
#define PLATFORM_uD			5
#define PLATFORM_Lr			-4
#define PLATFORM_Ud			-5
#define PLATFORM_TRAP		6
#define ICED_SPIKEHEAD		7
#define ANCHOR				8
#define PLATFORM_CIRCLE		9
#define	BOSS1				101
#define	BOSS2				102
#define	BOSS3				103
#define	BOSS4				104
#define	BOSS5				105
#define BOSS6				106
#define BOSS7				107
#define LASTBOSS			666
#define BOULDER				10
#define GO_CART				11
#define	FLY_MONSTER			12
#define JELLY_BLOB			13
#define GRENADE				14
#define FLETCHER			15
#define XAHN				16
#define ROTOPLATFORM		17
#define	SPIKEBALL1			18
#define MARK				19
#define RUSTY				20
#define GENERAL				21
#define SPIKEPLANT			22
#define COTTONSPIKE			23
#define SPIKEBALL2			24
#define	BOSS				25
#define SWING				26
#define SWING2				27
#define SPIKEBALL			28
#define SMASH_UP			29
#define SMASH_DOWN			30
#define SMASH				31
#define SPIDER				32
#define ELEVATORDOWN		33
#define ELEVATORUP			34
#define ELEVATOR			35
#define BOMB				36
#define DUD					37
#define ALDOG				38
#define GREENBLOB			39
#define BLOODHEAD			40
#define GBASLEEP			41
#define SNOWMOBILE			42
#define WEAK_BOULDER		43
#define LOOGIE				148
#define VROCK_MONSTER		149
#define BLOB_MINE			150
#define TROOP				151
#define FIRETROOP			152
#define CHOPPER				153
#define MEGAFLY				154
#define GREENDOOR			155
#define BLUEDOOR			156
#define YELLOWDOOR			157
#define REDDOOR				158
#define RAT					159
#define BIGSPIDER			160
#define BIGSPIDER2			161
#define INVISIBLEWALL		39

#define lvTWO_WEEKS_LATER		0
#define lvVENTS					1
#define lvSPIKE_VALLEY			2
#define lvFALL_EVENING			3
#define lvOCEAN_DROP			4
#define lvSHINY_FOREST			5
#define lvAQUEDUCT_CROSSWAYS	6
#define lvFROST_BITE			7
#define lvOTHER_WORLD			8
//----------------------------------------------------------------------------------

struct LD3BONUS{
	bool L;
	bool A;
	bool R1;
	bool R2;
	bool Y;
};

struct PARTICLE{
	float x, y;
	float xspeed, yspeed;
	float yvelocity;
	int   timer;
	int   col;
};

struct PROJECTILE{
	float x, y;
	int   dy;
	float xspeed, yspeed;
	int   timer;
	bool  falling;
	float yvelocity;
	bool  explode;
	int   radius;
	float spritenum;
	int   spriteset;
	float anicount;
	int   firstsprite;
	int   lastsprite;
	bool  xflip;
	bool  delafterlast;
	bool  del;
	int   power;
	int   id;
	RECT  HitBox;
};

struct CANNON{
	int x, y;
	int timer;
	int dir;
	int spriteset;
	int spritenum;
	int id;
};

struct LOCATION{
	int x, y, l;
};

#endif

