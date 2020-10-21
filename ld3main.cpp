//- Larry The Dinosaur 3 Main Game File
//- November, 2002 - Created by Joe King
//- September, 2003
//=======================================

//- Larry = 2 meters tall
//- 1 pixel = 2/16 = 0.125 meters
//- 4.9/0.125 = 39.2 - Larry Falls 39.2 pixels per second
//- 39.2/60fps = 0.653333

//#include "main.h"
using namespace std;
#include <math.h>
#include <time.h>
#include <fstream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include "fake_fmod.h"
#include "defines.h"
#include "entity.h"
#include "ld3.h"


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- Functions
//----------------------------------------------------------------------------------------------------------------------------
bool ProcessInput();				//- takes in the input and processes what's needed based on the input
void RenderScene();					//- draws the environment, entities, etc
void ProcessItems();				//- the main function for processing entities, timers, etc
void ProcessPlayer();				//- process the player
void LoadScript(const char* Filename);	//- loads in the given script and get's it ready for processing
void ReadWord(ifstream *File, char *Word);	//- reads a word from the given file
void ReadQuote(ifstream *File, char *Word);	//- reads what's in the double quotes from the given file
int  ReadValue(ifstream *File);				//- reads the value of a number from the given file
float ReadArithemetic(ifstream *File);		//- reads in simple arithmentic from the given file and returns it's result
void EntityTalk(char *Msg, int FaceId);		//- set what's needed for the given entity to talk
void ProcessScript();						//- process the script one command at a time
void RunScript();							//- runs what the script has processed(talking, fading, etc)
void ProcessMonsters();						//- process the monsters/platforms, calculate the monster's AI
void DrawMonsters();						//- draws the monsters/platforms
void Splatter(int x, int y, int amount, float xdir, int col);	//- create the given number of particles with the given color
void ProcessParticles();										//- move the particles
void ProcessCannons();											//- make the cannons shoot when their timer hits zero
void ScanMap();													//- scan the map at set the values for scores, monsters, etc.
void FireProjectile(float x, float y, int angle, float speed, int PreDefinedWeapon, ENTITY *owner, int dy);
void ProcessProjectiles();					//- move the projectiles, check if they hit a wall, the player
void WriteText(int x, int y, const char* Text, DWORD col, UINT format);	//- write a message to the screen
void DeactivateLasers();					//- turn off all the lasers on the map
void TurnOffWalls();						//- turn off all the walls on the map
void WriteMessage(const char* msg);				//- write a system message on the screen
void CheckForItems();						//- check if the player is in contact with any items(crystals, bottles, spikes, etc.)
void NextLevel(int level);					//- load in the next level
void ResetEntities();						//- ????
void RemoveEntities();						//- ????
void DisplayHUD();							//- draw the player's health, ammo, messages
void CreateExplosion(int x, int y);			//- create an explosion at the given coordinates
void ProcessExplosions();					//- process the explosion's animation
void mnuTitle();							//- process the title menu
void mnuScores();							//- show the hiscores
void mnuOptions();							//- process the options menu
void mnuSound();							//- sound options
void mnuControls();							//- controls options
void mnuCredits();							//- show the credits
void mnuCredits2();							//- show the credits
bool mnuChooseSlot(bool Loading);			//- process the new/load menu
void WriteBigFont(int x, int y, const char *text);//- write font onto the screen in the big, green letters
void SaveGame();							//- save the game in the Slot
void PutString(ofstream *File, char *Text);	//- write the given text into the given file
void DetonateExplosives();					//- detonate the explosives on the map
void SetDefaultValue(ENTITY *Entity);		//- set the default values for the given entity
void EnterHiScore();						//- check if the score is within the top ten, if so, enter initials for the list
void Flash(int red, int grn, int blu);		//- flash the screen with the given color
void Squirt(int x, int y, int amount, float xdir, int col);
bool FadeScreenToBlack();					//- fade the screen to black, returns true if any input was taken
bool FadeScreenOutOfBlack();				//- fade the screen out of black
bool Wait(int seconds);						//- wait for the given amount of seconds
ENTITY *CreateNewMAC();
void Intro();								//- do the intro
void Intro1();								//- sub intro 1
void Intro2();								//- sub intro 2
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to entities
//----------------------------------------------------------------------------------------------------------------------------
ENTITY				*Player;					//- points to the entity that the player is controlling
ENTITY				*Player2;					//- points to the entity that player 2 is controlling
ENTITY				*Boss;						//- points to the boss entity
ENTITY				*MACunit;					//- pointer to a controlled mini assault chopper
ENTITY				*FocusEntity;				//- pointer to the entity that is the focus
ENTITY				*FocusEntity2;				//- pointer to the entity that is the focus of the second screen
ENTITY				Monster[500];				//- monster/enemy entities
ENTITY				Platform[500];				//- platforms have the same properties of entities
ENTITY				NullEntity;					//- sometimes this is provided when a funciton request a pointer to an entity
												//- when there isn't one
ENTITY				*Elevator[20];				//- pointers to the elevator entities
PARTICLE			Particle[500];				//- particles are the pixel sized particles
PROJECTILE			Projectile[1500];			//- projectiles are fire balls, bubbles, sonic waves, etc.
CANNON				Cannon[50]; 				//- cannons are timed objects that shoot projectiles
LOCATION			Laser[50];					//- lasers are the barriers that need a switch to be pressed to turn them off
LOCATION			Wall[50];					//- walls are special walls that disappear when a touchplate is triggered
LOCATION			Explosion[50];				//- holds the locations and animation of the explosions
LOCATION			Explosive[100];				//- explosive is a set explosion that goes off when something is triggered
LOCATION			Marker;						//- the location of the marker
int					NumLasers  = 0;				//- the number of lasers in the map
int					NumWalls   = 0;				//- the number of special walls in the map
int					NumParticles = 0;			//- the number of particles
int					NumProjectiles    = 0;		//- the number of projectiles, fireballs, bubbles, etc.
int					NumCannons        = 0;		//- the number of cannons in the map(timed objects that shoot projectiles)
int					NumExplosives	  = 0;		//- the number of explosives in the map(see above for more info)
int					NumResetPlatforms = 0;		//- the number of platforms that need to be reset when the player dies
int					NumExplosions = 0;			//- the number of explosions currently going off
int					NumElevators = 0;			//- the number of elevators in the map
int					PlatformBlood = 132;		//- the color of the platform blood
int					NumMonsters = 0;			//- the number of entitys(monsters/platforms) in the map
int					NumPlayers = 1;				//- the number of players
int					CurrentPlayer = 1;			//- the current player being processed
int					Focus  = 0;					//- index of the entity the camera is focused on
int					Focus2 = 0;					//- index of the entity the second camera is focused on
int					PlatformTimer = 0;			//- the timer that depicts where the platforms are in relation to their
												//- starting spot
bool				Elevating	= false;		//- true if the player is using an elevator
bool				ControllingMAC = false;		//- true if the player is controlling a mini assault chopper
int					MACammo = 0;
float				MACfuel = 0;
float				FocusSwitchTimer = 0;		//- the time before you can switch the focus entity manually
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to script processing
//----------------------------------------------------------------------------------------------------------------------------
SDL_Color FadePalette[256];				//- temp palette for fading effects
ENTITY	*ScriptEntity[50];					//- pointer to the entity being processed in the script
UCHAR ScriptProcessed[10];					//- if zero, that number script on the map hasn't been processed

char  ScriptIns[8000];						//- holds the script messages for processing the script
char  ScriptMsg[8000];						//- holds the talking message for the script being processed
char  Identifier[500];						//- the identifying string in a script that points to an entity
char  TalkMessage[80];						//- holds what the current entity is saying and is split into TalkMessage1 & 2
char  TalkMessage1[50];						//- holds the top text
char  TalkMessage2[50];						//- holds the bottom text
char  SysMsg[80];							//- holds the system message that is displayed on the screen
char  TempMessage1[50];						//- temp array holding a message
char  TempMessage2[50];						//- temp array holding a message
char  ScriptFilename[80];					//- the filename of the script about to be loaded in

int   ScriptVal[100];						//- holds the values for the script messages
int   ScriptId[100];						//- holds the entity for the message being processed
int   NumScripts, NumMsgs;					//- the number of scripts and script messages
int   ScriptCount, MsgCount;				//- the current count of the script and message being processed
int   SysMsgCount = 0;						//- the timer that determines how long the system message is being displayed for
int   TalkId = 2;							//- the id of the current entity talking
int   TalkSprite = 2;						//- the sprite number for the picture of the sprite talking in the box
int   TalkWords, OldTalkWords;				//- the number of words in a message when an entity is talking
int   Length1, Length2;						//- holds the length of text messaged
int   LoopCount = 0;						//- loop counter
int   TalkMsgCount1 = 0;					//- ????
int   TalkMsgCount2 = 0;					//- ????
int   TalkDelay = 500;						//- ????
int   TalkDelayCount = 0;					//- ????
int   NumIds = 0;							//- the number of entities defined in a script
int   ScriptWait = 0;
bool  Talking = false;						//- if true, an entity is talking
bool  ProScript = false;					//- if true, a script is being processed
bool  CutScene = false;						//- if true, the letter box is drawn for the scripts
bool  Moving = false;						//- if true, an entity being processed in the script is moving
bool  SkipInput = true;						//- if true, the player can skip the script parts by input
bool  SkipScene = false;					//- if true, skip the cut scene
float TalkCount;							//- the delay timer for how long a message is held up
long  xMove, oxMove, MoveId;
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to the player
//----------------------------------------------------------------------------------------------------------------------------
LD3BONUS	LetterBonus;					//- holds values if the letters of LARRY are captured or not
int			BonusDisplayTimer = 0;			//- ????
int			Weapon[10];						//- this array holds the types of weapons the player has
int			NumWeapons		= 2;			//- the number of weapons the player has
int			SelectedWeapon	= 1;			//- the weapon that is currently selected by the player
int			WeaponPower		= 5;			//- the power/strength of the weapon
int			OldWeaponPower	= WeaponPower;
int			Shells			= 30;			//- the number of shotgun shells the player has
int			Bullets			= 0;			//- the number of bullets the player has
int			Rockets			= 0;			//- the number of rockets the player has
int			Grenades		= 5;			//- the number of grenades the player has
int			Choppers		= 0;			//- number of mini choppers the player has
int			Singularities	= 1;
int			GrenadeThrowingTimer = 0;		//- ????
int			WeaponSelectTimer	 = 0;		//- a timer that slows down the rate at which a weapon is selected, so the
int			Difficulty		= 0;
												//- player dosen't sift through the weapons blazing fast
int			GrenadeAngle	= 30;			//- the angle at which the grenade will be thrown
int			HoldingTimer	= 0;			//- ????
int			LookTimer		= 0;			//- the timer the delays for about a second while the player is holding up or
int			Lives			= 5;			//- the number of lives the player has
int			SpeedTimer		= 0;			//- the time the player has while he/she is fast
int			PowerTimer		= 0;			//- the time the player has while his/her weapon is stronger
int			EndOfLevelTimer	= 0;			//- the time between the end of the level and loading the next
int			GameOverTimer	= 0;			//- ????
int			JumpBTimer		= 0;			//- the time the player has while he/she can jump high
int			ExtraLifeScore	= 0;			//- ????											
int			SteppedOnTouchPlate = 0;		//- set to 1 when the player steps on a touchplate so the game won't go
												//- through the same calculation again when the player steps on another
int			CrystalCount	= 0;			//- the number of crystals the player has collected
int			TotalCrystals	= 0;			//- the number of crystals in the map
int			CrystalBonusDisplayTimer = 0;	//- the timer that keeps the 'All Crystals Collected' message up for a bit
int			SlotNum = 1;					//- the slot that is selected in the new/load menu
int			Phasing = 0;					//- counter for the player phasing while transporting
int			PhaseCount = 1;					//- how much Phasing adds by

bool		ShowPlayer			= false;	//- if this flag is true, the player is drawn, else the player is invisible
bool		HoldingGrenade		= false;	//- if true, the player is getting ready to through a grenade
bool		ShowGrenadeCursor	= false;	//- if true, the grenade angle cursor is showing
bool		GotBonus			= false;	//- ????
bool		Swimming			= false;	//- true if the player is underwater and can swim
bool		HasShotgun			= true;		//- if true, the player has a shotgun
bool		HasMachinegun		= false;	//- if true, the player has a machinegun
bool		HasFlamethrower		= false;	//- if true, the player has a flamethrower
bool		HasBazooka			= false;	//- if true, the player has a bazooka
bool		InCurrent			= false;	//- if true, the player is in a current
bool		DidJump				= false;	//- if true, the player has jumped and is still in the air
												//- down before the player looks up and down
float		JumpVel			= -3;			//- the velocity at which the player jumps
float		oldJumpVel		= JumpVel;
long		Score			= 0;			//- holds the score the player has
long		ScoreDisplay	= 0;			//- the score being displayed on the screen that adds up to the actual score when
												//- the player's score changes
bool		HasWaterGun		= false;		//- player has a water gun if true
float		ShootTimer		= 0;			//- time before the player can shoot again
float		PlayerY1		= 0;
float		PlayerY2		= 0;
float		SledgeSpeed		= 0;			//- the speed at which the sledehammer is swung
bool		AddSledgeSpeed	= false;

bool		HasGreenCard	= false;
bool		HasBlueCard		= false;
bool		HasYellowCard	= false;
bool		HasRedCard		= false;
bool		DumpGreenCard	= false;		//- if true, dump the cards if the player dies
bool		DumpBlueCard	= false;
bool		DumpYellowCard	= false;
bool		DumpRedCard		= false;

int			StatusState		= 0;			//- the state of the status displays
bool		ChangeStatusState = false;
int			NumKills		= 0;
int			TotalKills		= 0;
float		MaxLife			= 100;
int			HealthAdd1		= 50;
int			HealthAdd2		= 50;
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things related to the map/level
//----------------------------------------------------------------------------------------------------------------------------
int					ScoreMap[500][100];		//- this array holds in correlation the score value for each element in the
											//- map array
int					xShift, yShift;			//- Hold how far the screen is shifted on the map
int					xShift2, yShift2;		//- shift values for player two
int					xSkyShift, xSkyShift2;	//- for shifting the background without shifting the map
int					oxShift, oyShift;		//- old shift values, incase something happens to the actual shift values, they
											//- revert back to these values
int					oxShift2, oyShift2;		//- same, but for player two
int					xShiftMin;				//- the max the player can move to the left before the screen scrolls
int					xShiftMax;				//- the max the player can move to the right before the screen scrolls
int					yShiftMin;				//- the max the player can move up before the screen scrolls
int					yShiftMax;				//- the max the player can move down before the screen scrolls
int					BossXShift = 0;			//- Where xShift should be when the player has reached a boss
int					BossYShift = 0;			//- Where yShift should be when the player has reached a boss
int					LockXShift = 0;			//- ????
int					LockYShift = 0;			//- ????
int					yLook = 0;				//- The offset of the yShift when the player is looking up or down
int					Level = 1;				//- the level number
bool				LockShift = false;		//- if this flag is set to true, the screen will not scroll
bool				EndOfLevel  = false;	//- if true, the player has reached the end of the level
LOCATION			Teleport[50];			//- holds the location of the teleporters
LOCATION			Position[10];			//- Set positions in the map
int					NumTeleports = 0;		//- the number of teleporters in the level
int					NumTeleport;			//- holds the teleport number the player is on while he is phasing out
bool				OnTeleport;				//- true if the player is on a teleporter
char				PaletteFilename[80];	//- the filename of the current palette
char				LevelMessage[80];		//- message to display at the beginning of the level and when the player respawns
char				LevelFilename[2000];	//- stores the filenames of the levels
char				LevelData[2000];		//- stores the filenames of the levels data
char				LevelName[2000];		//- level filename without the extension
char				ObjectName[20];			//- level object names (mushroom, coffee mugs, etc)
bool				Secret[4];				//- if false, the corresponding secret area has not been found yet
int					Secrets;				//- the number of secret areas found in the level
int					NumSecrets;				//- the number of secret areas in the level
bool				Shifting = false;
float				PlatformVelocity;
int					TreadMillDeath = 0;
char				TileSetFilename[80];
bool				ShowFPS = false;
bool				FPSKey = false;
int					HalfX, HalfY;
//int					Wall, Floor, Top, Left, Right;
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to the palette
//----------------------------------------------------------------------------------------------------------------------------
bool				FadeToBlack = false;	//- Fade the screen to black if true
bool				FadeToWhite = false;	//- Fade the screen to white if true
bool				FadeOutOfBlack  = false;//- Fade the screen from black to normal palette if true
bool				FadeOutOfWhite  = false;//- Fade the screen from white to normal palette if true
bool				Fading = false;			//- the palette is in the process of fading if true
bool				PaletteChange = false;  //- if a palette change happened while fading, this is true
int					red, grn, blu;			//- the temp rgb values for the fading procedures
int					PalSpeed = 5;			//- the speed at which the palette shifts
int					PalCount = 0;			//- the counter for the palette speed
bool				ShiftPalette = true;	//- if true, the last 15 colors of the palette shift
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to sound
//----------------------------------------------------------------------------------------------------------------------------
//int					sfxVolume = 96;			//- the sound volume
//int					maxMusicVolume = 196;	//- the highest the music volume can go
//int					MusicVolume = 196;		//- the music volume
//int					MusicFade = 196;		//- slowly adjust to the music volume for smooth fading
int						sfxVolume = 196;		//- the sound volume
int						maxMusicVolume = 196;		//- the highest the music volume can go
int						MusicVolume = 195;		//- the music volume
int						MusicFade = 96;			//- slowly adjust to the music volume for smooth fading
bool				WaitMusicFade = false;	//- if true, the script waits until the music has faded until it continues on
bool				LoadedBossSong = false; //- a flag that tells whether the boss song has been loaded in yet when the
											//- player reaches a boss
bool				PlayingVRockSong = false;
FMUSIC_MODULE *song1;						//- the level music that loops
FMUSIC_MODULE *mscLevelComplete;			//- played when the player completes a level
FMUSIC_MODULE *mscXahnTheme;				//- played in a cutscene with xahn
FMUSIC_MODULE *mscBoss;						//- played when the player is fighting a boss
FMUSIC_MODULE *mscVRock;
//FSOUND_SAMPLE *fstep;
//FSOUND_SAMPLE *mfstep;
//FSOUND_SAMPLE *cfstep;
FSOUND_SAMPLE *shotgun;						//- shotgun fire sound
FSOUND_SAMPLE *sndRifle;					//- alien assault rifle fire sound
FSOUND_SAMPLE *diescum;						//- player says "die scum!"
FSOUND_SAMPLE *huh;							//- played when the player jumps
FSOUND_SAMPLE *sndItemPickup;				//- played when the player picks up an item
FSOUND_SAMPLE *sndDing;						//- played when the player's score adds up
FSOUND_SAMPLE *sndBonus;					//- ????
FSOUND_SAMPLE *sndShorted;					//- played when the lasers are deactivated
FSOUND_SAMPLE *sndBoom;						//- played when something explodes
FSOUND_SAMPLE *sndBubbles;					//- played when bubbles are shot out of the bubble pods
FSOUND_SAMPLE *sndBounce;					//- played when the player jumps on a spike plant and springs upward
FSOUND_SAMPLE *sndTeleport;					//- played while the player is teleporting
FSOUND_SAMPLE *sndThunder;					//- played when the player teleports
FSOUND_SAMPLE *sndDeath[5];					//- one of the 5 deaths is played randomly when the player dies
FSOUND_SAMPLE *sndSquirt;					//- played when the player squirts the water gun
FSOUND_SAMPLE *sndPump;						//- played when the player pumps the water gun
FSOUND_SAMPLE *sndRock[3];					//- vrockmonster sounds
FSOUND_SAMPLE *sndChopper;					//- played when flying a mini assault chopper
FSOUND_SAMPLE *sndDoorUp;					//- played when a door opens
FSOUND_SAMPLE *sndDoorDown;					//- played when a door closes
int				NumDeathSounds = 3;			//- then number of death sounds
bool			PlayedDeathSound = false;
bool			PlayedEndOfLevelMusic = false;
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- things relating to input
//----------------------------------------------------------------------------------------------------------------------------

// TODO(davidgow): get rid of this
// NOTE: Copied from https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ee416627(v=vs.85)
typedef struct DIJOYSTATE {
    LONG lX;
    LONG lY;
    LONG lZ;
    LONG lRx;
    LONG lRy;
    LONG lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[32];
} DIJOYSTATE, *LPDIJOYSTATE;


const UCHAR				*Keyboard;				//- holds the keyboard input
DIJOYSTATE			JoyState;					//- holds the state of the joystick/gamepad
bool				LockInput = false;			//- if this flag is set to false, the game input will not be processed
												//- except for the escape key
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- other (classes/trig tables/etc)
//----------------------------------------------------------------------------------------------------------------------------
WINAPP				WinApp;						//- This class contains the primitives such as surfaces, palettes, etc.
LD3ENGINE			LD3;						//- This class contains all the drawing functions
float				Cosine[500], Sine[500];		//- Predefines cosine and sine tables for faster calculation
bool				EndGame = false;			//- the game will exit if this is false
SDL_Event  msg;										//- holds the windows messages
bool Active;									//- if false, the game isn't processed because the player has activated another
												//- window
DWORD active;									//- holds the active state of the window giving by the windows messages
int					xRes = 320;					//- The width of the screen in pixels
int					yRes = 240;					//- The height of the screen in pixels
int					HoldDelay = 10;				//- The delay for scrolling in the menu
float				diff = 1;
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//- custom controls
//----------------------------------------------------------------------------------------------------------------------------
UCHAR			keyJump		= SDL_SCANCODE_LALT;
UCHAR			keyJump2	= SDL_SCANCODE_SPACE;
UCHAR			keyShoot	= SDL_SCANCODE_LCTRL;
UCHAR			keyShoot2	= SDL_SCANCODE_RCTRL;
UCHAR			keyGrenade	= SDL_SCANCODE_G;
UCHAR			keyGrenade2	= 0;
UCHAR			keyAim		= 0;
UCHAR			keyAim2		= 0;
UCHAR			keyNoFlip	= SDL_SCANCODE_LSHIFT;
UCHAR			keyNoMove	= SDL_SCANCODE_RSHIFT;
UCHAR			keyNoFlip2	= 0;
UCHAR			keyNoMove2	= 0;
UCHAR			keySkip		= SDL_SCANCODE_S;

UCHAR			joyJump		= 0;
UCHAR			joyShoot	= 2;
UCHAR			joyGrenade	= 1;
UCHAR			joyAim		= 3;
UCHAR			joyNoFlip	= 4;
UCHAR			joyNoMove	= -1;//5;
UCHAR			joyNext		= 5;
UCHAR			joyPrev		= -1;
UCHAR			joyJump2	= 0;
UCHAR			joyShoot2	= 1;
UCHAR			joyGrenade2	= 2;
UCHAR			joyAim2		= 3;
UCHAR			joyNoFlip2	= 4;
UCHAR			joyNoMove2	= 5;
UCHAR			JoyOne		= 0;
UCHAR			JoyTwo		= 0;
UCHAR			dpad		= 6;	//- d-pad sensitivity(1 - very sensitive, 10 - not sensitive)


#ifndef DT_CENTER
#define DT_CENTER 1
#endif
#ifndef DT_RIGHT
#define DT_RIGHT 2
#endif
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

	


//----------------------------------------------------------------------------------------------------------------------------
//- The windows callback function, when windows sends messages to the app, they get processed here
//----------------------------------------------------------------------------------------------------------------------------
bool WindowProc(SDL_Event *evt)
{

	float		active;
	char TileSetFilename[80];

	switch(evt->type)
	{
	case SDL_QUIT:
		{
			return true;
		}
	case SDL_WINDOWEVENT:
		{
			switch(evt->window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					Active = false;
					if(WinApp.WindowMode == false) FMUSIC_SetPaused(song1, true);
				} break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					Active = true;
					if(WinApp.WindowMode == false) FMUSIC_SetPaused(song1, false);
				} break;
			}
			
			return false;
		} break;
	case SDL_KEYDOWN:
		{
			return false;			
		} break;

	}
	//Active = false;

	return false;
}
//----------------------------------------------------------------------------------------------------------------------------
//- End windows callback
//----------------------------------------------------------------------------------------------------------------------------


//=================================================================================================
//= void Init(HINSTANCE hinstance)
//= Initialize the game by setting the graphics mode, initializing input devices, loading graphics
//= and sounds, and zeroing buffers
//=================================================================================================
void Init()
{	
	//- Remove the cursor
		//ShowCursor(false);

		LD3.xRes = xRes;
		LD3.yRes = yRes;
		HalfX = xRes >> 1;
		HalfY = yRes >> 1;

	//--------------------------------------------
	//- Initialize the graphics mode
	//--------------------------------------------		
		if(WinApp.WindowMode)
			WinApp.InitWindow(xRes, yRes, 0, 0);
		else
			WinApp.InitWindow(xRes, yRes, SDL_WINDOW_FULLSCREEN_DESKTOP, 0);
		WinApp.InitDirectDraw(xRes, yRes, 8);
		WinApp.CreateSurfaces();
	//--------------------------------------------
	//- End initializing the graphics mode
	//--------------------------------------------

	//--------------------------------------------
	//- Initialize the input
	//--------------------------------------------
#if 0
		WinApp.InitDirectInput();
		WinApp.InitKeyboard();
		WinApp.InitJoy();		
#endif
	//--------------------------------------------
	//- End Initializing the input
	//--------------------------------------------

	//--------------------------------------------
	//- Set the map parameters
	//--------------------------------------------
		xShift = 0; yShift = 0;	
	//--------------------------------------------
	//- End map settings
	//--------------------------------------------
	
		Difficulty = NORMAL;

	//--------------------------------------------
	//- Load the graphics global to all the levels
	//--------------------------------------------
		LD3.LoadTileSet(&WinApp, "gfx/ld3objs.put", 256);
		//LD3.LoadTileSet(&WinApp, "gfx/ld3scrts.put", 417);
		//LD3.LoadTileSet(&WinApp, "gfx/ld3scrpt.put", 426);
		LD3.LoadTileSet(&WinApp, "gfx/ld3gplts.put", 500);		

		LD3.LoadSprite(&WinApp, "gfx/lary.put", LARRY_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/bones.put", FLETCHER_SPRITE_SET, 40);
		LD3.LoadSprite(&WinApp, "gfx/mark.put", MARK_SPRITE_SET, 60);
		LD3.LoadSprite(&WinApp, "gfx/rusty.put", RUSTY_SPRITE_SET, 80);
		LD3.LoadSprite(&WinApp, "gfx/xahn.put", XAHN_SPRITE_SET, 100);
		LD3.LoadSprite(&WinApp, "gfx/general.put", GENERAL_SPRITE_SET, 120);
		LD3.LoadSprite(&WinApp, "gfx/ld3talk.put", DIALOG_SPRITE_SET, 140);
		LD3.LoadSprite(&WinApp, "gfx/msonic2.put", SONIC_MONSTER_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/newrock.put", ROCK_MONSTER_SPRITE_SET, 30);
		LD3.LoadSprite(&WinApp, "gfx/spkehead.put", ICED_SPIKEHEAD_SPRITE_SET, 50);
		LD3.LoadSprite(&WinApp, "gfx/ld3fly.put", FLY_MONSTER_SPRITE_SET, 60);
		LD3.LoadSprite(&WinApp, "gfx/aldog.put", FLY_MONSTER_SPRITE_SET, 80);
		LD3.LoadSprite(&WinApp, "gfx/spider.put", SPIDER_SPRITE_SET, 100);
		LD3.LoadSprite(&WinApp, "gfx/ld3jello.put", JELLY_BLOB_SPRITE_SET, 120);
		LD3.LoadSprite(&WinApp, "gfx/gblob.put", GREEN_BLOB_SPRITE_SET, 140);
		LD3.LoadSprite(&WinApp, "gfx/ld3rock.put", VROCK_MONSTER_SPRITE_SET, 140);
		LD3.LoadSprite(&WinApp, "gfx/ld3fire.put", YELLOW_FIRE_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/ld3exp.put", EXPLOSION_SPRITE_SET, 40);
		LD3.LoadSprite(&WinApp, "gfx/blobmine.put", BLOB_MINE_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/ld3troop.put", TROOP_SPRITE_SET, 10);
		LD3.LoadSprite(&WinApp, "gfx/chopper.put", MAC_SPRITE_SET, 30);
		LD3.LoadSprite(&WinApp, "gfx/megafly.put", MEGAFLY_SPRITE_SET, 35);
		LD3.LoadSprite(&WinApp, "gfx/spider3.put", BIGSPIDER_SPRITE_SET, 85);
		LD3.LoadSprite(&WinApp, "gfx/spider2.put", BIGSPIDER_SPRITE_SET, 90);
		LD3.LoadSprite(&WinApp, "gfx/ld3rat.put", RAT_SPRITE_SET, 95);
		LD3.LoadSprite(&WinApp, "gfx/ld3doors.put", DOORS_SPRITE_SET, 100);
		//LD3.LoadSprite(&WinApp, "gfx/bigboss1.put", BOSS_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/ld3icons.put", ICON_SPRITE_SET, 40);
		
		LD3.LoadFont("gfx/font1.put");

		//- Load and set the palette
			LD3.LoadPalette(&WinApp, PaletteFilename);	
			WinApp.CreatePalette();
			WinApp.SetPalette();
	//--------------------------------------------
	//- End graphics loading
	//--------------------------------------------
	
	
	//--------------------------------------------
	//- Load the sounds global to all the levels
	//--------------------------------------------
		
		//- Initialize the sound system
#ifdef WITH_FMOD
#ifdef TARGET_OS_LINUX
			FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);
#endif
			FSOUND_Init(44100,32,0);
#else
			Mix_Init(MIX_INIT_MOD | MIX_INIT_OGG | MIX_INIT_MP3);
			Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
#endif

		//fstep   = FSOUND_Sample_Load(FSOUND_FREE, "sfx/footstep3.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0);
		//mfstep  = FSOUND_Sample_Load(FSOUND_FREE, "sfx/footstep2.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0);
		//cfstep  = FSOUND_Sample_Load(FSOUND_FREE, "sfx/footstep.wav",  FSOUND_NORMAL | FSOUND_LOOP_OFF,0);
		//- Load in the sounds
		shotgun			= FSOUND_Sample_Load(FSOUND_FREE, "sfx/shotgun.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndRifle		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/gun3.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		diescum			= FSOUND_Sample_Load(FSOUND_FREE, "sfx/yeah.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		huh				= FSOUND_Sample_Load(FSOUND_FREE, "sfx/huh.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndItemPickup	= FSOUND_Sample_Load(FSOUND_FREE, "sfx/item.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndDing			= FSOUND_Sample_Load(FSOUND_FREE, "sfx/ding2.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndBonus		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/bonus.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndShorted		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/shorted.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);
		sndBoom			= FSOUND_Sample_Load(FSOUND_FREE, "sfx/boom5.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndBubbles		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/bubbles.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndBounce		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/plantspring.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndTeleport		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/teleport.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndThunder		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/thunder.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);		
		sndDeath[0]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/death1.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndDeath[1]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/death2.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndDeath[2]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/death3.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndSquirt		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/squirt.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndPump			= FSOUND_Sample_Load(FSOUND_FREE, "sfx/pump.wav", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndRock[0]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/rock1.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndRock[1]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/rock2.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndRock[2]		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/rock3.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndDoorUp		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/doorup.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndDoorDown		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/doordown.mp3", FSOUND_NORMAL | FSOUND_LOOP_OFF, 0,0);
		sndChopper		= FSOUND_Sample_Load(FSOUND_FREE, "sfx/chopper.wav", FSOUND_NORMAL | FSOUND_LOOP_BIDI, 0,0);
		
		//mscLevelComplete = FMUSIC_LoadSongEx("music/victory.it", 0, 0, FSOUND_LOOP_OFF, NULL, NULL);
		mscLevelComplete = FMUSIC_LoadSongEx("music/bonus.mod", 0, 0, FSOUND_LOOP_OFF, NULL, 0);
		mscXahnTheme = FMUSIC_LoadSongEx("music/xahn.it", 0, 0, FSOUND_LOOP_OFF, NULL, 0);
		mscVRock = FMUSIC_LoadSongEx("music/vrock.mod", 0, 0, 0, NULL, 0);
						
		//- Set the master volume for the bonus song
			FMUSIC_SetMasterVolume(song1, maxMusicVolume);
			FSOUND_SetSFXMasterVolume(sfxVolume);

	//--------------------------------------------
	//- End loading sounds	
	//--------------------------------------------
		
	
	//--------------------------------------------
	//- Zero out the buffers
	//--------------------------------------------
		memset(ScriptMsg, 0, 8000);
		memset(ScriptIns, 0, 2000);
		memset(SysMsg, 0, 80);
		memset(ScoreMap, 0, sizeof(int)*50000);
		memset(ScriptProcessed, 0, 10);
		memset(ScriptFilename, 0, 80);
		memset(Cosine, 0, sizeof(float)*360);
		memset(Sine, 0, sizeof(float)*360);
		//memset(vCosine, 0, sizeof(float)*360);
		//memset(vSine, 0, sizeof(float)*360);
		//memset(FadePalette, 0, sizeof(PALETTEENTRY)*256);
		memcpy(FadePalette, WinApp.palette, sizeof(SDL_Color)*256);		
		memset(PaletteFilename, 0, 80);
		memset(LevelFilename, 0, 2000);
		memset(LevelData, 0, 2000);
		memset(LevelName, 0, 2000);
		memset(ObjectName, 0, 20);
	//--------------------------------------------
	//- End buffer clearing
	//--------------------------------------------

	//- Construct trig tables
		for(int i = 0; i <= 499; i++){
			Cosine[i]  = (float)cos((float)i*PI/180);
			Sine[i]    = (float)sin((float)i*PI/180);				
		}
		//for(i = 0; i <= 359; i++){
		//	vCosine[i] = Cosine[i] / (float)sqrt(pow(Sine[i],2));
		//	vSine[i]   = Sine[i] / (float)sqrt(pow(Cosine[i],2));
		//}

	//- Set the bonus letters to false
		LetterBonus.L  = false;
		LetterBonus.A  = false;
		LetterBonus.R1 = false;
		LetterBonus.R2 = false;
		LetterBonus.Y  = false;

	//- Set the starting weapons
		Weapon[0] = SLEDGEHAMMER;
		Weapon[1] = SHOTGUN;
		Weapon[2] = MACHINEGUN;
		Weapon[3] = MAC;		//- Mini Assault Chopper
		//Weapon[4] = SINGULARITY;//- Singularity Discharge Rifle
		if(Difficulty == JUSTICE){
			NumWeapons = 0;
			SelectedWeapon = 0;
		}
		else
			NumWeapons = 3;
		
	//- Randomize the timer
		srand( (unsigned)time( NULL ) );

	//- Set Larry's identity
		Player = &Monster[0];		
		Player->Init();
		Player->id = LARRY;
		Player->HitBox.left		= 3;
		Player->HitBox.right	= 16;
		Player->HitBox.top		= 1;
		Player->HitBox.bottom	= 19;

		xShiftMin = xRes/2-10;
		xShiftMax = xRes/2+10;
		yShiftMin = yRes/2-20;
		yShiftMax = yRes/2+20;
		
		Active = true;

		FocusEntity = Player;

		strcpy(PaletteFilename, "gfx/palettes/grad.pal");

	//- read in the levellist.dat file for info on the filenames of the levels

		int  n = 0;
		char word1[40], word2[40];
		char *lv, *dt;
		ifstream lfile("maps/levellist.dat", ios::binary);			
			while(NO_ONE_CARES){

				memset(word1, 0, 40);
				memset(word2, 0, 40);

				ReadWord(&lfile, word1);

				if(strcmp(word1, "END") == 0) break;

				lv = &LevelFilename[n*80];
				dt = &LevelData[n*80];
				strcpy(&LevelName[n*80], word1);
				n++;

				strcpy(lv, "maps/"); strcpy(dt, "maps/");
				strcat(lv, word1);   strcat(dt, word1);
				strcat(lv, ".map");  strcat(dt, ".dat");
			}
		lfile.close();

	//- load in the controls
			//TODO(davidgow): Load Controls
#if 0
		ifstream sfile;

		sfile.open("controls.dat", ios::binary);

			sfile.get(keyJump);
			sfile.get(keyJump2);
			sfile.get(keyShoot);
			sfile.get(keyShoot2);
			sfile.get(keyGrenade);
			sfile.get(keyGrenade2);
			sfile.get(keyAim);
			sfile.get(keyAim2);
			sfile.get(keyNoFlip);
			sfile.get(keyNoFlip2);
			sfile.get(keyNoMove);
			sfile.get(keyNoMove2);

			sfile.get(joyJump);
			sfile.get(joyShoot);
			sfile.get(joyAim);
			sfile.get(joyGrenade);
			sfile.get(joyNoFlip);
			sfile.get(joyNoMove);
			sfile.get(dpad);

		sfile.close();
#endif
}


//=================================================================================================
//= void Shutdown()
//= Free up the resources and prepare to exit the program
//=================================================================================================
void Shutdown()
{
	
	//--------------------------------------------
	//- Shutdown Larry The Dinosaur III
	//--------------------------------------------
		FSOUND_Sample_Free(sndDoorDown);	
		FSOUND_Sample_Free(sndDoorUp);
		FSOUND_Sample_Free(sndChopper);
		FSOUND_Sample_Free(sndRock[2]);
		FSOUND_Sample_Free(sndRock[1]);	
		FSOUND_Sample_Free(sndRock[0]);
		FSOUND_Sample_Free(sndPump);	
		FSOUND_Sample_Free(sndSquirt);
		FSOUND_Sample_Free(sndDeath[2]);
		FSOUND_Sample_Free(sndDeath[1]);
		FSOUND_Sample_Free(sndDeath[0]);
		FSOUND_Sample_Free(sndThunder);
		FSOUND_Sample_Free(sndTeleport);
		FSOUND_Sample_Free(sndBubbles);
		FSOUND_Sample_Free(sndBoom);
		FSOUND_Sample_Free(sndShorted);
		FSOUND_Sample_Free(sndBonus);
		FSOUND_Sample_Free(sndDing);
		FSOUND_Sample_Free(sndItemPickup);
		FSOUND_Sample_Free(huh);
		FSOUND_Sample_Free(diescum);
		FSOUND_Sample_Free(sndRifle);
		FSOUND_Sample_Free(shotgun);
		//FSOUND_Sample_Free(cfstep);
		//FSOUND_Sample_Free(mfstep);
		//FSOUND_Sample_Free(fstep);
		FMUSIC_FreeSong(mscVRock);
		FMUSIC_FreeSong(mscBoss);
		FMUSIC_FreeSong(mscXahnTheme);
		FMUSIC_FreeSong(mscLevelComplete);
		FMUSIC_FreeSong(song1);		
		//- Restore the cursor
			SDL_ShowCursor(true);
	//--------------------------------------------
	//- End shutting down Larry The Dinosaur III
	//--------------------------------------------	
}


//=================================================================================================
//= void LD3Main(HINSTANCE hinstance)
//= This function processes the main loop of the game
//= This includes calling the functions that draw and calculate the entities
//=================================================================================================
void LD3Main()
{	
	Init();
	int FPS, FPSCount;
	FPS = 0; FPSCount = 0;
	SDL_Rect DestArea;

	//--------------------------------------------
	//- Declarations
	//--------------------------------------------
		DWORD				dwFrameStartTime;
		uint64_t		FrameStartTime;
		uint64_t		TickCount;
		uint64_t		Frequency;
		int			TimeDeductCount = 0;
	//--------------------------------------------
	//- End Declarations
	//--------------------------------------------

	DestArea.y = 0;
	DestArea.h = yRes;
	DestArea.x = 0;
	DestArea.w = xRes;

	//NOTE(davidgow): We need to initialise (allocate) the keyboard array
	//really early, as otherwise it's read when still 0.
	Keyboard = SDL_GetKeyboardState(0);
	

	Player->uani = 10;
			//EntityTalk("Welcome ", 2);
			//EntityTalk("It's time to kick some alien trash yet again...", 2);
	
	LD3.ClearBuffer(&WinApp, 0);
	//song1 = FMUSIC_LoadSong("music/jnk.it");
	//FMUSIC_PlaySong(song1);
	//LD3.LoadSplashScreen(&WinApp, "gfx/jnk.bmp", 6400);
	//FMUSIC_StopSong(song1);

	Frequency = SDL_GetPerformanceFrequency();

	while(!EndGame){
		
		Level = -1;
		NextLevel(-1);
		mnuTitle();
		//- Load in the starting level
		if(EndGame == false) NextLevel(Level);

		dwFrameStartTime = SDL_GetTicks();		
		FrameStartTime = SDL_GetPerformanceCounter();

		LD3.FlipSurfaces(&WinApp);
		LD3.ClearBuffer(&WinApp, 0);
		LD3.FlipSurfaces(&WinApp);
		LD3.ClearBuffer(&WinApp, 0);
		LD3.FlipSurfaces(&WinApp);
				
	while(!EndGame)
	{
		//----------------------------------------------
		//- Get any windows messages and process them
		//----------------------------------------------
			if(SDL_PollEvent(&msg))
			{
				if(msg.type == SDL_QUIT)
					break;
				WindowProc(&msg);
			}
		//----------------------------------------------
		//- End getting windows messages
		//----------------------------------------------		
		
			//Active = false;
			if(Active){
				RenderScene();
				if(ShowFPS){
#if 0
					sprintf(cFPS, "FPS: %d", FPS);
					WriteText(0, 0, cTemp, 15, 0);
#endif
				}
				ProcessPlayer();
				ProcessItems();
				RunScript();
			}
			
			if(EndGame){
				EndGame = false;
				break;
			}

		//- Flip the surfaces letting the player view the updated scene
			//if(WinApp.WindowMode) while(GetTickCount() - FrameStartTime < 8.66f);
			//while(GetTickCount()-dwFrameStartTime < 20);
			
			if(Active){
				//LD3.WaitForRetrace(&WinApp);
				//WinApp.lpddsprimary->Blt(&DestArea, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
				LD3.FlipSurfaces(&WinApp);
				//LD3.FlipSurfaces(&WinApp);
			}

			//QueryPerformanceCounter(&TickCount);
			//while(float(TickCount.QuadPart-FrameStartTime.QuadPart) / (float)Frequency.QuadPart < 0.034f) QueryPerformanceCounter(&TickCount);

			
			FPSCount++;
			if(SDL_GetTicks()-dwFrameStartTime >= 1000){
				FPS = FPSCount;
				FPSCount = 0;
				dwFrameStartTime = SDL_GetTicks();
			}
			
			//TickCount = GetTickCount();
			//diff = float(TickCount-FrameStartTime) / float(1000.0f/75.0f);
			//FrameStartTime = TickCount;

			TickCount = SDL_GetPerformanceCounter();
			diff = float(TickCount-FrameStartTime)  / (float)Frequency;//float(1000.0f/75.0f);
			//diff = diff/float(1000.0f/75.0f);
			diff *= 75;
			
			FrameStartTime = TickCount;

		
		//--------------------------------------------
		//- If finished level or died, don't process
		//- input accept for the escape key
		//--------------------------------------------
			if(EndOfLevelTimer == 0 && GameOverTimer == 0 && Active && Phasing == false)
				ProcessInput();
			else{
				Player->speed = 0;
				Player->velocity = 0;
			}
			//TODO(davidgow): Really should rework this to use Keycodes, not a global.
			//WinApp.lpdikey->GetDeviceState(256, Keyboard);
			Keyboard = SDL_GetKeyboardState(0);
			//if(JoyStickEnabled){
			//	WinApp.lpdijoy->Poll();
			//	WinApp.lpdijoy->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			//}
			if(Keyboard[SDL_SCANCODE_ESCAPE]) break;	
		//--------------------------------------------
		//- End processing input
		//--------------------------------------------
			
			
		
		
		//- Wait until 1/60 of a second has passed
			//while(GetTickCount() - FrameStartTime < 100);
			
	}
	EnterHiScore();
	}
	Shutdown();
}

//=================================================================================================
//= void Flash(int col)
//= Flash the screen with the given color by chaging all the colors to the palette
//=================================================================================================
void Flash(int red, int grn, int blu)
{
	for(int n = 0; n <= 255; n++){
		FadePalette[n].r   = (BYTE)red;
		FadePalette[n].g = (BYTE)grn;
		FadePalette[n].b  = (BYTE)blu;
	}
	SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
	FadeOutOfWhite = true;
	Fading = true;
}

//=================================================================================================
//= bool ProcessInput()
//= Take in the input from the mouse and gamepad and process what's needed
//= Return true if the escape key is pressed
//=================================================================================================
bool ProcessInput()
{
	//------------------------------------------------
	//- Declarations
	//------------------------------------------------
		int		mp, mp2;
		int		px, py;
		int		mx1, mx2;
		int		my1, my2;
		int		offset;
		long	mpt;
		static int  SayCount = 0;
		bool	result = false;
		char	Text1[40];
		char	Text2[40];
	//------------------------------------------------
	//- End Declarations
	//------------------------------------------------
	
	//------------------------------------------------
	//- Read the input from the keyboard and gamepad
	//------------------------------------------------
		Keyboard = SDL_GetKeyboardState(0);
#ifdef HAVE_JOYSTICK
		if(WinApp.JoyStickEnabled){
			WinApp.lpdijoy[JoyOne]->Poll();
			WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
		}
#endif
	//------------------------------------------------
	//- End reading input
	//------------------------------------------------

	//------------------------------------------------
	//------------------------------------------------
	//-- Check the input
	//------------------------------------------------
	//------------------------------------------------
		//- If the escape key is pressed, return true which
		//- signals the outer function to exit the game
			if(Keyboard[SDL_SCANCODE_ESCAPE]) result = true;

		//- Take screenshot if the required key is pressed
			if(Keyboard[SDL_SCANCODE_F12]) LD3.TakeScreenshot(&WinApp);

			if(LockInput){
				if(Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_LALT] || Keyboard[SDL_SCANCODE_SPACE] || JoyState.rgbButtons[0] || JoyState.rgbButtons[2]){
					if(SkipInput){
						TalkCount = 1000;
						TalkMsgCount1 = Length1;
						TalkMsgCount2 = Length2;
						TalkDelayCount = TalkDelay;
						SkipInput = false;						
					}					
				}
				else SkipInput = true;
				if(Keyboard[keySkip]) SkipScene = true;
			}
			else
				SkipInput = true;
			
			
		if(LockInput == false){

			if(ControllingMAC){
				if(Keyboard[SDL_SCANCODE_RIGHT] || JoyState.lX > dpad){
#ifndef WITH_NO_MOVE
				MACunit->x += 1.5f*diff;
#else
				if(!JoyState.rgbButtons[joyNoMove] && !Keyboard[keyNoMove] && !Keyboard[keyNoMove2]) MACunit->x += 1.5f*diff;
#endif
				if(!JoyState.rgbButtons[joyNoFlip] && !Keyboard[keyNoFlip] && !Keyboard[keyNoFlip2]) MACunit->xFlip = false;}
				if(Keyboard[SDL_SCANCODE_LEFT]  || JoyState.lX < -dpad){
#ifndef WITH_NO_MOVE
				MACunit->x -= 1.5f*diff;
#else
				if(!JoyState.rgbButtons[joyNoMove] && !Keyboard[keyNoMove] && !Keyboard[keyNoMove2]) MACunit->x -= 1.5f*diff;
#endif
				if(!JoyState.rgbButtons[joyNoFlip] && !Keyboard[keyNoFlip] && !Keyboard[keyNoFlip2]) MACunit->xFlip = true;}
				if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad) MACunit->y += 1.0f*diff;
				if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad) MACunit->y -= 1.0f*diff;

				if((Keyboard[keyShoot] || Keyboard[keyShoot2] || JoyState.rgbButtons[joyShoot]) && MACammo > 0 && MACunit->lani == MACunit->lstart){
					MACunit->lani = MACunit->lstart + 1;
					if(MACunit->xFlip)
						FireProjectile(MACunit->x, MACunit->y+9, 180, 10, MACHINEGUN, MACunit, 0);
					else
						FireProjectile(MACunit->x, MACunit->y+9, 0, 10, MACHINEGUN, MACunit, 0);

					FSOUND_PlaySound(FSOUND_FREE, sndRifle);
					MACammo--;
				}
				if(Keyboard[keyJump] || Keyboard[keyJump2] || JoyState.rgbButtons[joyJump]) MACfuel = 0;
			}
			else{

				
			//------------------------------------------------
			//- Check for moving input
			//------------------------------------------------
				if(ShowGrenadeCursor == false){					
					if(Keyboard[SDL_SCANCODE_RIGHT] || JoyState.lX > dpad){
#ifndef WITH_NO_MOVE
						Player->speed += (Player->groundfriction*2)*diff;//.1f;
#else
						if(!JoyState.rgbButtons[joyNoMove] && !Keyboard[keyNoMove] && !Keyboard[keyNoMove2]) Player->speed += (Player->groundfriction*2)*diff;//.1f;
#endif
						if(!JoyState.rgbButtons[joyNoFlip] && !Keyboard[keyNoFlip] && !Keyboard[keyNoFlip2]) Player->xFlip = false;}
						//Player->speed += Player->groundfriction*2;//.1f;			
						//Player->xFlip = false;}
					if(Keyboard[SDL_SCANCODE_LEFT]  || JoyState.lX < -dpad){
#ifndef WITH_NO_MOVE
						Player->speed -= (Player->groundfriction*2)*diff;//.1f;
#else
						if(!JoyState.rgbButtons[joyNoMove] && !Keyboard[keyNoMove] && !Keyboard[keyNoMove2]) Player->speed -= (Player->groundfriction*2)*diff;//.1f;
#endif
						if(!JoyState.rgbButtons[joyNoFlip] && !Keyboard[keyNoFlip] && !Keyboard[keyNoFlip2]) Player->xFlip = true;}
						//Player->speed -= Player->groundfriction*2;//.1f;
						//Player->xFlip = true;}
				}				
			//------------------------------------------------
			//- End moving input
			//------------------------------------------------
			
				
				if(ShowGrenadeCursor == true){
					if(JoyState.lX != 0 || Keyboard[SDL_SCANCODE_LEFT] || Keyboard[SDL_SCANCODE_RIGHT]){
						if(Player->xFlip){
							if(JoyState.lX < -dpad || Keyboard[SDL_SCANCODE_LEFT]) GrenadeAngle --;
							if(JoyState.lX > dpad || Keyboard[SDL_SCANCODE_RIGHT]) GrenadeAngle ++;
						}else{
							if(JoyState.lX < -dpad || Keyboard[SDL_SCANCODE_LEFT]) GrenadeAngle ++;
							if(JoyState.lX > dpad || Keyboard[SDL_SCANCODE_RIGHT]) GrenadeAngle --;
						}
						if(GrenadeAngle < 0) GrenadeAngle = 0;
						if(GrenadeAngle > 90) GrenadeAngle = 90;

						//ShowGrenadeCursor = true;
					}
				}

				ShowGrenadeCursor = false;
				
			
			//------------------------------------------------
			//- Check for jump input
			//------------------------------------------------
				if((Keyboard[keyJump] || Keyboard[keyJump2] || JoyState.rgbButtons[joyJump]) &&( (Player->friction == Player->groundfriction && Player->velocity >= 0) || (Swimming == true && Player->uani == 6))){
					
					if(JoyState.lY > dpad || Keyboard[SDL_SCANCODE_DOWN]){
						mpt = int(Player->x)/20+int(Player->y+40)/20*MAPWIDTH+55000;
						mp = LD3.Map[mpt];
						mp2 = LD3.Map[mpt+1];
						if((mp <= 11 || mp > 40) && (mp2 <= 11 || mp2 > 40)){
							Player->y += 3*diff;
							Player->velocity = Player->gravity*4;
						}
					}
					else{
						Player->velocity = JumpVel; Player->friction = Player->airfriction;
						DidJump = true;						
						FSOUND_PlaySound(FSOUND_FREE,huh);
					}
				}
			//------------------------------------------------
			//- End jump input
			//------------------------------------------------


			//------------------------------------------------
			//- Check for look input
			//------------------------------------------------
				if(Swimming == false){
					if((JoyState.lY > dpad || Keyboard[SDL_SCANCODE_DOWN]) && Player->velocity == 0){
						LookTimer++;
						if(LookTimer >= 60)	yLook += 2;
						if(yLook > 100) yLook = 100;
					}
					else if((JoyState.lY < -dpad || Keyboard[SDL_SCANCODE_UP]) && Player->velocity == 0){
						LookTimer++;
						if(LookTimer >= 60) yLook -= 2;
						if(yLook < -80) yLook = -80;
					}
					else{
						LookTimer = 0;
						if(yLook < 0) yLook += 2;
						if(yLook > 0) yLook -= 2;
					}
				}
				else{
					if(JoyState.lY > dpad || Keyboard[SDL_SCANCODE_DOWN]){
						Player->velocity += 0.05f;
						if(Player->velocity > 1.5f) Player->velocity = 1.5f;
					}
					if(JoyState.lY < -dpad || Keyboard[SDL_SCANCODE_UP]){
						Player->velocity -= 0.1f;
					}
				}
			//------------------------------------------------
			//- End look input
			//------------------------------------------------
			
			//------------------------------------------------
			//- Check for input for throwing grenades
			//------------------------------------------------
				//- Decrease grenade timer
					GrenadeThrowingTimer -= 1;
					if(GrenadeThrowingTimer <= 0) GrenadeThrowingTimer = 0;

				if((Keyboard[keyGrenade] || Keyboard[keyGrenade2] || JoyState.rgbButtons[joyGrenade]) && GrenadeThrowingTimer <= 0 && Grenades > 0) HoldingGrenade = true;

				if(HoldingGrenade){
					HoldingTimer += 1;
					if(HoldingTimer > 80) HoldingTimer = 80;
					ShowGrenadeCursor = true;
				}

				if(Keyboard[keyGrenade] == 0 && Keyboard[keyGrenade2] == 0 && JoyState.rgbButtons[joyGrenade] == 0 && HoldingGrenade == true){
					if(Player->xFlip)
						FireProjectile(Player->x-9, Player->y-7, 180-GrenadeAngle, float(HoldingTimer>>4), GRENADE, Player, 7);
					else
						FireProjectile(Player->x+9, Player->y-7, GrenadeAngle, float(HoldingTimer>>4), GRENADE, Player, 7);
					
					GrenadeThrowingTimer = 50;
					HoldingGrenade = false;
					Grenades -= 1;
					HoldingTimer = 0;
					ShowGrenadeCursor = true;
				}

				if(Keyboard[keyAim] || Keyboard[keyAim2] || JoyState.rgbButtons[joyAim]) ShowGrenadeCursor = true;
			//------------------------------------------------
			//- End Grenade Input
			//------------------------------------------------
			

			WeaponSelectTimer--;
			if(WeaponSelectTimer < 0) WeaponSelectTimer = 0;

			//-------------------------------------------------
			//- Check for input that makes Larry switch weapons
			//- If so, make Larry switch to the next of last
			//- weapon based on the input
			//- Set a timer to count down so it takes a little
			//- time before Larry can switch the weapon again
			//- to prevent switching too fast
			//-------------------------------------------------
				if((Keyboard[SDL_SCANCODE_RIGHTBRACKET] || JoyState.rgbButtons[joyNext]) && WeaponSelectTimer == 0){
					SelectedWeapon += 1;
					if(SelectedWeapon > NumWeapons)	SelectedWeapon = 0;
					if(SelectedWeapon < 0)			SelectedWeapon = NumWeapons;

					if(Weapon[SelectedWeapon] == SLEDGEHAMMER)	{Player->uani = 50; WeaponPower = 3;}
					if(Weapon[SelectedWeapon] == SHOTGUN)		{Player->uani = 10; WeaponPower = 5;}
					if(Weapon[SelectedWeapon] == MACHINEGUN)	{Player->uani = 17; WeaponPower = 5;}
					if(Weapon[SelectedWeapon] == MAC)			{Player->uani = 40; WeaponPower = 0;}
					if(Weapon[SelectedWeapon] == SINGULARITY)	{Player->uani = 41; WeaponPower = 0;}

					ShootTimer = 0;
					WeaponSelectTimer = 30;	
				}
				if(Keyboard[SDL_SCANCODE_LEFTBRACKET] && WeaponSelectTimer == 0){
					SelectedWeapon -= 1;
					if(SelectedWeapon > NumWeapons)	SelectedWeapon = 0;
					if(SelectedWeapon < 0)			SelectedWeapon = NumWeapons;

					if(Weapon[SelectedWeapon] == SLEDGEHAMMER)	{Player->uani = 50; WeaponPower = 3;}
					if(Weapon[SelectedWeapon] == SHOTGUN)		{Player->uani = 10; WeaponPower = 5;}
					if(Weapon[SelectedWeapon] == MACHINEGUN)	{Player->uani = 17; WeaponPower = 5;}
					if(Weapon[SelectedWeapon] == MAC)			{Player->uani = 40; WeaponPower = 0;}
					if(Weapon[SelectedWeapon] == SINGULARITY)	{Player->uani = 41; WeaponPower = 0;}
					
					ShootTimer = 0;
					WeaponSelectTimer = 30;	
				}
			//-------------------------------------------------
			//- End checking input for weapon switch
			//-------------------------------------------------
			
			//------------------------------------------------
			//- If Larry has jumped and the jump key isn't
			//- pressed, make him start to fall
			//- This is so that the play can control how high
			//- Larry is to jump
			//------------------------------------------------
				if((Player->velocity < 0 && (DidJump && ((Keyboard[keyJump] == 0 && Keyboard[keyJump2] == 0) && JoyState.rgbButtons[0] == 0))) || (Player->velocity < 0 && DidJump == false)){
					Player->velocity /= 1.1f;
				}
			//------------------------------------------------
			//- End controlled jump input
			//------------------------------------------------

			//------------------------------------------------
			//- If Larry is shooting, process his sounds and
			//- animations
			//------------------------------------------------
				if(Player->shoot){
					switch(Weapon[SelectedWeapon])
					{
					case SLEDGEHAMMER:
						Player->uani += SledgeSpeed*diff;
						if(Player->uani >= 55){
							Player->uani = 50;
							Player->shoot = false;							
						}
						else if(Player->uani >= 52){
							offset = (int(54-Player->uani)<<2);
							if(Player->xFlip == false){
								px = Player->x+15-offset;
								py = Player->y+9-offset;
							}
							else{
								px = Player->x+(int(54-Player->uani)<<2);
								py = Player->y+9-offset;
							}
							for(int i = 1; i <= NumMonsters; i++){
								mx1 = Monster[i].x+Monster[i].HitBox.left;
								mx2 = Monster[i].x+Monster[i].HitBox.right;
								my1 = Monster[i].y+Monster[i].HitBox.top;
								my2 = Monster[i].y+Monster[i].HitBox.bottom;
								if(px >= mx1 && px <= mx2 && py >= my1 && py <= my2){
									Monster[i].life -= (1+fabs(Player->speed))*(SledgeSpeed*5);
									Splatter(px, py, 20, 0, Monster[i].BloodCol);
								}
							}
						}
						break;

					case SHOTGUN:
						Player->uani += 0.2f*diff;
						//if(HasWaterGun) if((int)Player->uani == 13 || (int)Player->uani == 15) FSOUND_PlaySound(FSOUND_FREE, sndPump);
						if(Player->uani > 16){
							Player->uani = 10;
							Player->shoot = false;							
							if(SayCount == 0) FSOUND_PlaySound(FSOUND_FREE,diescum);
							SayCount++;
							if(SayCount > 10) SayCount = 0;							
						}
						break;
					
					case MACHINEGUN:
						Player->uani += .4f*diff;
						if(Player->uani >= 20){
							Player->uani = 17;
							Player->shoot = false;							
							if(SayCount == 0) FSOUND_PlaySound(FSOUND_FREE,diescum);
							SayCount++;
							if(SayCount > 50) SayCount = 0;
						}
						break;
					
					case MAC:

						MACunit = CreateNewMAC();
						ControllingMAC = true;
						FocusEntity = MACunit;
						
						FSOUND_PlaySound(5, sndChopper);
						
						Player->shoot = false;

						break;

					case SINGULARITY:
						Player->uani += .4f*diff;
						if(Player->uani >= 43){
							Player->uani = 41;
							Player->shoot = false;							
							//if(SayCount == 0) FSOUND_PlaySound(FSOUND_FREE,diescum);
							//SayCount++;
							//if(SayCount > 50) SayCount = 0;
						}
						break;
					
					}
				}
			//------------------------------------------------
			//- End processing Larry shooting
			//------------------------------------------------
			
			//------------------------------------------------
			//- Check for the input to shoot
			//- If so, make Larry shoot and start his
			//- animation for shooting
			//------------------------------------------------
				if((Keyboard[keyShoot] || Keyboard[keyShoot2] || JoyState.rgbButtons[joyShoot]) && (Player->uani == 10 || Player->uani == 16 || Player->uani == 17 || Player->uani == 19 || Player->uani == 40 || Player->uani == 41 || Player->uani == 50))
				{					
						
						if(Weapon[SelectedWeapon] == SHOTGUN && Shells > 0){
							if(HasWaterGun){
								if(Player->xFlip)
									Squirt(int(Player->x-11), int(Player->y+10), 13, -2-rand()%2+Player->speed, 72);									
								else									
									Squirt(int(Player->x+17), int(Player->y+10), 13, 2+rand()%2+Player->speed, 72);
								
								FSOUND_PlaySound(FSOUND_FREE, sndSquirt);
							}
							else{
								FSOUND_PlaySound(FSOUND_FREE, shotgun);
								Shells -= 1;								
							}
							Player->uani = 11.0f;
							Player->shoot = true;							
						}
						else if(Weapon[SelectedWeapon] == MACHINEGUN && Bullets > 0){
							Player->uani = 18;
							FSOUND_PlaySound(FSOUND_FREE, sndRifle);
							Bullets -= 1;
							Player->shoot = true;
						}
						else if(Weapon[SelectedWeapon] == MAC && Choppers > 0 && ShootTimer <= 0){
							Choppers--;
							Player->shoot = true;
							ShootTimer = 500;
						}
						else if(Weapon[SelectedWeapon] == SLEDGEHAMMER){
							if(AddSledgeSpeed)
								SledgeSpeed += 0.2f;
							else
								SledgeSpeed = 0.2f;
							if(SledgeSpeed > 0.4f) SledgeSpeed = 0.4f;
							AddSledgeSpeed = false;
							Player->uani = 51;
							Player->shoot = true;
						}
						
						if(Player->shoot){
							if(Player->xFlip)
								FireProjectile(Player->x, Player->y+10, 180, 1, Weapon[SelectedWeapon], Player, 0);
							else
								FireProjectile(Player->x, Player->y+10, 0, 1, Weapon[SelectedWeapon], Player, 0);							
						}

						if(Weapon[SelectedWeapon] == SINGULARITY && Singularities > 0 && ShootTimer <= 0){
							Player->uani = 42;
							FSOUND_PlaySound(FSOUND_FREE, sndThunder);
							Flash(255,255,255);
							Singularities -= 1;
							Player->shoot = true;
							if(Player->xFlip)
								FireProjectile(Player->x, Player->y, 180, 1, Weapon[SelectedWeapon], Player, 0);
							else
								FireProjectile(Player->x+19, Player->y, 0, 1, Weapon[SelectedWeapon], Player, 0);
							ShootTimer = 500;
						}
						ShootTimer -= diff;
				}
				if(Keyboard[keyShoot] == 0 && Keyboard[keyShoot2] == 0 && JoyState.rgbButtons[joyShoot] == 0 && Player->shoot == false){
					AddSledgeSpeed = true;
					SledgeSpeed -= 0.02f*diff;
					if(SledgeSpeed < 0.0f){
						SledgeSpeed = 0.0f;
						AddSledgeSpeed = false;
					}
				}
			//------------------------------------------------
			//- End checking shooting input
			//------------------------------------------------

			/*if(Keyboard[SDL_SCANCODE_S]){
					LoadScript("scripts/sc01.l3s");
					while(Keyboard[SDL_SCANCODE_S]) WinApp.lpdikey->GetDeviceState(256, Keyboard);
			}*/
			}
			}

			if(Keyboard[SDL_SCANCODE_1]) {FadeToBlack = true; Fading = true;}
			if(Keyboard[SDL_SCANCODE_2]) {FadeOutOfBlack = true; Fading = true;}
			if(Keyboard[SDL_SCANCODE_3]) {FadeToWhite = true; Fading = true;}
			if(Keyboard[SDL_SCANCODE_4]) {FadeOutOfWhite = true; Fading = true;}
			if(Keyboard[SDL_SCANCODE_5]){
				Flash(128, 255, 128);
				Player->lani = SKELETON;
				Player->lstart = SKELETON;
				Player->lend = SKELETON;
				Player->uani = SKELETON;
			}

			if(Keyboard[SDL_SCANCODE_F]){
				if(FPSKey == false){
					if(ShowFPS)
						ShowFPS = false;
					else
						ShowFPS = true;
				}
				FPSKey = true;
			}
			else FPSKey = false;

			FocusSwitchTimer -= diff;
			if(FocusSwitchTimer < 0) FocusSwitchTimer = 0;
			if(Keyboard[SDL_SCANCODE_F8] && FocusSwitchTimer == 0){
				Focus++;
				if(Focus > NumMonsters) Focus = 0;
				FocusEntity = &Monster[Focus];
				FocusSwitchTimer = 30;
			}
			if(Keyboard[SDL_SCANCODE_F7] && FocusSwitchTimer == 0){
				Focus--;
				if(Focus < 0) Focus = NumMonsters;
				FocusEntity = &Monster[Focus];
				FocusSwitchTimer = 30;
			}			
			if(Keyboard[SDL_SCANCODE_EQUALS]){
				if(ChangeStatusState){
					StatusState++;
					if(StatusState > 5) StatusState = 0;
				}				
			}
			if(Keyboard[SDL_SCANCODE_MINUS]){
				if(ChangeStatusState){
					StatusState--;
					if(StatusState < 0) StatusState = 5;
				}				
			}
			if(Keyboard[SDL_SCANCODE_EQUALS] || Keyboard[SDL_SCANCODE_MINUS])
				ChangeStatusState = false;
			else
				ChangeStatusState = true;
						
		//------------------------------------------------
		//------------------------------------------------
		//-- End Checking Input
		//------------------------------------------------
		//------------------------------------------------
			
		//------------------------------------------------
		//- Check if it's needed to scroll the screen
		//------------------------------------------------

			//- If Larry has fallen off the map, kill him
				if(Player->y >= 2000){
					Player->life = 0;
					Player->y = 2000;
				}
		
		//------------------------------------------------
		//- End scrolling screen
		//------------------------------------------------
	
				
	
	
	
	//--------------------------------------------------
	//- Play the sound of floor that Larry's stepping on
	//--------------------------------------------------
		/*if(Player->lani == 1 || Player->velocity > 1){
			mx = (int)(Player->x+9) / 20;
			my = (int)(Player->y+19) / 20;
			mp = LD3.Map[mx+(my<<MAPXLSHIFT)]; 
			//FSOUND_StopSound(1);
			//if(mp > 20 && mp < 28)
			//	FSOUND_PlaySound(1, mfstep);
			//else if(mp != 0 && mp < 21)
			//	FSOUND_PlaySound(1, fstep);
			//else if(mp > 27 && mp != 0)
			//	FSOUND_PlaySound(1, cfstep);
		}*/
	//--------------------------------------------------
	//- End playing floor sound
	//--------------------------------------------------

	//- Return false since the escape key wasn't pressed
		return(result);
}

//=================================================================================================
//= void RenderScene()
//= Scroll and render the environment, draw the entities, and other essentials such as grenade
//= cursors
//=================================================================================================
void RenderScene()
{
	//----------------------------------------------
	//- Make sure the focus stays on the player
	//----------------------------------------------
		yShift = oyShift;			
		
		if(LockShift){// && ProScript == false){
			xShift = oxShift;
			yShift = oyShift;
			if(xShift < LockXShift) xShift ++;
			if(xShift > LockXShift) xShift --;
			if(yShift < LockYShift) yShift ++;
			if(yShift > LockYShift) yShift --;
			if(xShift == oxShift && yShift == oyShift) Shifting = false;
		}
		else{
			if(FocusEntity->x-xShift > xShiftMax) xShift = FocusEntity->x-xShiftMax;
			if(FocusEntity->x-xShift < xShiftMin) xShift = FocusEntity->x-xShiftMin;
			if(FocusEntity->y-yShift > yShiftMax) yShift = FocusEntity->y-yShiftMax;
			if(FocusEntity->y-yShift < yShiftMin) yShift = FocusEntity->y-yShiftMin;
		}

	//----------------------------------------------
	//- end setting focus on player
	//----------------------------------------------

	//--------------------------------------------
	//- Check to make sure we don't scroll off
	//- the map
	//--------------------------------------------
		if(xShift < 0) xShift = 0;
		if(yShift < 0) yShift = 0;
		if(xShift > 9680) xShift = 9680;
		if(yShift > 1720) yShift = 1720;
	//--------------------------------------------
	//- End Scroll Check
	//--------------------------------------------
		
		oxShift = xShift;
		oyShift = yShift;

		yShift += yLook;
		if(yShift < 0) yShift = 0;
		if(yShift > 1720) yShift = 1720;
		
	//--------------------------------------------
	//- Render the environment
	//--------------------------------------------
		LD3.DrawSky(&WinApp, xShift+xSkyShift, yShift);
		LD3.DrawMap(&WinApp, xShift, yShift, 0, diff);
		LD3.DrawMap(&WinApp, xShift, yShift, 1, diff);		
	//--------------------------------------------
	//- End Render Environment
	//--------------------------------------------
		
	//--------------------------------------------
	//- Process and Draw Entities
	//--------------------------------------------
		DrawMonsters();

	//----------------------------------------------
	//- If player is aiming a grenade, draw the
	//- grenade aiming cursor
	//----------------------------------------------
		if(HoldingGrenade || ShowGrenadeCursor){
			int cx, cy, am;
			
			if(HoldingGrenade)
				am = (HoldingTimer>>1);
			else
				am = 20;

			if(Player->xFlip){
				cx = int(Player->x-1+Cosine[180-GrenadeAngle]*am-xShift);
				cy = int(Player->y+9-Sine[180-GrenadeAngle]*am-yShift);
			}
			else{
				cx = int(Player->x+20+Cosine[GrenadeAngle]*am-xShift);
				cy = int(Player->y+9-Sine[GrenadeAngle]*am-yShift);
			}
			
			//LD3.PutPixel(&WinApp, cx, cy, 41);
			//LD3.PutPixel(&WinApp, cx-1, cy, 40);
			//LD3.PutPixel(&WinApp, cx+1, cy, 40);
			//LD3.PutPixel(&WinApp, cx, cy-1, 40);
			//LD3.PutPixel(&WinApp, cx, cy+1, 40);
			LD3.PutSprite(&WinApp, cx-10, cy-10, -1, miGrenadeCursor, false);
		}
	//----------------------------------------------
	//- End drawing grenade cursor
	//----------------------------------------------

	DisplayHUD();
}


//=================================================================================================
//= void ProcessItems()
//= Calls the functions that processes the entities, projectiles, etc
//= Also processes timers and other essentials
//=================================================================================================
void ProcessItems()
{
	int red, grn, blu, count;
	SDL_Color ShiftPal[256];

	//--------------------------------------------
	//- Process Entities
	//--------------------------------------------
		if(Active){
			ProcessMonsters();
			ProcessProjectiles();
			ProcessParticles();
			ProcessCannons();
			ProcessExplosions();
		}
	//--------------------------------------------
	//- End Processing Entities
	//--------------------------------------------

	//--------------------------------------------
	//- Process Timers
	//--------------------------------------------
		if(SpeedTimer){
			SpeedTimer--;
			if(SpeedTimer <= 0){
				Player->maxspeed = 2;
				FMUSIC_SetMasterSpeed(song1, 1.0f);
			}
		}
		if(PowerTimer){
			PowerTimer--;
			if(PowerTimer <= 0) WeaponPower = OldWeaponPower;
		}
		if(JumpBTimer){
			JumpBTimer--;
			if(JumpBTimer <= 0) JumpVel = oldJumpVel;
		}		
	//--------------------------------------------
	//- End Processing Timers
	//--------------------------------------------
		
	//- Shift the palette at the far end
		/*if(Fading == false){
					
			PalCount++;
			if(PalCount > PalSpeed){
				PalCount = 0;

				for(int n = 255; n >= 240; n--){
					FadePalette[n] = FadePalette[n-1];
				}
				FadePalette[240] = FadePalette[255];				
			}
		}

	WinApp.lpddpal->Release();
	WinApp.lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp.lpddpal, NULL);
	WinApp.lpddsprimary->SetPalette(WinApp.lpddpal);*/

		if(ShiftPalette){
			PalCount++;
			if(PalCount > PalSpeed/diff){
				PalCount = 0;
				memcpy(ShiftPal, &WinApp.lpddpal->colors[240], 15 * sizeof(SDL_Color));
				SDL_SetPaletteColors(WinApp.lpddpal, ShiftPal, 241, 14); 
				SDL_SetPaletteColors(WinApp.lpddpal, &ShiftPal[14], 240, 1); 
			}
		}
}

//=================================================================================================
//= void ProcessPlayer()
//= Process the player
//=================================================================================================
void ProcessPlayer()
{
	int HitFloor;
	
	//------------------------------------------------
	//- Process Player
	//------------------------------------------------
	if(Player->life > 0){
		
		Player->ox = Player->x;
		Player->oy = Player->y;
		
		Player->Move(diff);		
		HitFloor = Player->Fall(&LD3.Map[0], MAPWIDTH, diff);
		Player->CheckWallHit(&LD3.Map[0], MAPWIDTH);
		
		if(Player->oy <= Player->y){
			PlayerY1 = Player->oy;
			PlayerY2 = Player->y;
		}
		else{
			PlayerY1 = Player->y;
			PlayerY2 = Player->oy;
		}
		
		
		PlatformVelocity = 10000;
		if(HitFloor) PlatformVelocity = -10000;

		if(TreadMillDeath && (HitFloor == miTreadmillLeft || HitFloor == miTreadmillRight)){
			Splatter(Player->x+7, Player->y+7, 50, 0, 37);
			Splatter(Player->x+7, Player->y+15, 167, 0, 37);
			Player->life = 0;			
		}		
				
		//- Check if Larry is in contact with an item
		if(Player->nocheck == false) CheckForItems();
	}

	if(Player->life <= 0 && PlayedDeathSound == false){		
		int d = rand()%(NumDeathSounds-1);
		FSOUND_PlaySound(FSOUND_FREE, sndDeath[d]);
		PlayedDeathSound = true;
	}

	//------------------------------------------------
	//- End Processing Player
	//------------------------------------------------
}

//=================================================================================================
//= void LoadScript(const char* Filename)
//= Load an external script file and store it for processing
//=================================================================================================
void LoadScript(const char* Filename)
{
	//--------------------------------------------------
	//- Declarations
	//--------------------------------------------------
		char Word1[20], Word2[20];
		char Quote[80];
		//char ch, ach;
		//int  count;
		//ENTITY	*sEntity;
		int   Id = 0;
		int   Id2 = 0;
		bool  NewId;
		//int   amount;
		int   null;
		float tempdist, dist, mx, my;
		bool  FileExist = false;
		int   nm;
	//--------------------------------------------------
	//- End Declarations
	//--------------------------------------------------

	//--------------------------------------------------
	//- Initializing
	//--------------------------------------------------
		memset(Identifier, 0, 500);
		//memset(ScriptEntity, 0, sizeof(ENTITY)*50);
		//memset(ScriptIns, 0, 8000);
		//memset(ScriptMsg, 0, 8000);		
		NumScripts = 0;  NumMsgs   = 0;
		ScriptCount = 0; MsgCount  = 0;
		NumIds = 0;
		ProScript = false;
	//--------------------------------------------------
	//- End Initializing
	//--------------------------------------------------

		if(Player->velocity != 0){
			Player->speed = 0;
			Player->setspeed = 0;
		}
		
		ifstream ScriptFile(Filename, ios::binary);
		if(ScriptFile) FileExist = true;
		
		while(ScriptFile)
		{
			Keyboard = SDL_GetKeyboardState(0);
			if(Keyboard[SDL_SCANCODE_ESCAPE]) break;
			
			memset(&Word1, 0, 20);
			memset(&Word2, 0, 20);
						
			ReadWord(&ScriptFile, &Word1[0]); SDL_strupr(Word1);
			ReadWord(&ScriptFile, &Word2[0]); SDL_strupr(Word2);

			
			if(strcmp(Word2, "=") == 0){				
				memset(&Word2, 0, 20);
				ReadWord(&ScriptFile, &Word2[0]); SDL_strupr(Word2);

				NewId = true;
				for(int i = 1; i <= NumIds; i++){
					if(strcmp(Word1, &Identifier[i*50]) == 0) NewId = false;					
				}

				if(NewId){
					NumIds++;
					strcpy(&Identifier[NumIds*50], Word1);
					
					if(strcmp(Word2, "ROCKMONSTER") == 0){
						//- Find closest rockmonster for manipulation
						dist = 900000;
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == ROCK_MONSTER){
								mx = (float)fabs(Monster[i].x-Player->x);
								my = (float)fabs(Monster[i].y-Player->y);
								tempdist = (float)sqrt(mx*mx+my*my);
								if(tempdist < dist){
									ScriptEntity[NumIds] = &Monster[i];
									dist = tempdist;
								}
							}
						}
						if(dist == 900000){
							memset(&Identifier[NumIds*50], 0, 50);
							NumIds--;							
						}
					}
					if(strcmp(Word2, "LARRY") == 0){
						//- Find Larry
						//for(int i = 0; i <= NumMonsters; i++){
						//	if(Monster[i].id == LARRY){
						//		ScriptEntity[NumIds] = &Monster[i];
						//		break;
						//	}
						//}
						ScriptEntity[NumIds] = Player;						
					}
					if(strcmp(Word2, "FLETCHER") == 0){
						//- Find Fletcher
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == FLETCHER){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "MARK") == 0){
						//- Find Mark
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == MARK){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "BADMARK") == 0){
						//- Find BadMark
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == BOSS4){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "XAHN") == 0){
						//- Find Xahn
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == XAHN){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "RUSTY") == 0){
						//- Find Rusty
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == RUSTY){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "GENERAL") == 0){
						//- Find General
						for(int i = 1; i <= NumMonsters; i++){
							if(Monster[i].id == GENERAL){
								ScriptEntity[NumIds] = &Monster[i];
								break;
							}
						}
					}
					if(strcmp(Word2, "NEWROCKMONSTER") == 0){
						//- Create a new rockmonster
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= ROCK_MONSTER;
							Monster[nm].spriteset= ROCK_MONSTER_SPRITE_SET;
							Monster[nm].lani	= 30;
							Monster[nm].lstart	= 30;
							Monster[nm].lend	= 39;
							Monster[nm].life	= 20;
							Monster[nm].dontfall= true;
							Monster[nm].ScoreValue = 100;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];
					}
					if(strcmp(Word2, "NEWFLETCHER") == 0){
						//- Create a new Fletcher
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= FLETCHER;
							Monster[nm].spriteset= FLETCHER_SPRITE_SET;
							Monster[nm].lani	= 40;
							Monster[nm].uani	= 49;
							Monster[nm].lstart	= 40;
							Monster[nm].lend	= 45;
							Monster[nm].life	= 100;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 0;
							Monster[nm].setspeed = 0;
							Monster[nm].maxspeed = 2;
							Monster[nm].speed = 0;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];							
					}
					if(strcmp(Word2, "NEWMARK") == 0){
						//- Create a new Mark
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= MARK;
							Monster[nm].spriteset= MARK_SPRITE_SET;
							Monster[nm].lani	= 60;
							Monster[nm].uani	= 69;
							Monster[nm].lstart	= 60;
							Monster[nm].lend	= 64;
							Monster[nm].life	= 100;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 0;
							Monster[nm].setspeed = 0;
							Monster[nm].maxspeed = 1.7f;
							Monster[nm].speed = 0;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];
					}
					if(strcmp(Word2, "NEWBADMARK") == 0){
						//- Create a new BadMark
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id	= BOSS4;
							Monster[nm].armor = 2.0f;
							Monster[nm].BloodCol = 42;
							Monster[nm].spriteset= BOSS_SPRITE_SET;							
							Monster[nm].lani = 0;
							Monster[nm].lstart = 0;
							Monster[nm].lend = 5;
							Monster[nm].uani = 6;
							Monster[nm].life	= 100;
							Monster[nm].maxspeed = 2;
							Monster[nm].setspeed = 0;
							Monster[nm].speed = 0;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 1000;
							Monster[nm].count   = 0;
							Monster[nm].count   = 2;
							Monster[nm].HitBox.top    =   1;
							Monster[nm].HitBox.bottom =  19;
							Monster[nm].HitBox.left   =   1;
							Monster[nm].HitBox.right  =  17;
														
							Monster[nm].visible = true;
							Boss = &Monster[nm];
							/*
							BossXShift  = (int)Monster[nm].x-(xRes/2);
							BossYShift  = (int)Monster[nm].y-(yRes/2);
							if(BossYShift < 0) BossYShift = 0;
							if(BossXShift < 0) BossXShift = 0;*/
							LD3.LoadSprite(&WinApp, "gfx/badmark.put", BOSS_SPRITE_SET, 0);
							ScriptEntity[NumIds] = &Monster[nm];
					}
					if(strcmp(Word2, "NEWRUSTY") == 0){
						//- Create a new Rusty
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= RUSTY;
							Monster[nm].spriteset= RUSTY_SPRITE_SET;
							Monster[nm].lani	= 80;
							Monster[nm].uani	= 89;
							Monster[nm].lstart	= 80;
							Monster[nm].lend	= 85;
							Monster[nm].life	= 100;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 0;
							Monster[nm].setspeed = 0;
							Monster[nm].maxspeed = 1.8f;
							Monster[nm].speed = 0;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];
					}
					if(strcmp(Word2, "NEWGENERAL") == 0){
						//- Create a new General
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= GENERAL;
							Monster[nm].spriteset= GENERAL_SPRITE_SET;
							Monster[nm].lani	= 120;
							Monster[nm].uani	= 129;
							Monster[nm].lstart	= 120;
							Monster[nm].lend	= 125;
							Monster[nm].life	= 100;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 0;
							Monster[nm].setspeed = 0;
							Monster[nm].maxspeed = 1.5;
							Monster[nm].speed = 0;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];
					}
					if(strcmp(Word2, "NEWXAHN") == 0){
						//- Create a new Xahn
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= 0;
							Monster[nm].y		= 0;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= XAHN;
							Monster[nm].spriteset= XAHN_SPRITE_SET;
							Monster[nm].lani	= 100;
							Monster[nm].uani	= 109;
							Monster[nm].lstart	= 100;
							Monster[nm].lend	= 105;
							Monster[nm].life	= 100;
							Monster[nm].dontfall= false;
							Monster[nm].ScoreValue = 0;
							Monster[nm].setspeed = 0;
							Monster[nm].maxspeed = 2;
							Monster[nm].speed = 0;
							Monster[nm].BloodCol = 37;
							ScriptEntity[NumIds] = &Monster[nm];
					}
				}			
			}

			Id = 0;
			for(int i = 1; i <= NumIds; i++)
				if(strcmp(Word1, &Identifier[i*50]) == 0) Id = i;

			if(strcmp(Word1, "SCRIPT") == 0 && strcmp(Word2, "END") == 0) break;
						
			if(Id != 0 || strcmp(Word1, "UNKNOWN") == 0){
				if(strcmp(Word2, "SAY") == 0 || strcmp(Word2, "TALK") == 0)
				{
					memset(&Quote, 0, 80);
					ReadQuote(&ScriptFile, &Quote[0]); SDL_strupr(Quote);
					//EntityTalk(Quote, 2);
					strcpy(&ScriptMsg[NumMsgs*80], Quote);
					strcpy(&ScriptIns[NumScripts*20], "TALK");
					ScriptVal[NumScripts] = 0;
					ScriptId[NumScripts] = Id;
					NumScripts++;
					NumMsgs++;				
				}
				else if(strcmp(Word2, "XMOVE") == 0 || strcmp(Word2, "YMOVE") == 0 || strcmp(Word2, "LOCKINPUT") == 0 || strcmp(Word2, "XFLIP") == 0 || strcmp(Word2, "SETSPEED") == 0 || strcmp(Word2, "VISIBLE") == 0 || strcmp(Word2, "LANI") == 0 || strcmp(Word2, "UANI") == 0 || strcmp(Word2, "SPLATTER") == 0 || strcmp(Word2, "LOOP") == 0 || strcmp(Word2, "WAIT") == 0)
				{
					strcpy(&ScriptIns[NumScripts*20], Word2);
					ScriptVal[NumScripts] = ReadValue(&ScriptFile);
					ScriptId[NumScripts] = Id;
					NumScripts++;				
				}
				else if(strcmp(Word2, "XMOVETO") == 0)
				{
					strcpy(&ScriptIns[NumScripts*20], Word2);
					ScriptVal[NumScripts] = (int)ReadArithemetic(&ScriptFile);
					ScriptId[NumScripts] = Id;
					NumScripts++;				
				}
				else if(strcmp(Word2, "X") == 0 || strcmp(Word2, "Y") == 0 || strcmp(Word2, "XSHIFT") == 0 || strcmp(Word2, "YSHIFT") == 0  || strcmp(Word2, "WAITANDYSHIFT") == 0){
			
					memset(Word1, 0, 20);
					if(strcmp(Word2, "X") == 0) strcpy(Word1, "X=");
					if(strcmp(Word2, "Y") == 0) strcpy(Word1, "Y=");
					if(strcmp(Word2, "XSHIFT") == 0) strcpy(Word1, "XSHIFT");
					if(strcmp(Word2, "YSHIFT") == 0) strcpy(Word1, "YSHIFT");
					if(strcmp(Word2, "XMOVETO") == 0) strcpy(Word1, "XMOVETO");
					if(strcmp(Word2, "WAITANDYSHIFT") == 0) strcpy(Word1, "WAITANDYSHIFT");
					
					memset(&Word2, 0, 20);
					ReadWord(&ScriptFile, &Word2[0]); SDL_strupr(Word2);
					
					if(strcmp(Word2, "=") == 0){
						strcpy(&ScriptIns[NumScripts*20], Word1);
						ScriptVal[NumScripts] = (int)ReadArithemetic(&ScriptFile);
						ScriptId[NumScripts] = Id;
						NumScripts++;					
					}			
				}
				else if(strcmp(Word2, "AISTATE") == 0){
					memset(&Word2, 0, 20);
					ReadWord(&ScriptFile, &Word2[0]); SDL_strupr(Word2);

					if(strcmp(Word2, "=") == 0){
						memset(&Word2, 0, 20);
						ReadWord(&ScriptFile, &Word2[0]); SDL_strupr(Word2);

						strcpy(&ScriptIns[NumScripts*20], Word2);
						ScriptVal[NumScripts] = 0;
						ScriptId[NumScripts] = Id;
						NumScripts++;
					}
				}
				else if(strcmp(Word2, "NEXTLEVEL") == 0 || strcmp(Word2, "DIE") == 0 || strcmp(Word2, "EXPLODE") == 0 || strcmp(Word2, "SHOOT") == 0 || strcmp(Word2, "MOVETOPOS1") == 0 || strcmp(Word2, "MOVETOPOS2") == 0 || strcmp(Word2, "ISHURT") == 0){
					strcpy(&ScriptIns[NumScripts*20], Word2);
					ScriptVal[NumScripts] = 0;
					ScriptId[NumScripts] = Id;
					NumScripts++;	
				}
				else if(strcmp(Word2, "JUMPTOLEVEL") == 0){
					strcpy(&ScriptIns[NumScripts*20], Word2);
					ScriptVal[NumScripts] = ReadValue(&ScriptFile);
					ScriptId[NumScripts] = Id;
					NumScripts++;
				}
				else if(strcmp(Word2, "FADETOBLACK") == 0 || strcmp(Word2, "FADETOWHITE") == 0 || strcmp(Word2, "FADEOUTOFBLACK") == 0 || strcmp(Word2, "FADEOUTOFWHITE") == 0 || strcmp(Word2, "SHOWCREDITS") == 0 || strcmp(Word2, "CLEARBUFFERS") == 0){
					strcpy(&ScriptIns[NumScripts*20], Word2);
					ScriptVal[NumScripts] = 0;
					ScriptId[NumScripts] = Id;
					NumScripts++;
				}
			}
			else
			{
				if(strcmp(Word2, "SAY") == 0 || strcmp(Word2, "TALK") == 0)
				{
					memset(&Quote, 0, 80);
					ReadQuote(&ScriptFile, &Quote[0]); SDL_strupr(Quote);								
				}
				else if(strcmp(Word2, "XMOVE") == 0 || strcmp(Word2, "YMOVE") == 0 || strcmp(Word2, "LOCKINPUT") == 0)
				{
					null = ReadValue(&ScriptFile);					
				}
				else if(strcmp(Word1, "MUSIC") == 0){
					if(strcmp(Word2, "FADETO") == 0 || strcmp(Word2, "WAITANDFADETO") == 0){
						strcpy(&ScriptIns[NumScripts*20], Word2);
						ScriptVal[NumScripts] = ReadValue(&ScriptFile);
						ScriptId[NumScripts] = Id;
						NumScripts++;
					}
					if(strcmp(Word2, "STOP") == 0 || strcmp(Word2, "PLAY") == 0 || strcmp(Word2, "PLAYLEVELSONG") == 0 || strcmp(Word2, "PLAYXAHNTHEME") == 0 || strcmp(Word2, "STOPXAHNTHEME") == 0){
						strcpy(&ScriptIns[NumScripts*20], Word2);
						ScriptVal[NumScripts] = 0;
						ScriptId[NumScripts] = Id;
						NumScripts++;
					}
				}
			}
		}
					
	ScriptFile.close();

	if(FileExist){
		strcpy(&ScriptIns[NumScripts*20], "ENDSCRIPT");
		ProScript = true;
		ScriptCount = 0;
		MsgCount    = 0;
	}
}

//=================================================================================================
//= void ReadWord(ifstream *File, char *Word)
//= Read a word from the given file and store it in *Word
//=================================================================================================
void ReadWord(ifstream *File, char *Word)
{
	//- Read a word in the given file and return it

	char ch = 0;
	int  count = 0;
		
	//memset(Word, 0, 20);
	File->get(ch);
	while(ch <= 42 || ch >= 123 || ch == 46) File->get(ch);
	while(ch == 59){
		while(ch != '\n') File->get(ch);
		while(ch <= 42 || ch >= 123 || ch == 46) File->get(ch);
	}

	while(ch >= 43 && ch <= 122 && ch != 46 && ch != 13){
		Word[count] = ch;
		count++;
		File->get(ch);		
		if (File->eof()) break;
	}	
	Word[count] = '\0';
}

//=================================================================================================
//= void ReadQuote(ifstream *File, char *Word)
//= This function searches in the given file until it finds a double quote, then reads until it
//= finds the next double quote and stores what was read in *Word
//=================================================================================================
void ReadQuote(ifstream *File, char *Word)
{
	//- Read a quote in the given file and return it

	char ch;
	int  count = 0;
		
	//memset(Word, 0, 20);
	File->get(ch);
	while(ch != 34) File->get(ch);
	
	File->get(ch);
	while(ch != 34){
		Word[count] = ch;
		count++;
		File->get(ch);		
	}	
	Word[count] = '\0';
}

//=================================================================================================
//= int ReadValue(ifstream *File)
//= This function searches in the given file until it finds a valid number and returns it's value
//=================================================================================================
int ReadValue(ifstream *File)
{
	//- Find a number in the given file and return it's value

	char ch;
	char Num[20];
	int  count = 0;

	memset(Num, 0, 20);
	File->get(ch);
	while(ch <= 42 || ch >= 123) File->get(ch);	
	while(ch == 59){
		while(ch != '\n') File->get(ch);
		while(ch <= 42 || ch >= 123) File->get(ch);
	}

	while(ch >= 43 && ch <= 122){
		Num[count] = ch;
		count++;
		File->get(ch);		
	}	
	return(atoi(Num));
}

//=================================================================================================
//= float ReadArithmetic(ifstream *file)
//= Reads basic math calculations within < > and returns the value
//=================================================================================================
float ReadArithemetic(ifstream *File)
{
	float value = 0;
	int   ConstantValue = 0;
	int   Id = 0;
	char Word2[20];
	int arithemetic = ADD;
		
	ReadWord(File, &Word2[0]);
					
	value = 0;
	while(NO_ONE_CARES)
	{
		memset(&Word2, 0, 20);
		ReadWord(File, &Word2[0]); SDL_strupr(Word2);

		if(strcmp(Word2, ">") == 0) break;
						
		Id = 0;
		for(int i = 1; i <= NumIds; i++)
			if(strcmp(Word2, &Identifier[i*50]) == 0) Id = i;
					
		if(Id == 0){
			//memset(&Word2, 0, 20);
			//ReadWord(File, &Word2[0]); SDL_strupr(Word2);

			if(strcmp(Word2, "+") == 0) arithemetic = ADD;
			if(strcmp(Word2, "-") == 0) arithemetic = SUBTRACT;
			if(strcmp(Word2, "*") == 0) arithemetic = MULTIPLY;
			if(strcmp(Word2, "/") == 0) arithemetic = DIVIDE;

			//- check if it's a number
			if((Word2[0] >= 48 && Word2[0] <= 57) || (Word2[1] >= 48 && Word2[1] <= 57)){
				ConstantValue = atoi(Word2);
				switch(arithemetic)
				{
				case ADD:
					value += ConstantValue;
					break;
				case SUBTRACT:
					value -= ConstantValue;
					break;
				case MULTIPLY:
					value *= ConstantValue;
					break;
				case DIVIDE:
					value /= ConstantValue;
					break;
				}
				
			}
		}
		else{
			memset(&Word2, 0, 20);
			ReadWord(File, &Word2[0]); SDL_strupr(Word2);

			switch(arithemetic)
			{
			case ADD:
				if(strcmp(Word2, "X") == 0) value += ScriptEntity[Id]->x;
				if(strcmp(Word2, "Y") == 0) value += ScriptEntity[Id]->y;
				break;
			case SUBTRACT:
				if(strcmp(Word2, "X") == 0) value -= ScriptEntity[Id]->x;
				if(strcmp(Word2, "Y") == 0) value -= ScriptEntity[Id]->y;
				break;
			case MULTIPLY:
				if(strcmp(Word2, "X") == 0) value *= ScriptEntity[Id]->x;
				if(strcmp(Word2, "Y") == 0) value *= ScriptEntity[Id]->y;
				break;
			case DIVIDE:
				if(strcmp(Word2, "X") == 0) value /= ScriptEntity[Id]->x;
				if(strcmp(Word2, "Y") == 0) value /= ScriptEntity[Id]->y;
				break;
			}
		}
	}

	return(value);
}

//=================================================================================================
//= void EntityTalk(char *Msg, int FaceId)
//= This functions takes the given message and splits it into two strings depending on the size
//= Then it stores these strings to be processed in the talk box while the game is running
//=================================================================================================
void EntityTalk(char *Msg, int FaceId)
{
	int length, n, t;
	
	memset(TalkMessage, 0, 80);
	memset(TalkMessage1, 0, 50);
	memset(TalkMessage2, 0, 50);
	strcpy(TalkMessage, &Msg[0]);
	SDL_strupr(TalkMessage);
	//TalkId = FaceId;
	TalkCount = 0;

	TalkWords = 0;
	length = strlen(TalkMessage);
	for(int i = 0; i < length; i++)
		if(TalkMessage[i] == ' ') TalkWords++;
	TalkWords++;
	TalkWords <<= 1;
	TalkWords--;
	OldTalkWords = TalkWords;

	if(length > 40){
		for(int i = 0; i <= 39; i++) TalkMessage1[i] = TalkMessage[i];
		if(TalkMessage[39] != ' ' && TalkMessage[40] != ' '){
			for(int i = 39; i >= 0; i--){
				if(TalkMessage[i] == ' '){
					for(n = i+1; n <= 39; n++)
						TalkMessage2[n-i-1] = TalkMessage[n];
					n = 39-i;
					break;
				}
				else
					TalkMessage1[i] = ' ';
			}
		}
		else
			n = 0;
		
		if(TalkMessage[40] == ' ')
			t = 1;
		else
			t = 0;		

		for(int i = 0; i <= length-40; i++) TalkMessage2[i+n] = TalkMessage[i+40+t];
	}
	else
		strcpy(TalkMessage1, TalkMessage);
	
	Length1 = strlen(TalkMessage1);
	Length2 = strlen(TalkMessage2);
	TalkMsgCount1 = 0;
	TalkMsgCount2 = 0;
	memset(TempMessage1, 0, 50);
	memset(TempMessage2, 0, 50);
}

//=================================================================================================
//= void RunScript()
//= Runs what's processed from the script
//=================================================================================================
void RunScript()
{
	//--------------------------------------------
	//- Process the script
	//--------------------------------------------
		//- If someone is talking, draw the talk box and
		//- write what that someone is talking about
		bool ss = SkipScene;		
		if(CutScene) LD3.DrawLetterBox(&WinApp);	
		if(Talking){
				
				TalkCount += 0.1f;
				if(int(TalkCount*10) % 2 == 0){
					
					if(TalkMsgCount1 >= Length1)
						TalkMsgCount2 ++;
					else
						TalkMsgCount1 ++;					
					
					if(TalkMsgCount2 > Length2){
						TalkMsgCount2 = Length2;
						TalkId = TalkSprite;
					}
						
					memset(TempMessage1, 0, 50);
					memset(TempMessage2, 0, 50);
					memcpy(TempMessage1, TalkMessage1, TalkMsgCount1);
					memcpy(TempMessage2, TalkMessage2, TalkMsgCount2);
						
				}

				if(CutScene == false){
					//- Draw talk box at top
					LD3.PutSprite(&WinApp, 0, 0, DIALOG_SPRITE_SET, TalkId, 0);			
					LD3.DrawTalkBox(&WinApp, 0);
					WriteText(24, 4,  TempMessage1, 15, 0);
					WriteText(24, 11, TempMessage2, 15, 0);
				}
				else{
					//- Draw talk box at bottom
					LD3.PutSprite(&WinApp, 0, yRes-20, DIALOG_SPRITE_SET, TalkId, 0);
					LD3.DrawTalkBox(&WinApp, yRes-20);
					WriteText(24, yRes-16,  TempMessage1, 15, 0);
					WriteText(24, yRes-9, TempMessage2, 15, 0);
				}

					
				if((TalkMsgCount2 == Length2 && Length2 != 0) || (TalkMsgCount1 == Length1 && Length2 == 0) || SkipScene){
					TalkDelayCount++;
					if((TalkDelayCount >= TalkDelay) || SkipScene){
						Talking = false;
						if(ProScript){
							do{
								ProcessScript();
							} while(SkipScene);
							SkipScene = ss;
						}						
					}
				}
				else
				{
					
					if(TalkCount >= 1 && TalkWords > 0)
					{
						TalkCount = 0;
						TalkWords--;
						TalkId++;
						if(TalkId >= TalkSprite+2)
							TalkId = TalkSprite;
						else
							TalkId = TalkSprite+1;
					}
				}					
			}
		//- If someone isn't talking or has finished talking,
		//- processing what's next in the script queue
		else{		
			if(ProScript){
				do{
					ProcessScript();
				} while(SkipScene);
				SkipScene = ss;
			}
		}
	//--------------------------------------------
	//- End Processing Script
	//--------------------------------------------

	//--------------------------------------------
	//- Alter the palette if fading
	//--------------------------------------------
		if(Fading){
			PaletteChange = false;
			if(FadeToBlack){
				for(int n = 0; n <= 255; n++){
					red = (int)FadePalette[n].r;
					grn = (int)FadePalette[n].g;
					blu = (int)FadePalette[n].b;
					if(red > 0) {red -= FADE_SPEED; PaletteChange = true;}
					if(grn > 0) {grn -= FADE_SPEED; PaletteChange = true;}
					if(blu > 0) {blu -= FADE_SPEED; PaletteChange = true;}
					if(red < 0) red = 0;
					if(grn < 0) grn = 0;
					if(blu < 0) blu = 0;
					FadePalette[n].r   = (BYTE)red;
					FadePalette[n].g = (BYTE)grn;
					FadePalette[n].b  = (BYTE)blu;
				}
			}
			else if(FadeOutOfBlack){
				for(int n = 0; n <= 255; n++){
					red = (int)FadePalette[n].r;
					grn = (int)FadePalette[n].g;
					blu = (int)FadePalette[n].b;
					if(red < WinApp.palette[n].r)   {red += FADE_SPEED; PaletteChange = true;}
					if(grn < WinApp.palette[n].g) {grn += FADE_SPEED; PaletteChange = true;}
					if(blu < WinApp.palette[n].b)  {blu += FADE_SPEED; PaletteChange = true;}
					if(red > WinApp.palette[n].r)   red = WinApp.palette[n].r;
					if(grn > WinApp.palette[n].g) grn = WinApp.palette[n].g;
					if(blu > WinApp.palette[n].b)  blu = WinApp.palette[n].b;
					FadePalette[n].r   = (BYTE)red;
					FadePalette[n].g = (BYTE)grn;
					FadePalette[n].b  = (BYTE)blu;
				}
			}
			else if(FadeToWhite){
				for(int n = 0; n <= 255; n++){
					red = (int)FadePalette[n].r;
					grn = (int)FadePalette[n].g;
					blu = (int)FadePalette[n].b;
					if(red < 255) {red += FADE_SPEED; PaletteChange = true;}
					if(grn < 255) {grn += FADE_SPEED; PaletteChange = true;}
					if(blu < 255) {blu += FADE_SPEED; PaletteChange = true;}
					if(red > 255) red = 255;
					if(grn > 255) grn = 255;
					if(blu > 255) blu = 255;
					FadePalette[n].r   = (BYTE)red;
					FadePalette[n].g = (BYTE)grn;
					FadePalette[n].b  = (BYTE)blu;
				}
			}
			else if(FadeOutOfWhite){
				for(int n = 0; n <= 255; n++){
					red = (int)FadePalette[n].r;
					grn = (int)FadePalette[n].g;
					blu = (int)FadePalette[n].b;
					if(red > WinApp.palette[n].r)   {red -= FADE_SPEED; PaletteChange = true;}
					if(grn > WinApp.palette[n].g) {grn -= FADE_SPEED; PaletteChange = true;}
					if(blu > WinApp.palette[n].b)  {blu -= FADE_SPEED; PaletteChange = true;}
					if(red < WinApp.palette[n].r)   red = WinApp.palette[n].r;
					if(grn < WinApp.palette[n].g) grn = WinApp.palette[n].g;
					if(blu < WinApp.palette[n].b)  blu = WinApp.palette[n].b;
					FadePalette[n].r   = (BYTE)red;
					FadePalette[n].g = (BYTE)grn;
					FadePalette[n].b  = (BYTE)blu;
				}
			}				
			
			if(PaletteChange == false){
				Fading = false;
				FadeToBlack = false;
				FadeToWhite = false;
				FadeOutOfBlack = false;
				FadeOutOfWhite = false;
			}
			else{
				//WinApp.lpddpal->Release();
				//WinApp.lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp.lpddpal, NULL);
				//WinApp.lpddsprimary->SetPalette(WinApp.lpddpal);
				SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
			}
		}
	//--------------------------------------------
	//- End altering palette
	//--------------------------------------------

	//----------------------------------------------
	//- If the music volume has changed, change it
	//- smoothly
	//----------------------------------------------
		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade == MusicVolume) WaitMusicFade = false;
	//----------------------------------------------
	//- End changing music volume
	//----------------------------------------------		

		if(SkipScene == true && ProScript == false){//ProScript == false && SkipScene == true){
			SkipScene = false;
			LD3.LoadPalette(&WinApp, PaletteFilename);
			memcpy(FadePalette, WinApp.palette, sizeof(SDL_Color)*256);	
			SDL_FreePalette(WinApp.lpddpal);
			WinApp.lpddpal = SDL_AllocPalette(256);
			SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
			SDL_SetSurfacePalette(WinApp.lpddsback, WinApp.lpddpal);
			//TODO(davidgow): This orignally updated the primary surface palette? Do we need to re-blit?
			Flash(255,255,255);
		}
		if(ss == false) SkipScene = false;
}
					
//=================================================================================================
//= void ProcessScript()
//= Read the script commands and process them
//=================================================================================================
void ProcessScript()
{
	int amount, Id, x, y;
	bool loop = true;

	//FadeTo
	
	while(loop == true && Moving == false && Fading == false && WaitMusicFade == false && Shifting == false && ScriptWait == 0)
	{
		Id = ScriptId[ScriptCount];
				
		if(strcmp(&ScriptIns[ScriptCount*20], "ENDSCRIPT") == 0){
			ProScript = false;
			LockInput = false;
			LockShift = false;
			loop = false;
			SkipScene = false;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "FADETO") == 0){
			MusicVolume = ScriptVal[ScriptCount];
			if(MusicVolume > maxMusicVolume) MusicVolume = maxMusicVolume;
			loop = false;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "WAITANDFADETO") == 0){
			MusicVolume = ScriptVal[ScriptCount];
			if(MusicVolume > maxMusicVolume) MusicVolume = maxMusicVolume;
			WaitMusicFade = true;
			loop = false;
		}		
		if(strcmp(&ScriptIns[ScriptCount*20], "STOP") == 0){
			FMUSIC_StopSong(song1);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "PLAY") == 0){
			FMUSIC_SetMasterVolume(song1, maxMusicVolume);
			FMUSIC_PlaySong(song1);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "CLEARBUFFERS") == 0){		
			LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
			LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "PLAYXAHNTHEME") == 0){
			FMUSIC_StopSong(song1);
			FMUSIC_SetMasterVolume(mscXahnTheme, maxMusicVolume);
			MusicVolume = maxMusicVolume;
			MusicFade   = maxMusicVolume;
			FMUSIC_PlaySong(mscXahnTheme);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "STOPXAHNTHEME") == 0){
			FMUSIC_StopSong(mscXahnTheme);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "PLAYLEVELSONG") == 0){
			FMUSIC_StopSong(mscXahnTheme);			
			FMUSIC_PlaySong(song1);
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "SHOWCREDITS") == 0){
			mnuCredits2();
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "WAIT") == 0){
			amount = ScriptVal[ScriptCount];
			ScriptWait = amount*75;
			loop = false;			
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "X=") == 0){
			amount = ScriptVal[ScriptCount];
			ScriptEntity[Id]->x = (float)amount;
			loop = true;
			/*if(Boss->id == ScriptEntity[Id]->id){
				BossXShift  = (int)ScriptEntity[Id]->x-(xRes/2);
				if(BossXShift < 0) BossXShift = 0;
			}*/
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "Y=") == 0){
			amount = ScriptVal[ScriptCount];
			ScriptEntity[Id]->y = (float)amount;
			loop = true;
			/*if(Boss->id == ScriptEntity[Id]->id){
				BossYShift  = (int)ScriptEntity[Id]->y-(yRes/2);
				if(BossYShift < 0) BossYShift = 0;
			}*/
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "XSHIFT") == 0){
			LockShift = true;
			LockYShift = yShift;
			
			amount = ScriptVal[ScriptCount];
			LockXShift = amount;
			loop = true;			
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "YSHIFT") == 0){
			LockShift = true;
			LockXShift = xShift;
			
			amount = ScriptVal[ScriptCount];
			LockYShift = amount;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "WAITANDYSHIFT") == 0){
			LockShift = true;
			LockXShift = xShift;
			
			amount = ScriptVal[ScriptCount];
			LockYShift = amount;
			loop = false;
			Shifting = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "LANI") == 0){
			amount = ScriptVal[ScriptCount];
			//ScriptEntity[Id]->lstart = amount;
			ScriptEntity[Id]->lani = (float)amount;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "UANI") == 0){
			amount = ScriptVal[ScriptCount];
			//ScriptEntity[Id]->ustart = amount;
			ScriptEntity[Id]->uani = (float)amount;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "SPLATTER") == 0){
			amount = ScriptVal[ScriptCount];
			if(ScriptEntity[Id]->xFlip)
				Splatter((int)ScriptEntity[Id]->x+9, (int)ScriptEntity[Id]->y+9, amount, -1, ScriptEntity[Id]->BloodCol);
			else
				Splatter((int)ScriptEntity[Id]->x+9, (int)ScriptEntity[Id]->y+9, amount,  1, ScriptEntity[Id]->BloodCol);
			loop = false;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "LOOP") == 0){
			if(LoopCount == 0){
				LoopCount = ScriptVal[ScriptCount];				
				ScriptCount -= 2;
			}
			else{
				LoopCount--;
				if(LoopCount > 0) ScriptCount -= 2;
			}
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "EXPLODE") == 0){
			Splatter((int)ScriptEntity[Id]->x+9, (int)ScriptEntity[Id]->y+9, 100, 0, ScriptEntity[Id]->BloodCol);
			CreateExplosion((int)ScriptEntity[Id]->x+9, (int)ScriptEntity[Id]->y+9);
			ScriptEntity[Id]->life = 0;
			ScriptEntity[Id]->visible = false;			
			loop = false;
		}		

		if(strcmp(&ScriptIns[ScriptCount*20], "VISIBLE") == 0){
			amount = ScriptVal[ScriptCount];
			if(amount == 0)
				ScriptEntity[Id]->visible = false;
			else if(amount == 1)
				ScriptEntity[Id]->visible = true;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "XMOVE") == 0){
			MoveId = Id;
			Moving = true;
			amount = ScriptVal[ScriptCount];
			xMove = long(ScriptEntity[Id]->x+amount);
			if(amount > 0)			
				ScriptEntity[Id]->setspeed = ScriptEntity[Id]->maxspeed;
			else
				ScriptEntity[Id]->setspeed = -ScriptEntity[Id]->maxspeed;
			loop = false;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "XMOVETO") == 0){
			MoveId = Id;
			Moving = true;
			amount = ScriptVal[ScriptCount];
			xMove = amount;
			if(ScriptEntity[Id]->x < xMove)
				ScriptEntity[Id]->setspeed = ScriptEntity[Id]->maxspeed;
			else
				ScriptEntity[Id]->setspeed = -ScriptEntity[Id]->maxspeed;
			loop = false;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "NEXTLEVEL") == 0){
			Level++;
			NextLevel(Level);
			//EndOfLevel = true;
			loop = false;
		}
		
		
		if(strcmp(&ScriptIns[ScriptCount*20], "ISHURT") == 0){
			LD3.LoadSprite(&WinApp, "gfx/rustyhrt.put", RUSTY_SPRITE_SET, 80);
			loop = true;
		}		

		if(strcmp(&ScriptIns[ScriptCount*20], "DIE") == 0){
			ScriptEntity[Id]->life = 0;
			x = (int)ScriptEntity[Id]->x;
			y = (int)ScriptEntity[Id]->y;
			Splatter(x+10, y+10, 40, 0, ScriptEntity[Id]->BloodCol);
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "MOVETOPOS1") == 0){
			Player->x = Position[0].x;
			Player->y = Position[0].y;
			Marker.x = Player->x;
			Marker.y = Player->y;
			RemoveEntities();
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "MOVETOPOS2") == 0){
			Player->x = Position[1].x;
			Player->y = Position[1].y;
			Marker.x = Player->x;
			Marker.y = Player->y;
			RemoveEntities();
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "SETSPEED") == 0){
			amount = ScriptVal[ScriptCount];
			ScriptEntity[Id]->setspeed = (float)amount;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "FADETOBLACK") == 0){
			Fading = true;
			FadeToBlack = true;
			loop = false;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "FADETOWHITE") == 0){
			Fading = true;
			FadeToWhite = true;
			loop = false;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "FADEOUTOFBLACK") == 0){
			Fading = true;
			FadeOutOfBlack = true;
			loop = false;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "FADEOUTOFWHITE") == 0){
			Fading = true;
			FadeOutOfWhite = true;
			loop = false;
		}
		
		if(strcmp(&ScriptIns[ScriptCount*20], "YMOVE") == 0){
			amount = ScriptVal[ScriptCount];
			ScriptEntity[Id]->y += amount;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "XFLIP") == 0){
			amount = ScriptVal[ScriptCount];
			if(amount <= 0) ScriptEntity[Id]->xFlip = false; else ScriptEntity[Id]->xFlip = true;
			loop = false;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "TALK") == 0){
			if(Id == 0)
				TalkSprite = 0;
			else{
				if(ScriptEntity[Id]->id == LARRY)		TalkSprite = 142;
				if(ScriptEntity[Id]->id == FLETCHER)	TalkSprite = 144;
				if(ScriptEntity[Id]->id == XAHN)		TalkSprite = 146;
				if(ScriptEntity[Id]->id == MARK)		TalkSprite = 148;
				if(ScriptEntity[Id]->id == RUSTY)		TalkSprite = 150;
				if(ScriptEntity[Id]->id == GENERAL)		TalkSprite = 152;
			}
			
			EntityTalk(&ScriptMsg[MsgCount*80], 2);		
			MsgCount++;
			
			TalkId = TalkSprite+1;
			//LD3.PutSprite(&WinApp, 0, 0, 1, TalkId, 0);
			//LD3.DrawTalkBox(&WinApp);
			Talking = true;
			loop = false;
			TalkDelayCount = 0;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "AIANGRY") == 0){
			ScriptEntity[Id]->aiState = aiANGRY;
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "AICALM") == 0){
			ScriptEntity[Id]->aiState = aiCALM;
			loop = true;
		}
		if(strcmp(&ScriptIns[ScriptCount*20], "AISCARED") == 0){
			ScriptEntity[Id]->aiState = aiSCARED;
			loop = true;
		}

		if(strcmp(&ScriptIns[ScriptCount*20], "LOCKINPUT") == 0){
			amount = ScriptVal[ScriptCount];
			if(amount == 0){
				LockInput = false;
				CutScene = false;
			}
			if(amount == 1){
				LockInput = true;
				CutScene = true;
			}
			loop = true;
		}
		//EntityTalk(&ScriptMsg[MsgCount*0], 2);

		ScriptCount++;
	}
	if(Moving){
		if(ScriptEntity[MoveId]->setspeed > 0){
			if(ScriptEntity[MoveId]->x >= xMove){
				Moving = false;
				ScriptEntity[MoveId]->setspeed = 0;
			}
		}
		if(ScriptEntity[MoveId]->setspeed < 0){
			if(ScriptEntity[MoveId]->x <= xMove){
				Moving = false;
				ScriptEntity[MoveId]->setspeed = 0;
			}
		}		
	}
	if(SkipScene){
		loop = true;
		Moving = false;
		Fading = false;
		WaitMusicFade = false;
		Shifting = false;
		ScriptWait = 0;
		CutScene = false;
		Talking = false;		
	}
		
	ScriptWait--;
	if(ScriptWait < 0) ScriptWait = 0;
}

//=================================================================================================
//= void ProcessMonsters()
//= This function processes the monsters' AI
//= This function also processing the moving and falling platforms
//= Technically, platforms are monsters with adnormal properties
//=================================================================================================
void ProcessMonsters()
{
	ENTITY	*m;
	int		mx, my, mp, mp2, mp3, rnd, nm, n, p;
	long	mpt;
	bool	OnPlatform;
	float	oldx, newx, oldy, newy;
	float   xdist, ydist;
	bool	LastScript = false;
	PARTICLE *bp;
	float   pv, lv;
	float	y1, y2;
	bool	InElevatorRange = false;
	bool	Exiting = false;

	y1 = PlayerY1+15;
	y2 = PlayerY2+15;

	//- Draw the monsters
	//Player->ox = Player->x;
	for(int i = 1; i <= NumMonsters; i++){
		m = &Monster[i];
		
		mpt = int(m->x/20)+int(m->y/20+1)*MAPWIDTH+55000;
		mp = LD3.Map[mpt];
		if(mp == miSpikes && m->ScoreValue > 0){
			m->life = 0;
			Splatter((int)m->x+10, (int)m->y+10, 40, 0, Player->BloodCol);
		}

		//m->aiState = aiCALM;

		OnPlatform = false;
		switch(Monster[i].id)
		{
		case BOMB:
			
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);			
			mx = (int)(m->x) / 20;
			my = (int)(m->y+35) / 20;			
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40) || m->life <= 0){
				CreateExplosion(m->x+9, m->y+9);
				m->life = 0;
				if(Boss->life <= 0){
					Boss->life = 1;
					Boss->count = 0;
					Boss->count2 = 40;
					MusicVolume = 0;
				}
			}		
			break;

		case DUD:
			
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);			
			mx = (int)(m->x) / 20;
			my = (int)(m->y+19) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40) || m->life <= 0){
				CreateExplosion(m->x+9, m->y+9);
				m->life = 0;
				if(Boss->life <= 0){
					Boss->life = 1;
					Boss->count = 0;
					Boss->count2 = 40;
					MusicVolume = 0;
				}
			}		
			break;

		case FLETCHER:
			
			m->life = 100;
			m->Move(diff);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
									
			break;
		
		case MARK:
			
			m->life = 100;
			m->Move(diff);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
									
			break;
		case RUSTY:
			
			m->life = 100;
			m->Move(diff);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
									
			break;
		case XAHN:
			
			m->Move(diff);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
									
			break;
		case GENERAL:
			
			m->Move(diff);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
									
			break;

		case GREENDOOR:

			if(HasGreenCard && fabs((Player->x+9)-(m->x+9)) < 50 && fabs((Player->y+9)-(m->y+9)) < 50){
				if(m->lani == m->lstart) FSOUND_PlaySound(FSOUND_FREE,sndDoorUp);
				m->lani += 0.25f*diff;
				if(m->lani > m->lstart+6){
					m->lani = m->lstart+6;
					LD3.Map[(int)m->count2] = 0;
					LD3.Map[(int)m->count3] = 0;
				}
			}
			else{
				if(m->lani == m->lstart+6) FSOUND_PlaySound(FSOUND_FREE,sndDoorDown);
				m->lani -= 0.25f*diff;
				if(m->lani < m->lstart){
					m->lani = m->lstart;
					LD3.Map[(int)m->count2] = INVISIBLEWALL;
					LD3.Map[(int)m->count3] = INVISIBLEWALL;
				}
			}
			break;

		case BLUEDOOR:

			if(HasBlueCard && fabs((Player->x+9)-(m->x+9)) < 50 && fabs((Player->y+9)-(m->y+9)) < 50){
				if(m->lani == m->lstart) FSOUND_PlaySound(FSOUND_FREE,sndDoorUp);
				m->lani += 0.25f*diff;
				if(m->lani > m->lstart+6){
					m->lani = m->lstart+6;
					LD3.Map[(int)m->count2] = 0;
					LD3.Map[(int)m->count3] = 0;
				}
			}
			else{
				if(m->lani == m->lstart+6) FSOUND_PlaySound(FSOUND_FREE,sndDoorDown);
				m->lani -= 0.25f*diff;
				if(m->lani < m->lstart){
					m->lani = m->lstart;
					LD3.Map[(int)m->count2] = INVISIBLEWALL;
					LD3.Map[(int)m->count3] = INVISIBLEWALL;
				}
			}
			break;

		case YELLOWDOOR:

			if(HasYellowCard && fabs((Player->x+9)-(m->x+9)) < 50 && fabs((Player->y+9)-(m->y+9)) < 50){
				if(m->lani == m->lstart) FSOUND_PlaySound(FSOUND_FREE,sndDoorUp);
				m->lani += 0.25f*diff;
				if(m->lani > m->lstart+6){
					m->lani = m->lstart+6;
					LD3.Map[(int)m->count2] = 0;
					LD3.Map[(int)m->count3] = 0;
				}
			}
			else{
				if(m->lani == m->lstart+6) FSOUND_PlaySound(FSOUND_FREE,sndDoorDown);
				m->lani -= 0.25f*diff;
				if(m->lani < m->lstart){
					m->lani = m->lstart;
					LD3.Map[(int)m->count2] = INVISIBLEWALL;
					LD3.Map[(int)m->count3] = INVISIBLEWALL;
				}
			}
			break;

		case REDDOOR:

			if(HasRedCard && fabs((Player->x+9)-(m->x+9)) < 50 && fabs((Player->y+9)-(m->y+9)) < 50){
				if(m->lani == m->lstart) FSOUND_PlaySound(FSOUND_FREE,sndDoorUp);
				m->lani += 0.25f*diff;
				if(m->lani > m->lstart+6){
					m->lani = m->lstart+6;
					LD3.Map[(int)m->count2] = 0;
					LD3.Map[(int)m->count3] = 0;
				}
			}
			else{
				if(m->lani == m->lstart+6) FSOUND_PlaySound(FSOUND_FREE,sndDoorDown);
				m->lani -= 0.25f*diff;
				if(m->lani < m->lstart){
					m->lani = m->lstart;
					LD3.Map[(int)m->count2] = INVISIBLEWALL;
					LD3.Map[(int)m->count3] = INVISIBLEWALL;
				}
			}
			break;

		case GBASLEEP:

			if(fabs(Player->x-m->x) < 60 && fabs(Player->y-m->y) < 60 && m->lani == 79){
				m->lani = 88;
				CreateExplosion(m->x+9, m->y+19);
				NumMonsters++;
				nm = NumMonsters;					
				Monster[nm].Init();
				Monster[nm].id			= GREENBLOB;
				Monster[nm].startx		= m->x;
				Monster[nm].starty		= m->y+5;				
				SetDefaultValue(&Monster[nm]);
				Monster[nm].aiState		= aiANGRY;
			}
			break;

		case ELEVATOR:

			//- m->setspeed == rate at which the doors are opening or closing ( > 0 is opening, < 0 is closing)
			//- if m->count2 == 2 then the doors are opening to let the player out

			if(m->count2 == 2) Exiting = true; else Exiting = false;

			//- check if the player is within range of the elevator
			if(Player->x+9 >= m->x && Player->x+9 <= m->x+39 && Player->y+9 >= m->y && Player->y+9 <= m->y+39){
				InElevatorRange = true;
				//m->count3 = 1;
			}
			else{
				InElevatorRange = false;
				//m->count3 = 0;
			}
			
			if(Exiting == true && InElevatorRange == false) {Exiting = false; m->count2 = 0;}
			if(Exiting) m->setspeed = 0.75f;		//- open the doors
			
			//- if the elevator is currently doing nothing, check if the player is within
			//- range for the doors to open or not
			if(m->setspeed == 0 && m->count == 0){			
				//- if the player is infront of the elevator, open the doors					
				if(InElevatorRange == true && Elevating == false) m->setspeed = 0.75f;
			}
			else{
				//- if the player isn't exiting, check if the player is
				//- within the elevator range to open the doors or not
				if(Exiting == false){
					
					if(InElevatorRange){
						if(Elevating == false) 
							m->setspeed = 0.75f;
						else
							m->setspeed = -0.75f;
					}
					else{
						//- close the doors since the player isn't within range
						m->setspeed = -0.75f;						
					}
				}

				m->count += m->setspeed*diff; //- move the doors in or out based on m->setspeed
				
				//- If the doors have opened to their extent, then either let the player in or
				//- out of the elevator based on their previous state
				if(m->count >= 20){
					m->count = 20; //- don't open the doors any further				
					if(InElevatorRange){
						if(Exiting){	
							//- let the player out of the elevator
							Player->nocheck = false;
							ShowPlayer = true;
							Elevating = false;
							LockInput = false;
							//m->count2 = 0; //- exiting = false next time around
						}
						else{
							//- let the player inside the elevator							
							ShowPlayer = false;
							Player->nocheck = true;
							Player->speed = 0;
							LockInput = true;
							Elevating = true;						
						}
					}
				}

				if(m->count < 0){
					m->count = 0;
					
					if(Elevating){
						//- move the player up or down
						//- m->count3 was set to either a positive or negative number at the start
						//- based on whether it was one leading up or one leading down
						Player->y += m->count3*diff;					

						//- check all the elevators except the one currently being processed to see if the player
						//- is at the right spot to exit from one of the elevators
						for(int i = 1; i <= NumElevators; i++){
							if(Elevator[i]->x != m->x || Elevator[i]->y != m->y){
								if(m->count3 > 0){
									if(Player->x+9 >= Elevator[i]->x && Player->x+9 <= Elevator[i]->x+39 && Player->y+19 >= Elevator[i]->y+40){
										Elevator[i]->count2 = 2; //- exiting will = true next time around
										m->count2 = 0;			 //- process this elevator no more
										m->setspeed = 0;
										Player->y = Elevator[i]->y+21;
										Player->velocity = 0;
										break;
									}
								}
								else{
									if(Player->x+9 >= Elevator[i]->x && Player->x+9 <= Elevator[i]->x+39 && Player->y+19 <= Elevator[i]->y+40){
										Elevator[i]->count2 = 2; //- exiting will = true next time around
										m->count2 = 0;			 //- process this elevator no more
										m->setspeed = 0;
										Player->y = Elevator[i]->y+21;
										Player->velocity = 0;
										break;
									}
								}
							}
						}
					}
					else
						m->setspeed = 0;
				}				
			}

			break;

		case SPIDER:
	
			m->ox = m->x;
			m->oy = m->y;
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			
			m->count -= diff;

			if(m->count <= 0 && (int)m->y == (int)m->oy){
				m->count = 20+rand()%90;

				rnd = rand()%2;
				if(rnd == 0){
					m->setspeed = -2;
					m->xFlip = true;
				}
				if(rnd == 1){
					m->setspeed = 2;
					m->xFlip = false;
				}
			}

			rnd = rand()%20;
			if(fabs(m->x-Player->x) < 50 && fabs(m->y-Player->y) < 80 && rnd == 5 && m->y == m->oy) m->velocity = -1.5f;

			if(fabs(m->y-Player->y) < 8 && fabs(m->x-Player->x) < 16){
				Player->life -= 1;
				Splatter(m->x+7, m->y+7, 5, m->setspeed, 36);
			}
			
			break;

		case RAT:
	
			m->ox = m->x;
			m->Move(diff);
			mp = m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			
			m->count -= diff;

			if((m->count <= 0 && m->friction == m->groundfriction) || mp > 10){
				m->count = 20+rand()%90;

				rnd = rand()%2;
				if(rnd == 0){
					m->setspeed = -2;
					m->xFlip = true;
				}
				if(rnd == 1){
					m->setspeed = 2;
					m->xFlip = false;
				}
			}
			
			break;

		case BIGSPIDER:
	
			m->ox = m->x;
			m->Move(diff);
			m->y += 5;
			mp = m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->y -= 5;
						
			m->count -= diff;

			if((m->count <= 0 && m->friction == m->groundfriction) || mp > 10){
				m->count = 20+rand()%90;

				rnd = rand()%2;
				if(rnd == 0){
					m->setspeed = -2;
					m->xFlip = true;
				}
				if(rnd == 1){
					m->setspeed = 2;
					m->xFlip = false;
				}
			}

			mx = (int)(m->x) / 20;
			if(m->xFlip) mx--; else mx++;
			my = (int)(m->y-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if(mp == 0){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 90;
				m->count = 10;
			}
			
			break;

		case BIGSPIDER2:
	
			m->ox = m->x;
			m->Move(diff);
			mp = m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
									
			m->count -= diff;

			if((m->count <= 0 && m->friction == m->groundfriction) || mp > 10){
				m->count = 20+rand()%90;

				rnd = rand()%2;
				if(rnd == 0){
					m->setspeed = -2;
					m->xFlip = true;
				}
				if(rnd == 1){
					m->setspeed = 2;
					m->xFlip = false;
				}
			}

			mx = (int)(m->x) / 20;
			if(m->xFlip) mx--; else mx++;
			my = (int)(m->y+15) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if(mp == 0){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 85;
				m->count = 10;
			}
			
			break;

		case ALDOG:
	
			m->ox = m->x;
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);

			m->count -= diff;

			if(m->count <= 0){
				
				rnd = rand()%2;
				if(m->aiState == aiCALM){
					m->count = 40+rand()%90;
					if(rnd == 0){
						if(m->setspeed >= -1)
							m->setspeed = -1.5f;
						else
							m->setspeed = -1.0f;
						m->xFlip = true;
					}
					if(rnd == 1){
						if(m->setspeed <= 1)
							m->setspeed = 1.5f;
						else
							m->setspeed = 1.0f;
						m->xFlip = false;
					}
				}
				else if(m->aiState == aiANGRY){
					m->count = rand()%40;
					if(m->x < Player->x){
						m->setspeed = 2.2f;
						m->xFlip = false;
					}
					else{
						m->setspeed = -2.2f;
						m->xFlip = true;
					}

					if(fabs(m->x-Player->x) < 60 && m->friction == m->groundfriction){
						m->velocity = -2.0f;					
					}
				}
				else if(m->aiState == aiSCARED){
					if(m->x < Player->x){
						m->setspeed = -1.7f;
						m->xFlip = true;
					}
					else{
						m->setspeed = 1.7f;
						m->xFlip = false;
					}
					
				}
			}

			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp < 40 || mp2 < 40) && m->friction == m->groundfriction){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 0;
				m->count = 10;
			}

			break;

		case VROCK_MONSTER:
	
			m->ox = m->x;
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);

			m->uani += 0.25f*diff;
			if(m->uani >= m->uend) m->uani = m->ustart;

			if(m->friction == m->airfriction) m->uani = m->ustart+2;

			//- create blood drop particle			
			rnd = rand()%30;
			if(rnd == 1){
			if(NumParticles < 490) NumParticles++;
				bp = &Particle[NumParticles];
				bp->x = m->x+rand()%19;
				bp->y = m->y+10+rand()%5;
				bp->xspeed = 0;
				bp->yspeed = 0.1f;
				bp->timer  = 120;
				bp->yvelocity = 0;
				bp->col    = 34+rand()%2;
			}

			m->count -= diff;

			if(m->count <= 0){
				
				rnd = rand()%3;
				if(m->aiState == aiCALM){
					m->count = 40+rand()%90;
					if(rnd == 0){
						if(m->setspeed >= -1)
							m->setspeed = -1.5f;
						else
							m->setspeed = -1.0f;
						m->xFlip = true;
					}
					if(rnd == 1){
						if(m->setspeed <= 1)
							m->setspeed = 1.5f;
						else
							m->setspeed = 1.0f;
						m->xFlip = false;
					}
					if(rnd == 2) m->setspeed = 0.0f;
				}
				else if(m->aiState == aiANGRY){
					m->count = rand()%40;
					if(m->friction == m->groundfriction){
						if(m->x < Player->x){
							m->setspeed = 2.2f;
							m->xFlip = false;
						}
						else{
							m->setspeed = -2.2f;
							m->xFlip = true;
						}
					}
					else{
						if(m->setspeed > 0) m->setspeed = 2.2f; else m->setspeed = -2.2f;						
					}

					//if(fabs(m->x-Player->x) < 60 && m->friction == m->groundfriction){
					//	m->velocity = -2.0f;
					//}
				}
				else if(m->aiState == aiSCARED){
					if(m->x < Player->x){
						m->setspeed = -1.7f;
						m->xFlip = true;
					}
					else{
						m->setspeed = 1.7f;
						m->xFlip = false;
					}
					
				}
			}

			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp < 40 || mp2 < 40) && m->friction == m->groundfriction){
				
				if(m->aiState == aiANGRY && fabs(m->x-Player->x) < 100 && (Player->y-m->y) > -40 && (Player->y-m->y) < 200 && m->friction == m->groundfriction && Player->friction == Player->groundfriction){
					m->velocity = -2.0f;
					if(m->x < Player->x){
						m->setspeed = 2.5f;
						m->xFlip = false;
					}
					else{
						m->setspeed = -2.5f;
						m->xFlip = true;
					}
					rnd = rand()%5;
					if(rnd == 1) FSOUND_PlaySound(FSOUND_FREE,sndRock[0]);
				}
				else{				
					m->x = m->ox;
					m->setspeed = 0;
					m->count = 0;
					m->lani  = 150;
					m->count = 10;
				}
			}

			if(m->x+9 >= Player->x && m->x+9 <= Player->x+19 && m->y+10 >= Player->y && m->y+10 <= Player->y + 19){
				//if(m->uani > m->ustart+4 && m->uani < m->uend){
					if(m->xFlip)
						Splatter(m->x+6, m->y+10, 14, 0, 36);
					else
						Splatter(m->x+13, m->y+10, 14, 0, 36);
					Player->life -= 1;
				//}
			}		

			break;

		case BLOB_MINE:
	
			m->ox = m->x;
			if(m->lani < 4){
				m->Move(diff);
				m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
				//m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			}

			m->count -= diff;

			if(m->count <= 0){
				
				rnd = rand()%2;
				if(m->aiState == aiCALM){
					m->count = 50+rand()%100;
					if(rnd == 0){
						m->setspeed = -0.5f;
						//m->x += m->setspeed;
						m->xFlip = true;
					}
					if(rnd == 1){
						m->setspeed = 0.5f;
						//m->x += m->setspeed;
						m->xFlip = false;
					}					
				}				

			}

			if(m->life < 0) m->life = 1;
			
			if(m->lani >= 4){
				m->lani += 0.2f*diff;
				if(m->lani >= 9) CreateExplosion(m->x+9, m->y+9);
			}

			if(fabs(m->x-Player->x) < 30 && fabs(m->y-Player->y) < 30 && m->lani < 4) m->lani = 4;
			if(m->aiState == aiANGRY && m->lani < 4) m->lani = 4;

			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp < 40 || mp2 < 40) && m->friction == m->groundfriction){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 0;
				m->count = 10;
			}

			break;


		case GREENBLOB:
	
			m->ox = m->x;
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);

			m->count -= diff;

			if(m->aiState == aiANGRY){
				rnd = rand()%35;
				if(rnd == 2){
					if(m->xFlip)
						FireProjectile(m->x+5, m->y, 160, 3.5f, LOOGIE, m, 4);
					else
						FireProjectile(m->x+5, m->y, 20, 3.5f, LOOGIE, m, 4);
				}
			}

			if(m->count <= 0){
				
				rnd = rand()%2;
				if(m->aiState == aiCALM){
					m->count = 50+rand()%100;
					if(rnd == 0){
						m->setspeed = -1.0f;
						m->x += m->setspeed;
						m->xFlip = true;
					}
					if(rnd == 1){
						m->setspeed = 1.0f;
						m->x += m->setspeed;
						m->xFlip = false;
					}

					rnd = rand()%5;
					if(rnd == 2){
						if(m->xFlip)
							FireProjectile(m->x+5, m->y+5, 160, 3.5f, LOOGIE, m, 4);
						else
							FireProjectile(m->x+5, m->y+5, 20, 3.5f, LOOGIE, m, 4);
					}
				}
				else if(m->aiState == aiANGRY){
					m->count = rand()%40;
					if(m->x < Player->x){
						m->setspeed = 1.2f;
						m->x += m->setspeed;
						m->xFlip = false;
					}
					else{
						m->setspeed = -1.2f;
						m->x += m->setspeed;
						m->xFlip = true;
					}					
				}
				else if(m->aiState == aiSCARED){
					if(m->x < Player->x){
						m->setspeed = -1.2f;
						m->x += m->setspeed;
						m->xFlip = true;
					}
					else{
						m->setspeed = 1.2f;
						m->x += m->setspeed;
						m->xFlip = false;
					}
					
				}
			}

			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if((mp < 40 || mp2 < 40) && m->friction == m->groundfriction){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 140;
				m->count = 10;
			}

			break;

		case BLOODHEAD:
	
			m->ox = m->x;
			m->Move(diff);
			mp = m->CheckWallHit(&LD3.Map[0], MAPWIDTH);

			m->count -= diff;
			if(mp) m->count = 0;

			if(fabs((Player->x+9)-(m->x+9)) < 15 && fabs((Player->y+9)-(m->y+9)) < 15){
				if(m->xFlip)
					Splatter(m->x+9, m->y+9, 10, -1.0f, 37);
				else
					Splatter(m->x+9, m->y+9, 10, 1.0f, 37);
			}

			if(m->aiState == aiANGRY){
				if(m->y+9 < Player->y) m->y += 0.2f*diff;
				if(m->y+9 > Player->y) m->y -= 0.2f*diff;
			}

			if(m->count <= 0){
				
				rnd = rand()%2;
				if(m->aiState == aiCALM){
					m->count = 40+rand()%80;
					if(rnd == 0){
						m->setspeed = -1.0f;
						//m->x += m->setspeed;
						m->xFlip = true;
					}
					if(rnd == 1){
						m->setspeed = 1.0f;
						//m->x += m->setspeed;
						m->xFlip = false;
					}
					
				}
				else if(m->aiState == aiANGRY){
					m->count = rand()%40;
					if(m->x < Player->x){
						m->setspeed = 1.5f;
						//m->x += m->setspeed;
						m->xFlip = false;
					}
					else{
						m->setspeed = -1.5f;
						//m->x += m->setspeed;
						m->xFlip = true;
					}					
				}
				else if(m->aiState == aiSCARED){
					if(m->x < Player->x){
						m->setspeed = -1.5f;
						//m->x += m->setspeed;
						m->xFlip = true;
					}
					else{
						m->setspeed = 1.5f;
						//m->x += m->setspeed;
						m->xFlip = false;
					}
					
				}
			}			

			break;
			

		case JELLY_BLOB:		
			
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->uani += 0.2f;
			if(m->uani >= m->uend+1) m->uani = m->ustart;				

			switch(m->aiState)
			{
			case aiCALM:
						
				//m->y = m->oy + Sine[int(m->uani*10)]*100;
				if(fabs(Player->x-m->x) < 50 && fabs(Player->y-m->y) < 30){
					m->aiState = aiANGRY;
					m->count2  = 1;
					m->lani	   = 131;
				}
				break;

			case aiANGRY:
				
				if(m->x < Player->x) m->setspeed = 1;
				if(m->x > Player->x) m->setspeed = -1;
				
				if(m->count2){
					m->count2++;
					m->lani = (float)131+((int)m->count2>>4);
					if(m->count2 > 50){
						m->count2 = 0;
						m->count = 1;
					}
				}

				if(m->count){
					m->count++;
					m->lani = (float)133+((int)m->count>>2);
					if(m->count > 15) m->count = 1;
					if(fabs(m->x-Player->x) < 20 && fabs(m->y-Player->y) < 20 && m->lani == (int)m->lani){
						Player->life -= 1;
						Splatter(Player->x, m->y+10, 10, 0, 36);
					}
				}		
				
				break;
			}			
			break;

		case SPIKEPLANT:
			
			if(fabs(Player->x-m->x) < 50 && fabs(Player->y-m->y) < 50 && m->lani == 11){
				m->count = 1;
				m->lani = 12;
			}
			if(m->count > 0){
				//- make spike plant open
				m->count += diff;
				m->lani = (float)12+((int)m->count>>3);
				if(m->count >= 17){
					FireProjectile(m->x, m->y, 10, 1.5f, COTTONSPIKE, m, 0);
					FireProjectile(m->x, m->y, 50, 1.5f, COTTONSPIKE, m, 0);
					FireProjectile(m->x, m->y, 120, 1.5f, COTTONSPIKE, m, 0);
					FireProjectile(m->x, m->y, 170, 1.5f, COTTONSPIKE, m, 0);
					FireProjectile(m->x, m->y, 90, 1.5f, COTTONSPIKE, m, 0);
					m->count = 0;
				}
			}
			if((int)m->lani == 14 || m->lani == 16){
				int py = (int)Player->y+Player->HitBox.bottom;
				int px = (int)Player->x+9;
				int my = (int)m->y+m->HitBox.top;
				int mx = (int)m->x;
				if(Player->velocity > 0 && py > my && py < my+6 && px > mx+m->HitBox.left && px < mx+m->HitBox.right){
					if(m->lani == 14) m->lani = 15; else m->lani = 14;
					m->count2 = 0;
				}				
			}
			if(m->lani == 15){
				int py = (int)Player->y+Player->HitBox.bottom;
				int px = (int)Player->x+9;
				int my = (int)m->y+m->HitBox.top;
				int mx = (int)m->x;
				m->count2 += diff;
				if(m->count2 > 20 || (py < my || py > my+7) || (px < mx+m->HitBox.left || px > mx+m->HitBox.right)){
					m->lani = 16;
					Player->velocity = -9;					
					FSOUND_PlaySound(FSOUND_FREE,sndBounce);
				}
			}
			break;

		case SONIC_MONSTER:
			switch(Monster[i].aiState)
			{
			case aiCALM:
				
				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					if(m->count >= 0){
						m->lani += 0.1f*diff;
						if(m->lani >= 14){
							m->lani = 14;
							m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
							m->Fall(&LD3.Map[0], MAPWIDTH, diff);
							rnd = 10/diff;
							if(rand()%rnd == 1){
								if(m->xFlip == false){
									FireProjectile(m->x, m->y, 0, 2, SONIC_WAVE, m, 4);
									//m->x -= 1-rand()%2;
								}
								else{
									FireProjectile(m->x, m->y, 180, 2, SONIC_WAVE, m, 4);
									//m->x += 1-rand()%2;
								}
							}
							m->count -= diff;					
						}
					}
					else
					{
						m->lani -= 0.1f;
						if(m->lani <= 8){
							m->attacking	= false;
							m->lstart		= 0;
							m->lend			= 9;
							m->lani			= 0;
						}
					}
				}
				else
				{
					m->ox = m->x;
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);

					if(m->count <= 0){
						m->setspeed = float(1-rand()%3);
						m->count = 30+rand()%110;
						if(m->setspeed < 0) m->xFlip = true;
						else m->xFlip = false;

						if(rand()%6 == 1){
							m->attacking= true;
							m->lstart	= 9;
							m->lend		= 14;
							m->lani		= 9;
							m->setspeed = 0;
							m->speed	= 0;
							m->count	= 500;
						}
					}
					m->count -= diff;
					if(m->count <= 0) m->setspeed = 0;

					mx = (int)(m->x) / 20;
					my = (int)(m->y+19-4) / 20;
					mpt = mx+(my<<MAPXLSHIFT)+55000;
					mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
					if(mp < 40 || mp2 < 40){
						m->x = m->ox;
						m->setspeed = 0;
						m->count = 0;
						m->lani  = 0;
					}
				}
				
				break;
			
			case aiSCARED:

				
				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					m->lani -= 0.2f;
					if(m->lani <= 8){
						m->attacking	= false;
						m->lstart		= 0;
						m->lend			= 9;
						m->lani			= 0;
					}
					m->count = 600;
				}
				else{
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
				
					if(m->x >= Player->x) m->setspeed = 1.3f;
					else m->setspeed = -1.3f;
					if(m->setspeed < 0) m->xFlip = true;
					else m->xFlip = false;
					m->count -= diff;
					if(m->count <= 0)
						if(fabs(Player->x - m->x) < 100) m->aiState = aiANGRY; else m->aiState = aiSCARED;					
				}								
				break;
				
			case aiANGRY:

				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					if(m->count >= 0){
						m->lani += 0.2f;
						if(m->lani >= 14){
							m->lani = 14;
							m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
							m->Fall(&LD3.Map[0], MAPWIDTH, diff);
							if(rand()%10 == 1){
								if(m->xFlip == false){
									FireProjectile(m->x, m->y, 0, 2, SONIC_WAVE, m, 4);
									//m->x -= 1-rand()%2;
								}
								else{
									FireProjectile(m->x, m->y, 180, 2, SONIC_WAVE, m, 4);
									//m->x += 1-rand()%2;
								}
							}
							m->count--;
							if(fabs(Player->x-m->x) >= 100) m->count = -1;
							if(Player->x < m->x && m->xFlip == false) m->count = -1;
							if(Player->x > m->x && m->xFlip) m->count = -1;
						}
					}
					else
					{
						m->lani -= 0.2f*diff;
						if(m->lani <= 8){
							m->attacking	= false;
							m->lstart		= 0;
							m->lend			= 9;
							m->lani			= 0;
						}
					}
				}
				else
				{
					m->ox = m->x;
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					
					//m->setspeed = 1-rand()%3;
					if(m->x < Player->x) m->setspeed = 1; else m->setspeed = -1;
					m->count = 30+rand()%110;
					if(m->setspeed < 0) m->xFlip = true;
					else m->xFlip = false;

					if(fabs(Player->x-m->x) < 100){
						m->attacking= true;
						m->lstart	= 9;
						m->lend		= 14;
						m->lani		= 9;
						m->setspeed = 0;
						m->speed	= 0;
						m->count	= 500;
					}

					mx = (int)(m->x) / 20;
					my = (int)(m->y+19-4) / 20;
					mpt = mx+(my<<MAPXLSHIFT)+55000;
					mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
					if(mp < 40 || mp2 < 40){
						m->x = m->ox;
						m->setspeed = 0;
						m->count = 0;
						m->lani  = 0;
					}
					//m->count--;
					//if(m->count == 0) m->setspeed = 0;
				}
				
				break;

			}
			
			break;

		case ROCK_MONSTER:
			
			switch(Monster[i].aiState)
			{
			case aiCALM:
				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					if(m->count >= 0){
						m->lani += 0.2f*diff;
						if(m->lani >= 44){
							m->lani = 44;
							m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
							m->Fall(&LD3.Map[0], MAPWIDTH, diff);
							m->count -= diff;
							if(m->count <= 0){
								if(m->xFlip == false){
									FireProjectile(m->x, m->y, 0, 2.2f, YELLOW_FIRE, m, 4);
									//m->x -= 1-rand()%2;
								}
								else{
									FireProjectile(m->x, m->y, 180, 2.2f, YELLOW_FIRE, m, 4);
									//m->x += 1-rand()%2;
								}
								m->count = 60+rand()%30;
							}
							if(fabs(Player->x-m->x) >= 150) m->count -= diff;
							if(Player->x < m->x && m->xFlip == false) m->count -= diff;
							if(Player->x > m->x && m->xFlip) m->count -= diff;
						}
					}
					else
					{
						m->lani -= 0.1f*diff;
						if(m->lani <= 40){
							m->attacking	= false;
							m->lstart		= 30;
							m->lend			= 39;
							m->lani			= 30;
						}
					}
				}
				else
				{
					m->ox = m->x;
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
						
					if(m->count <= 0){
						m->setspeed = float(1-rand()%3)*0.5f;
						m->count = 30+rand()%110;
						if(m->setspeed < 0) m->xFlip = true;
						else m->xFlip = false;

						if(rand()%3 == 1){
							m->attacking= true;
							m->lstart	= 40;
							m->lend		= 45;
							m->lani		= 40;
							m->setspeed = 0;
							m->speed	= 0;
							m->count	= 500;
						}
					}
					m->count -= diff;
					if(m->count == 0) m->setspeed = 0;

					mx = (int)(m->x) / 20;
					my = (int)(m->y+19-4) / 20;
					mpt = mx+(my<<MAPXLSHIFT)+55000;
					mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
					if(mp < 40 || mp2 < 40){
						m->x = m->ox;
						m->setspeed = 0;
						m->count = 0;
						m->lani  = 30;
					}
					break;
				}
				break;
			case aiSCARED:
				
				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					m->lani -= 0.2f*diff;
					if(m->lani <= 40){
						m->attacking	= false;
						m->lstart		= 30;
						m->lend			= 39;
						m->lani			= 30;
					}
					m->count = 600;
				}
				else{
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
				
					if(m->x >= Player->x) m->setspeed = 0.7f;
					else m->setspeed = -0.7f;
					if(m->setspeed < 0) m->xFlip = true;
					else m->xFlip = false;
					m->count -= diff;
					if(m->count <= 0)
						if(fabs(Player->x - m->x) < 100) m->aiState = aiANGRY; else m->aiState = aiSCARED;					
				}								
				break;				
			
			case aiANGRY:

				if(m->attacking){
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					if(m->count >= 0){
						m->lani += 0.2f*diff;
						if(m->lani >= 44){
							m->lani = 44;
							m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
							m->Fall(&LD3.Map[0], MAPWIDTH, diff);
							m->count -= diff;
							if(m->count <= 0){
								if(m->xFlip == false){
									FireProjectile(m->x, m->y, 0, 2.2f, YELLOW_FIRE, m, 4);
									//m->x -= 1-rand()%2;
								}
								else{
									FireProjectile(m->x, m->y, 180, 2.2f, YELLOW_FIRE, m, 4);
									//m->x += 1-rand()%2;
								}
								m->count = 60+rand()%30;
							}
							if(fabs(Player->x-m->x) >= 150) m->count -= diff;
							if(Player->x < m->x && m->xFlip == false) m->count -= diff;
							if(Player->x > m->x && m->xFlip) m->count -= diff;
						}
					}
					else
					{
						m->lani -= 0.2f*diff;
						if(m->lani <= 40){
							m->attacking	= false;
							m->lstart		= 30;
							m->lend			= 39;
							m->lani			= 30;
						}
					}
				}
				else
				{
					m->ox = m->x;
					m->Move(diff);
					m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
					m->Fall(&LD3.Map[0], MAPWIDTH, diff);
					
					//m->setspeed = 1-rand()%3;
					if(m->x < Player->x) m->setspeed = 0.6f; else m->setspeed = -0.6f;
					m->count = 30+rand()%110;
					if(m->setspeed < 0) m->xFlip = true;
					else m->xFlip = false;

					if(fabs(Player->x-m->x) < 150){
						m->attacking= true;
						m->lstart	= 40;
						m->lend		= 45;
						m->lani		= 40;
						m->setspeed = 0;
						m->speed	= 0;
						m->count	= 10;
					}

					mx = (int)(m->x) / 20;
					my = (int)(m->y+19-4) / 20;
					mpt = mx+(my<<MAPXLSHIFT)+55000;
					mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
					if(mp < 40 || mp2 < 40){
						m->x = m->ox;
						m->setspeed = 0;
						m->count = 0;
						m->lani  = 30;
					}
					//m->count--;
					//if(m->count == 0) m->setspeed = 0;
				}
				
				break;
			}
			break;

		case TROOP:

			//- move troop
			m->ox = m->x;
			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);

			//- make sure troop dosen't fall off edge
			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
			if(mp < 40 || mp2 < 40){
				m->x = m->ox;
				m->setspeed = 0;
				m->count = 0;
				m->lani  = 10;
			}

			if(m->attacking){
				m->count = 2;
				m->uani += 0.25f*diff;
				if(m->uani >= m->ustart + 3){
					m->uani = m->ustart + 1;
					FSOUND_PlaySound(FSOUND_FREE, sndRifle);
					if(m->xFlip)
						FireProjectile(m->x, m->y+10, 180, 15, MACHINEGUN, m, 0);
					else
						FireProjectile(m->x+19, m->y+10, 0, 15, MACHINEGUN, m, 0);
				}
				if(m->x + 9 < Player->x + 9)
					m->xFlip = false;
				else
					m->xFlip = true;

				if(fabs(m->x-Player->x > 100) || fabs(m->y-Player->y) > 40){
					m->uani = m->ustart;
					m->attacking = false;
				}
			}

			switch(Monster[i].aiState)
			{
			case aiCALM:				
				
				if(m->count <= 0){
					//m->setspeed = float(1-rand()%3);
					rnd = rand()%2;
					if(rnd == 1){
						m->setspeed = 1.0f;
						m->xFlip = false;
					}
					else{
						m->setspeed = -1.0f;
						m->xFlip = true;
					}
					if(fabs(m->x-Player->x) < 60) m->setspeed = 0;
					m->count = 30+rand()%110;						
				}
				m->count--;
				if(m->count <= 0) m->setspeed = 0;

				if(fabs(m->x-Player->x) < 100 && fabs(m->y-Player->y) < 40){
					m->setspeed = 0;
					m->attacking = true;
				}

				break;
			
			case aiSCARED:

				if(m->x >= Player->x) m->setspeed = 1.3f;
				else m->setspeed = -1.3f;
				if(m->setspeed < 0) m->xFlip = true;
				else m->xFlip = false;
				m->count -= 1;
				if(m->count <= 0)
					if(fabs(Player->x-m->x) < 100 && fabs(m->y-Player->y) < 40) m->aiState = aiANGRY; else m->aiState = aiSCARED;					
												
				break;
				
			case aiANGRY:

				//m->setspeed = 1-rand()%3;
				if(m->x < Player->x){
					m->setspeed = 1.0f;
					m->xFlip = false;
				}
				else{
					m->setspeed = -1;
					m->xFlip = true;
				}
				if(fabs(m->x-Player->x) < 60) m->setspeed = 0;
				m->count = 2;
				
				if(fabs(m->x-Player->x) < 100 && fabs(m->y-Player->y) < 40) m->attacking = true;
				
				break;
			}
			
			break;

		
		case ICED_SPIKEHEAD:
			
			switch(m->aiState)
			{
				case aiCALM:
				
				m->ox = m->x;
				m->Move(diff);
				m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
				m->Fall(&LD3.Map[0], MAPWIDTH, diff);

				if(m->count <= 0){
					m->count    = 30+rand()%30;
					m->setspeed = float(1-rand()%3)*2;
				}					
				m->count -= diff;
				
				mx = (int)(m->x) / 20;
				my = (int)(m->y+19-4) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+1];
				if(mp < 40 || mp2 < 40){
					m->x = m->ox;
					m->setspeed = 0;
					m->count = 0;					
				}

				if((Player->y+1 >= m->y+7 || Player->y+1 >= m->y+19) || (Player->y+19 >= m->y+7 || Player->y+19 >= m->y+19)){
					if((Player->x+3 >= m->x+1 && Player->x+3 <= m->x+18) || (Player->x+16 >= m->x+1 && Player->x+16 <= m->x+18)){
						Player->life -= 1;
						Splatter((int)m->x+10, (int)m->y+10, 20, 0, Player->BloodCol);
					}
				}

				break;
			}

			break;

		case FLY_MONSTER:
			
			
			m->lani += 0.5f*diff;
			if(m->lani >= m->lend+1) m->lani = m->lstart;

			m->x += Cosine[(int)m->count2]*2;
			m->y -= Sine[(int)m->count2]*2;

			if(m->x > m->ox + 30) m->x = m->ox + 30;
			if(m->x < m->ox - 30) m->x = m->ox - 30;
			if(m->y > m->oy + 30) m->y = m->oy + 30;
			if(m->y < m->oy - 30) m->y = m->oy - 30;

			m->count -= diff;
			if(m->count <= 0){
				m->count2 = rand()%360;
				m->count = 20+rand()%30;
			}
			
			
			if((m->x+10 >= Player->x && m->x+10 <= Player->x+19) && (m->y+10 >= Player->y && m->y+10 <= Player->y+19)){
				Player->life -= 1;
				Splatter((int)m->x+10, (int)m->y+10, 10, 0, Player->BloodCol);
			}
			
			break;
		
		case MEGAFLY:
			
			
			m->lani += 0.5f*diff;
			if(m->lani >= m->lend+1) m->lani = m->lstart;

			//- ox and oy are the directional vectors
			m->x += m->ox;//Cosine[(int)m->count2]*2;
			m->y -= m->oy;//Sine[(int)m->count2]*2;
			
			//m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			//m->velocity = -m->gravity;
			//m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			
			if(m->x > m->startx + 50) m->x = m->startx + 50;
			if(m->x < m->startx - 50) m->x = m->startx - 50;
			if(m->y > m->starty + 50) m->y = m->starty + 50;
			if(m->y < m->starty - 50) m->y = m->starty - 50;

			xdist = Player->x-m->startx;
			ydist = m->starty-Player->y;

			m->count -= diff;
			if(m->count <= 0){
				m->count2 = rand()%360;
				m->count = 10+rand()%30;
				if(m->count2 <= 90 || m->count2 > 270) m->xFlip = false; else m->xFlip = true;
				m->ox = Cosine[(int)m->count2]*2;
				m->oy = Sine[(int)m->count2]*2;
			}

			if(fabs(xdist) < 100 && fabs(ydist) < 100){
				m->count3 = sqrt(xdist*xdist+ydist*ydist);
				m->ox = xdist/m->count3;
				m->oy = ydist/m->count3;
				m->ox *= 4;
				m->oy *= 4;
				if(m->ox > 0) m->xFlip = false; else m->xFlip = true;
				m->lstart = 37;
				m->lend = 38;
				//m->life = 0;
			}
			else{
				m->lstart = 35;
				m->lend = 36;
			}
			
			if((m->x+10 >= Player->x && m->x+10 <= Player->x+19) && (m->y+10 >= Player->y && m->y+10 <= Player->y+19)){
				Player->life -= 1;
				Splatter((int)m->x+10, (int)m->y+10, 10, 0, Player->BloodCol);
			}
			
			break;
		
		case PLATFORM_CIRCLE:

			OnPlatform = false;

			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;

			if(m->count3 == 0){
				if(pv >= 0 && m->setspeed <= lv){
					if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv){
						OnPlatform = true;
						PlatformVelocity = m->setspeed;
					}
				}
			}

			oldx = m->x;
			oldy = m->y;
			
			m->x = m->ox + Cosine[(int)m->count2]*50;
			m->y = m->oy - Sine[(int)m->count2]*50;
						
			newx = m->x;
			newy = m->y;

			if(OnPlatform){				
				Player->y = m->y-15;
				Player->velocity = 0;
				Player->friction = Player->groundfriction;
				Player->x += (newx-oldx);				
			}
			
			m->count2 += (int)m->setspeed*diff;
			m->count = 10;

			if(m->count2 <    0) m->count2 += 360;
			if(m->count2 >= 360) m->count2 -= 360;

			if(m->count3 > 0){
				m->count3 -= diff;
				if(m->count3 <= 0){
					m->count3 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count3 = 500;
				m->visible = false;				
			}
			if(m->count3 < 100 && m->count3 > 0){
				if(((int)m->count3 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
			
			break;
		
		case SMASH:

			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;

			if(pv >= 0 && m->setspeed <= lv){
				if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv){
					OnPlatform = true;
					PlatformVelocity = m->setspeed;
				}				
			}
			if(Player->y+15 > m->y+Player->velocity+2 && Player->y < m->y+60 && Player->x+Player->HitBox.right >= m->x-20 && Player->x+Player->HitBox.left <= m->x+40){
				if(Player->x+9 <= m->x+9)
					Player->x = m->x-20-Player->HitBox.right;
				else
					Player->x = m->x+40+Player->HitBox.left;
			}

			m->y = m->starty - Sine[(int)m->count]*60;
			m->count += diff;
			if(m->count > 360) m->count -= 360;
			
			if(OnPlatform){
				Player->y = m->y-15;
				Player->velocity = 0;
				Player->friction = Player->groundfriction;

				mx = ((int)Player->x+9) / 20;
				my = ((int)Player->y+Player->HitBox.top-5) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp  = LD3.Map[mpt];	mp2 = LD3.Map[mpt+MAPWIDTH];
				if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40))
				{
					Splatter((int)Player->x+10, (int)Player->y+10, 50, 0, Player->BloodCol);
					Player->life = 0;
				}					
			}

			break;
		
		case SWING:

			OnPlatform = false;
			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;

			if(m->count3 == 0){
				if(Player->velocity >= 0 && m->setspeed <= lv){
					if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv){
						OnPlatform = true;
						PlatformVelocity = m->setspeed;
					}
				}
			}

			oldx = m->x;
			oldy = m->y;
			
			m->x = m->ox + Cosine[(int)m->count2]*50;
			m->y = m->oy + (float)fabs(Sine[(int)m->count2])*50;
						
			newx = m->x;
			newy = m->y;

			if(OnPlatform){
				Player->y = m->y-15;
				Player->velocity = 0;
				Player->friction = Player->groundfriction;
				Player->x += (newx-oldx);				
			}
			
			m->count2 += (int)m->setspeed*diff;
			m->count = 10;

			if(m->count2 <    0) m->count2 += 360;
			if(m->count2 >= 360) m->count2 -= 360;

			if(m->count3 > 0){
				m->count3 -= diff;
				if(m->count3 <= 0){
					m->count3 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count3 = 500;
				m->visible = false;				
			}
			if(m->count3 < 100 && m->count3 > 0){
				if(((int)m->count3 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
			
			break;

		case SWING2:

			OnPlatform = false;
			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;

			if(m->count3 == 0){
				if(Player->velocity >= 0 && m->setspeed <= lv){
					if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv){
						OnPlatform = true;
						PlatformVelocity = m->setspeed;
					}
				}
			}

			oldx = m->x;
			oldy = m->y;
			
			m->x = m->ox + Cosine[(int)m->count2]*50;
			m->y = m->oy + (float)fabs(Sine[(int)m->count2])*50;
						
			newx = m->x;
			newy = m->y;

			if(OnPlatform){
				Player->y = m->y-15;
				Player->velocity = 0;
				Player->friction = Player->groundfriction;
				Player->x += (newx-oldx);
			}
			
			m->count2 += (int)m->setspeed*diff;
			m->count = 10;

			if(m->count2 <    0) m->count2 += 360;
			if(m->count2 >= 360) m->count2 -= 360;

			if(m->count3 > 0){
				m->count3 -= diff;
				if(m->count3 <= 0){
					m->count3 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count3 = 500;
				m->visible = false;				
			}
			if(m->count3 < 100 && m->count3 > 0){
				if(((int)m->count3 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
			
			break;

		case ROTOPLATFORM:

			oldx = m->x;
			oldy = m->y;

			//m->x = m->ox + vCosine[m->count2]*40;
			//m->y = m->oy - vSine[m->count2]*40;
			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;
						
			if(m->count3 == 0){
				if(m->count2 <= lv){
					if(Player->x+16 >= m->x && Player->x <= m->x + 19 && m->y >= y1-pv && m->y <= y2+pv){
						OnPlatform = true;
						PlatformVelocity = (float)m->count2;
					}
				}
			}
			
			m->x += m->count*diff;
			m->y += m->count2*diff;

			if((m->x-m->ox) >  40){
				m->x = m->ox+40;
				m->count = 0;
				m->count2 = -1;
			}
			if((m->x-m->ox) < -40){
				m->x = m->ox-40;
				m->count = 0;
				m->count2 = 1;
			}
			if((m->y-m->oy) >  40){
				m->y = m->oy+40;
				m->count2 = 0;
				m->count = 1;
			}
			if((m->y-m->oy) < -40){
				m->y = m->oy-40;
				m->count2 = 0;
				m->count = -1;
			}
			/*if((m->count2 >= 315 || m->count2 <= 45) || (m->count2 >= 135 && m->count2 <= 225)){
				if(m->count2 >= 315 || m->count2 <= 45)
					m->x = m->ox + 40;
				else
					m->x = m->ox - 40;
				m->y = m->oy - Sine[m->count2]*40;				
			}
			else{
				m->x = m->ox + Cosine[m->count2]*40;
				if(m->count2 > 45 && m->count2 < 135)
					m->y = m->oy - 40;
				else
					m->y = m->oy + 40;
			}*/

			newx = m->x;
			newy = m->y;
			
			if(OnPlatform){
				Player->y = m->y-15;
				Player->velocity = 0;
				Player->friction = Player->groundfriction;
				Player->x += (newx-oldx);				
			}

			if(m->count3 > 0){
				m->count3 -= diff;
				if(m->count3 <= 0){
					m->count3 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count3 = 500;
				m->visible = false;				
			}
			if(m->count3 < 100 && m->count3 > 0){
				if(((int)m->count3 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
			
			break;

		case PLATFORM_TRAP:

			OnPlatform = false;
			
			pv = Player->velocity*diff;
			lv = m->setspeed*diff;
			
			if(m->count2 == 0){
				if(Player->velocity >= 0 && lv <= PlatformVelocity*diff){
					if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+pv+1){
						OnPlatform = true;
						PlatformVelocity = lv;
					}					
				}
			}			
			
			if(m->count)
			{
				m->y += m->setspeed;
				m->setspeed += 0.09f*diff;				
			}
						
			if(OnPlatform){
				if(m->count == 0){
					m->count = 1;
					//NumResetPlatforms++;
					//Platform[NumResetPlatforms] = Monster[i];
				}				
				Player->y = m->y-15;
				Player->velocity = lv;
				Player->friction = Player->groundfriction;				
			}
			if(m->count2 > 0){
				m->count2 -= diff;
				if(m->count2 <= 0){
					m->count2 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count2 = 500;
				m->visible = false;
				m->x = m->startx;
				m->y = m->starty;
				m->count = 0;
				m->setspeed = 0;
			}
			if(m->count2 < 100 && m->count2 > 0){
				if(((int)m->count2 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}									
			//if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+Player->velocity){
				//Player->y = m->y-15;
				//Player->velocity = 0;
				//Player->friction = Player->groundfriction;
			//}
			break;

		case ANCHOR:

			OnPlatform = false;

			pv = Player->velocity*diff;
			lv = m->setspeed*diff;
			
			if(m->count2 == 0){
				if(m->setspeed <= lv){
					if(Player->x+9 >= m->x-20 && Player->x+9 <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 <= m->y+pv+1){
						OnPlatform = true;
						PlatformVelocity = m->setspeed;
					}					
				}
			}
			
			
			if(m->count)
			{
				m->y += m->setspeed*diff;
				m->setspeed += 0.09f*diff;
			}
						
			if(OnPlatform){
				if(m->count == 0){
					m->count = 1;
					//NumResetPlatforms++;
					//Platform[NumResetPlatforms] = Monster[i];
				}
				Player->y = m->y-15;
				Player->velocity = m->setspeed;
				Player->friction = Player->groundfriction;				
			}
			
			mx = (int)(m->x) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000+MAPWIDTH;
			mp = LD3.Map[mpt-1]; mp2 = LD3.Map[mpt]; mp3 = LD3.Map[mpt+1];
			if(m->count != 0){
				if(LD3.Map[mpt-1-MAPWIDTH] > 40 || LD3.Map[mpt-MAPWIDTH] > 40 || LD3.Map[mpt+1-MAPWIDTH] > 40){
					if(LD3.Map[mpt-1-MAPWIDTH] < 162 && LD3.Map[mpt-MAPWIDTH] < 162 && LD3.Map[mpt+1-MAPWIDTH] < 162){
						m->setspeed = 0;
						m->y = my*20 - 12;
						if(OnPlatform) Player->y = m->y-15;
					}					
				}
				else{
					if(mp > 0 && mp < 40){
						FireProjectile(m->x-20, m->y, rand()%360, 2+rand()%2*0.5f, FALLING_GROUND, &NullEntity, mp);	
						Projectile[NumProjectiles].spritenum = (float)mp;
						LD3.Map[mpt-1] = 0;
						m->setspeed /= 2;
					}
					if(mp2 > 0 && mp2 < 40){
						FireProjectile(m->x, m->y, rand()%360, 2+rand()%2*0.5f, FALLING_GROUND, &NullEntity, mp2);	
						Projectile[NumProjectiles].spritenum = (float)mp2;
						LD3.Map[mpt] = 0;
						m->setspeed /= 2;							
					}
					if(mp3 > 0 && mp3 < 40){
						FireProjectile(m->x+20, m->y, rand()%360, 2+rand()%2*0.5f, FALLING_GROUND, &NullEntity, mp3);	
						Projectile[NumProjectiles].spritenum = (float)mp3;
						LD3.Map[mpt+1] = 0;
						m->setspeed /= 2;
					}
				}
				if(LD3.Map[mpt-MAPWIDTH] == miTreadmillLeft || LD3.Map[mpt-MAPWIDTH+1] == miTreadmillLeft || LD3.Map[mpt-MAPWIDTH+2] == miTreadmillLeft){
					m->x -= 0.8f*diff;
					if(OnPlatform){Player->x -= 0.8f*diff; Player->inertialx = -0.8f*diff;}
				}
				if(LD3.Map[mpt-MAPWIDTH] == miTreadmillRight || LD3.Map[mpt-MAPWIDTH+1] == miTreadmillRight || LD3.Map[mpt-MAPWIDTH+2] == miTreadmillRight){
					m->x += 0.8f*diff;
					if(OnPlatform){Player->x += 0.8f*diff; Player->inertialx = 0.8f*diff;}
				}


			}

			if(m->count2 > 0){
				m->count2 -= diff;
				if(m->count2 <= 0){
					m->count2 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				m->life = 20;
				m->count2 = 500;
				m->visible = false;
				m->x = m->startx;
				m->y = m->starty;
				m->count = 0;
				m->setspeed = 0;
			}
			if(m->count2 < 100 && m->count2 > 0){
				if(((int)m->count2 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}	
			//if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+Player->velocity){
				//Player->y = m->y-15;
				//Player->velocity = 0;
				//Player->friction = Player->groundfriction;
			//}
			//Marker.x = 3400;
			//Marker.y = 1200;
			break;

		case CHOPPER:

			mx = ((int)m->x+9) / 20;
			my = ((int)m->y+m->HitBox.top-5) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp  = LD3.Map[mpt];	mp2 = LD3.Map[mpt+MAPWIDTH];
			if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40))
			{
				m->x = m->ox;
				m->y = m->oy;
			}

			if(m->lani > m->lstart) m->lani -= 0.2f*diff;
			if(m->lani < m->lstart) m->lani = m->lstart;

			m->ox = m->x;
			m->oy = m->y;
			
			m->uani += 0.5f*diff;
			if(m->uani >= m->uend+1) m->uani = m->ustart;

			MACfuel -= 0.01f*diff;
			if(MACfuel <= 0){
				CreateExplosion(m->x+9, m->y+9);
				CreateExplosion(m->x-20, m->y+9);
				CreateExplosion(m->x+39, m->y+9);
				CreateExplosion(m->x+9, m->y-20);
				CreateExplosion(m->x+9, m->y+39);
				MACunit->life = 0;
				ControllingMAC = false;
				FocusEntity = Player;
				FSOUND_StopSound(5);
			}

			break;

		case BOULDER:

			mx = (int)(m->x+10) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt];

			m->x += m->setspeed*diff;
			if(mp >= 40){
				m->velocity = -m->velocity/2;
				m->y += m->velocity*diff;
				if(fabs(m->setspeed) < 2){
					if(m->lstart == 77) m->setspeed = 2; else m->setspeed = -2;
				}
				m->lani += 0.2f;
				if(m->lani >= 79) m->lani = 77;
			}
			else{
				m->y += m->velocity*diff;
				m->velocity += 0.09f*diff;
			}
			if(m->lstart == 77)	m->setspeed += 0.001f; else m->setspeed -= 0.001f;

			if(m->x+10 >= Player->x && m->x+10 <= Player->x+19 && m->y+10 >= Player->y && m->y+10 <= Player->y+19){
				Player->life -= 20;
				Splatter((int)Player->x+10,(int)Player->y+10,20,0,Player->BloodCol);
			}

			break;

		case WEAK_BOULDER:

			mx = (int)(m->x+10) / 20;
			my = (int)(m->y+19-4) / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt];

			m->x += m->setspeed*diff;
			if(mp >= 40){
				m->velocity = -m->velocity/2;
				m->y += m->velocity*diff;
				if(fabs(m->setspeed) < 2){
					if(m->lstart == 77) m->setspeed = 2; else m->setspeed = -2;
				}
				m->lani += 0.2f;
				if(m->lani >= 79) m->lani = 77;
			}
			else{
				m->y += m->velocity*diff;
				m->velocity += 0.09f*diff;
			}
			if(m->lstart == 77)	m->setspeed += 0.001f; else m->setspeed -= 0.001f;

			if(m->x+10 >= Player->x && m->x+10 <= Player->x+19 && m->y+10 >= Player->y && m->y+10 <= Player->y+19){
				Player->life -= 10;
				Splatter((int)Player->x+10,(int)Player->y+10,20,0,Player->BloodCol);
				m->life = 0;
			}

			break;

		case GO_CART:

			//if(DidJump) m->velocity = Player->velocity;//ShowPlayer = true;

			if(m->count){

				mx = (int)(m->x+10) / 20;
				my = (int)(m->y+19-7) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+MAPWIDTH];

				m->x += m->setspeed*diff;
				if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40)){
					m->setspeed = 0;
					m->life = 0;
					Player->life = 0;
					Splatter((int)Player->x+10,(int)Player->y+10,100,0,Player->BloodCol);
					ShowPlayer = true;
					xShiftMin = xRes/2-10;
					xShiftMax = xRes/2+10;
					m->count = 0;
				}
				/*if(mp2 >= 40 && DidJump == false){
					m->velocity = -m->velocity/2;
					m->y += m->velocity;
					if(m->setspeed < 2) m->setspeed = 2;					
				}
				else{
					m->y += m->velocity;
					m->velocity += 0.09f;
				}*/
				m->y = Player->y;
				if(ShowPlayer == false){
					m->lani = 81;
					Player->x = m->x;
					xShiftMin = 0;
					xShiftMax = 60;
					//Player->y = m->y-4;
					//if(Player->x-xShift < xShiftMin && xShift > 0) xShift -= m->setspeed;					
				}
				else
					m->lani = 80;
				m->setspeed += 0.01f*diff;
				if(m->setspeed >= 4) m->setspeed = 4;			

			}

			if(m->count2 > 0){
				m->count2 -= diff;
				if(m->count2 <= 0){
					m->count2 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0 || m->y > 2000){
				ShowPlayer = true;
				xShiftMin = xRes/2-10;
				xShiftMax = xRes/2+10;
				m->life = 20;
				m->count2 = 500;
				m->visible = false;
				m->x = m->startx;
				m->y = m->starty;
				m->count = 0;
				m->setspeed = 0;
			}
			if(m->count2 < 100 && m->count2 > 0){
				if(((int)m->count2 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}

			if(m->x+10 >= Player->x && m->x+10 <= Player->x+19 && m->y+10 >= Player->y && m->y+10 <= Player->y+19 && m->count == 0 && m->count2 == 0){
				m->count = 1;
				ShowPlayer = false;
			}

			break;

		case SNOWMOBILE:

			if(m->count){

				mx = (int)(m->x+10) / 20;
				my = (int)(m->y+19-7) / 20;
				mpt = mx+(my<<MAPXLSHIFT)+55000;
				mp = LD3.Map[mpt]; mp2 = LD3.Map[mpt+MAPWIDTH];

				//m->x += m->setspeed;
				m->x -= Cosine[int(((m->lani-20)*18)+90)]*4;//fabs(m->setspeed);
				if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40)){
					/*m->life = 0;
					Player->life = 0;
					Splatter((int)Player->x+10,(int)Player->y+10,100,0,Player->BloodCol);
					ShowPlayer = true;*/					
				}
				
				m->y = Player->y;
				if(ShowPlayer == false){
					//m->lani = 81;
					/*if(m->setspeed > 0){
						m->lani = 25;//+(m->setspeed - Player->speed);
					}
					else{
						m->lani = 35;//+(m->setspeed - Player->speed);
					}*/
					if(Player->speed > 0 && m->lani != 25){
						m->lani += fabs(Player->speed/4);//0.2f;
						if(m->lani > 39) m->lani = 20;
					}
					if(Player->speed < 0 && m->lani != 35){
						m->lani -= fabs(Player->speed/4);//0.2f;
						if(m->lani < 20) m->lani = 39;
					}
					if(Player->speed == 0){
						if((int)m->lani >= 20 && (int)m->lani < 25) m->lani += 0.2f;
						if((int)m->lani >  25 && (int)m->lani < 30) m->lani -= 0.2f;
						if((int)m->lani >= 30 && (int)m->lani < 35) m->lani += 0.2f;
						if((int)m->lani >  35 && (int)m->lani < 40) m->lani -= 0.2f;
					}
					Player->x = m->x;					
				}
				else
					m->lani = 80;
				//m->setspeed += 0.01f;
				//if(m->setspeed >= 4) m->setspeed = 4;
				m->setspeed += Player->speed/10;
				if(m->setspeed >=  4) m->setspeed =  4;
				if(m->setspeed <= -4) m->setspeed = -4;

				if(m->count2 > 0){
					m->count2--;
					if(m->count2 <= 0){
						m->count2 = 0;
						m->visible = true;
					}
				}
				if(m->life <= 0 || m->y > 2000){
					ShowPlayer = true;
					xShiftMin = xRes/2-10;
					xShiftMax = xRes/2+10;
					m->life = 20;
					m->count2 = 500;
					m->visible = false;
					m->x = m->startx;
					m->y = m->starty;
					m->count = 0;
					m->setspeed = 0;
				}
				if(m->count2 < 100 && m->count2 > 0){
					if(((int)m->count2 & 15) < 3)
						m->visible = true;
					else
						m->visible = false;
				}
			}					

			if(m->x+10 >= Player->x && m->x+10 <= Player->x+19 && m->y+10 >= Player->y && m->y+10 <= Player->y+19 && m->count == 0){
				m->count = 1;
				ShowPlayer = false;
				m->spriteset = LARRY_SPRITE_SET;
				m->lstart = 20;
				m->lend = 39;
				m->lani = 25;
			}		

			if((m->life <= 0 || m->y > 2000) && ShowPlayer == false){
				ShowPlayer = true;
				xShiftMin = xRes/2-10;
				xShiftMax = xRes/2+10;
				m->life = 20;
				m->visible = true;
				m->x = m->startx;
				m->y = m->starty;
				m->count = 0;
				m->setspeed = 0;
			}			
			
			break;

		case BOSS1:

			if(m->y-yShift > -20 && m->y-yShift < yRes && m->x-xShift > -20 && m->x-xShift < xRes && LockShift == false){
				LockShift = true;
				LockXShift = BossXShift;
				LockYShift = BossYShift;
				Boss = m;
				Boss->x += 140;
				m->count = 2000;
				m->count2 = 80;	
			}
			if(xShift == BossXShift && yShift == BossYShift){
				m->visible = true;
				m->Fall(&LD3.Map[0], MAPWIDTH, diff);
				m->oy = m->y;

				if(m->y-yShift >= -200 && m->y-yShift <= yRes+200){
					if(m->x-xShift+10 >= -200 && m->x-xShift+10 <= xRes+200){
						if(LoadedBossSong == false){
							FMUSIC_StopSong(song1);
							//song1   = FMUSIC_LoadSong("music/boss.it");
							//song1   = FMUSIC_LoadSong("music/laststand.it");
							FMUSIC_PlaySong(mscBoss);
							LoadedBossSong = true;
						}						
					}
				}
				/*if(m->life <= 0){
					EndOfLevel		= true;
					EndOfLevelTimer = 1000;
					FMUSIC_StopSong(song1);
					FMUSIC_PlaySong(mscLevelComplete);
				}*/

				if(m->life > 1){
					if(m->setspeed < 0)
						m->setspeed += 0.02f;
					else if(m->setspeed > 0)
						m->setspeed -= 0.02f;
					if(fabs(m->setspeed) <= 0.1f) m->setspeed = 0;
					m->x += m->setspeed;

					if(fabs(m->setspeed) > 0.4f){
						m->uani = float(5+(((int)m->count & 7)/4)*10);
					}

					m->count--;
					if(m->count <= 0)
					{
						m->count = 140;
						m->setspeed += 1.3f;
						
						if(m->x > m->startx + 100){
							FireProjectile(m->x-20, m->y+20, 180, 3, SPIKEBALL, &Monster[i], 11);
							FireProjectile(m->x-20, m->y+20, 160, 3, SPIKEBALL, &Monster[i], 11);
							FireProjectile(m->x-20, m->y+20, 150, 3, SPIKEBALL, &Monster[i], 11);
							m->count = 2000;
							m->count2 = 50;						
						}
						else{					
							int ang = 180-rand()%40;
							FireProjectile(m->x-20, m->y+20, ang, 3, SPIKEBALL, &Monster[i], 11);
						}					
					}
					if(m->count > 30 && m->count < 70) m->setspeed = -0.5f;
					
					
					if(m->count2 > 0){
						m->count2--;
						m->setspeed = -2;
						if(m->count2 <= 0){
							m->count = 10;
							m->count2 = 0;
						}
					}

					if(Player->x+9 >= m->x && Player->x+9 <= m->x + 60 && Player->y+9 >= m->y && Player->y+9 < m->y+59){
						//Player->life -= 1;
						//Splatter((int)Player->x+10,(int)Player->y+10,10,0,Player->BloodCol);
						Player->x = m->x-9;
					}
					if(Player->life <= 0) m->life = 100;
				}
				else{
					int rx = rand()%80;
					int ry = rand()%40;					
					m->count--;
					m->life = 1;
					if(m->count <= 0){						
						CreateExplosion(int(m->x+rx), int(m->y+ry));
						m->life = 1;
						if(m->count2 <= 0) m->count2 = 40;
						m->count = m->count2;
						m->count2--;
						if(m->count2 <= 0){
							for(int t = 0; t <= 13; t++){
								int rx = rand()%80;
								int ry = rand()%50;
								CreateExplosion(int(m->x+rx), int(m->y+ry));
								m->life = 0;
								EndOfLevel		= true;
								EndOfLevelTimer = 1000;
								//FMUSIC_StopSong(song1);
								//FMUSIC_PlaySong(mscLevelComplete);
							}					
						}
					}
				}
			}
			break;

		case BOSS2:

			if(m->y-yShift > -20 && m->y-yShift < yRes && m->x-xShift > -20 && m->x-xShift < xRes && m->life > 0){
				LockShift = true;
				LockXShift = BossXShift;
				LockYShift = BossYShift;
			}
			if(xShift == BossXShift && yShift == BossYShift){
				m->visible = true;
				if(m->y-yShift >= -200 && m->y-yShift <= yRes+200){
					if(m->x-xShift+10 >= -200 && m->x-xShift+10 <= xRes+200){
						if(LoadedBossSong == false){
							FMUSIC_StopSong(song1);
							//song1   = FMUSIC_LoadSong("music/boss.it");						
							FMUSIC_PlaySong(mscBoss);
							LoadedBossSong = true;
						}
						LD3.DrawBox(&WinApp, 267, 2, int(267+m->life), 6, int(36+(m->life/10)), true); 
						if(Player->life < 100) LD3.DrawBox(&WinApp, int(267+m->life), 2, 317, 6, 34, true);				
					}
				}
				if(m->life <= 0){
					LockShift = 0;
				}

				m->count--;
				if(m->y-20 <= Player->y+10 && m->y+20 >= Player->y+10) m->count -= 2;
				if(m->count <= 0)
				{
					FireProjectile(m->x, m->y-10, 180, 3, YELLOW_FIRE, &Monster[i], 11);
					FireProjectile(m->x+20, m->y-10, 0, 3, YELLOW_FIRE, &Monster[i], 11);				
					m->count = 60;				
				}			

				m->count2++;
				if(m->count2 >= 360) m->count2 = 0;

				m->x = m->ox + Cosine[(int)m->count2]*100;
				m->y = m->oy - Sine[(int)m->count2]*100;
				
				if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+19){
					Player->life -= 1;
					Splatter((int)Player->x+10,(int)Player->y+10,10,0,Player->BloodCol);
				}
				if(Player->life <= 0) m->life = 50;
			}
			break;

		case BOSS3:

			if(m->y-yShift > -20 && m->y-yShift < yRes && m->x-xShift > -20 && m->x-xShift < xRes && LockShift == false && Player->life > 0){
				LockShift = true;
				BossXShift += 2900;
				LockXShift = BossXShift;
				LockYShift = BossYShift;
				Boss = m;
				Boss->x += 2900;
				m->count = 100;
				m->count2 = 20;	
				m->visible = true;
			}
			if(m->visible == true){
								
				if(LoadedBossSong == false){
					FMUSIC_StopSong(song1);
					//song1   = FMUSIC_LoadSong("music/boss.it");
					//song1   = FMUSIC_LoadSong("music/ld3.it");
					//song1   = FMUSIC_LoadSong("music/laststand.it");
					FMUSIC_PlaySong(mscBoss);
					LoadedBossSong = true;
				}						
				/*if(m->life <= 0){
					EndOfLevel		= true;
					EndOfLevelTimer = 1000;
					FMUSIC_StopSong(song1);
					FMUSIC_PlaySong(mscLevelComplete);
				}*/

				if(Boss->x-xShift < 350){
				if(m->life > 1){
					//- life

					/*if(Boss->x-xShift > 150){
						Boss->x -= 2;
						Boss->count3 ++;
						if(Boss->count3 > 10) Boss->count3 = 0;
					}*/

					if(Boss->count3 <= 0){
						Boss->count3 = 200 + rand()%100;
						Boss->uani = 14;
					}
					else{
						Boss->count3 -= 1;

						if(Boss->life >= 50){
							LockXShift = BossXShift + 5-rand()%10;
							LockYShift = BossYShift + 5-rand()%10;
						}
						else{
							LockXShift = BossXShift + 10-rand()%20;
							LockYShift = BossYShift + 10-rand()%20;
						}

						if((Boss->life >= 50 && ((int)Boss->count3 & 63) == 0) || (Boss->life < 50 && ((int)Boss->count3 & 31) == 0)){
							NumMonsters++;
							nm = NumMonsters;

							Monster[nm].Init();
							Monster[nm].x		= Boss->x+(100-rand()%200);
							Monster[nm].y		= yShift-40;
							Monster[nm].ox		= Monster[nm].x;
							Monster[nm].oy		= Monster[nm].y;
							Monster[nm].id		= WEAK_BOULDER;
							Monster[nm].spriteset= -1;
							Monster[nm].lani	= 77;
							Monster[nm].lstart	= 77;
							Monster[nm].lend	= 78;
							Monster[nm].life	= 200;
							Monster[nm].dontfall= false;
							Monster[nm].xFlip	= false;
							Monster[nm].count	= 0;
							Monster[nm].setspeed= 0;
							Monster[nm].velocity= 0;
							Monster[nm].BloodCol= 132;
							Monster[nm].HitBox.left		= 0;
							Monster[nm].HitBox.right	= 19;
							Monster[nm].HitBox.top		= 0;
							Monster[nm].HitBox.bottom	= 19;

							if(rand()%2 == 1){
								Monster[nm].lani	= 78;
								Monster[nm].lstart	= 78;
							}

							//if(Player->x-LockXShift <= 0){
							//	if(Player->x-LockXShift < -20) Player->life = 0;
							//
							//	Player->x++;
							//}
							
						}
					}					
				}
				else{
					//- death
					Boss->life = 1;
					MusicVolume = 0;
					int rx = rand()%80;
					int ry = rand()%40;
					m->count--;
					if(m->count <= 0){
						CreateExplosion(int(m->x+rx), int(m->y+ry));
						m->life = 1;
						if(m->count2 <= 0) m->count2 = 40;
						m->count = m->count2;
						m->count2--;
						if(m->count2 <= 0){
							for(int t = 0; t <= 13; t++){
								int rx = rand()%80;
								int ry = rand()%50;
								CreateExplosion(int(m->x+rx), int(m->y+ry));
								m->life = 0;
								LockShift = false;
								EndOfLevel		= true;
								EndOfLevelTimer = 1000;
							}					
						}
					}					
				}
				}
				else{
					if(rand()%70 == 10){
						NumMonsters++;
						nm = NumMonsters;

						Monster[nm].Init();
						Monster[nm].x		= Player->x+(100-rand()%200);
						Monster[nm].y		= yShift-40;
						Monster[nm].ox		= Monster[nm].x;
						Monster[nm].oy		= Monster[nm].y;
						Monster[nm].id		= WEAK_BOULDER;
						Monster[nm].spriteset= -1;
						Monster[nm].lani	= 77;
						Monster[nm].lstart	= 77;
						Monster[nm].lend	= 78;
						Monster[nm].life	= 200;
						Monster[nm].dontfall= false;
						Monster[nm].xFlip	= false;
						Monster[nm].count	= 0;
						Monster[nm].setspeed= 0;
						Monster[nm].velocity= 0;
						Monster[nm].BloodCol= 132;
						Monster[nm].HitBox.left		= 0;
						Monster[nm].HitBox.right	= 19;
						Monster[nm].HitBox.top		= 0;
						Monster[nm].HitBox.bottom	= 19;

						if(rand()%2 == 1){
							Monster[nm].lani	= 78;
							Monster[nm].lstart	= 78;
						}
					}
				}
				if(Player->life <= 0){
					Boss->x = Boss->startx;
					Boss->y = Boss->starty-20;
					Boss->life = 100;
					BossXShift -= 2900;
					Boss->visible = false;
				}					

			}
			break;
			
		case BOSS4:

			m->Move(diff);
			m->CheckWallHit(&LD3.Map[0], MAPWIDTH);
			m->Fall(&LD3.Map[0], MAPWIDTH, diff);
			if(m->y-yShift > -20 && m->y-yShift < yRes && m->x-xShift > -20 && m->x-xShift < xRes && LockShift == false && ProScript == false){
				LockShift = true;
				LockXShift = BossXShift;
				LockYShift = BossYShift;
				Boss = m;
				//Boss->x += 140;
				//m->count = 2000;
				//m->count2 = 80;
				m->count = 77;
				m->count2 = 0;
				//m->setspeed = 1.5f;
			}
			if(xShift == LockXShift && yShift == LockYShift && LockShift){
				m->visible = true;
								
				if(m->y-yShift >= -200 && m->y-yShift <= yRes+200){
					if(m->x-xShift+10 >= -200 && m->x-xShift+10 <= xRes+200){
						if(LoadedBossSong == false){
							FMUSIC_StopSong(song1);
							//LD3.LoadSprite(&WinApp, "gfx/badmark.put", BOSS_SPRITE_SET, 0);
							//song1   = FMUSIC_LoadSong("music/boss.it");
							//song1   = FMUSIC_LoadSong("music/laststand.it");
							//song1   = FMUSIC_LoadSong("music/badmark.it");
							//song1   = FMUSIC_LoadSong("music/brainmaster.it");
							FMUSIC_PlaySong(mscBoss);
							Boss->HitBox.top = 1;
							Boss->HitBox.bottom = 19;
							Boss->HitBox.left = 1;
							Boss->HitBox.right = 17;
							Boss->lani = 0;
							Boss->lstart = 0;
							Boss->lend = 5;
							Boss->uani = 6;
							LoadedBossSong = true;
							if(m->x == m->startx)
								m->count = 64;//int(77 - fabs(m->x-m->startx)*0.5f);
							else
								m->count = 200;
							m->setspeed = -1.5f;
						}						
					}
				}
				/*if(m->life <= 0){
					EndOfLevel		= true;
					EndOfLevelTimer = 1000;
					FMUSIC_StopSong(song1);
					FMUSIC_PlaySong(mscLevelComplete);
				}*/

				if(m->life > 0){
					
					if(m->count2 == 0) m->count--;
					if(m->count <= 0){
						m->count2 = 50;
						m->count = 180;
						m->setspeed = 0;
						if(m->xFlip)
							m->xFlip = false;
						else
							m->xFlip = true;
					}
											
					if(m->count2 > 0){
						m->count2--;
						if(m->count2 <= 0){
							if(m->xFlip)
								m->setspeed = -1.5f;
							else
								m->setspeed = 1.5f;
							
							rnd = rand()%20;

							if(rnd <= 18){								
								//m->setspeed = 0;
								m->uani = 10.0f;
								rnd = rand()%2;
								if(rnd == 1) m->velocity = -3.0f;
							}
						}
					}

					if(m->uani >= 10 && m->uani < 17){
						m->uani += 0.2f;						
					}
					else if(m->uani >= 17 && m->uani < 30){
						m->uani += 0.2f;
						if(m->uani >= 20.0f && m->uani <= 20.1f){
							if(m->xFlip)
								FireProjectile(m->x, m->y, 180, 3, YELLOW_FIRE, Boss, 0);
							else
								FireProjectile(m->x, m->y, 0, 3, YELLOW_FIRE, Boss, 0);								
						}
						if(m->uani >= 24){
							rnd = rand()%20;
							if(rnd >= 19)
								m->uani = 6.0f;
							else
								m->uani = 17.0f;
						}
					}
					if(m->uani >= 30){
						m->uani += 0.2f;
						if(m->uani >= 40) m->uani = 6.0f;
					}

					if(m->aiState == aiANGRY){
						rnd = rand()%5;
						if(rnd == 1 && m->uani < 30) m->uani = 30;
						m->aiState = aiCALM;
					}
					
					if(Player->life <= 0){
						m->life = 100;
						m->x = m->startx+64;
						m->y = m->starty;
						m->count2 = 0;
						m->count = 0;
						m->xFlip = 1;
					}
				}
				else{
					EndOfLevel		= true;
					EndOfLevelTimer = 1000;
					//FMUSIC_StopSong(song1);
					//FMUSIC_PlaySong(mscLevelComplete);
				}
			}
			break;
			
		case LASTBOSS:

			if(m->y-yShift > -20 && m->y-yShift < yRes && m->x-xShift > -20 && m->x-xShift < xRes && LockShift == false){
				LockShift = true;
				LockXShift = BossXShift;
				LockYShift = BossYShift;
				Boss = m;
				Boss->x += 550;
				//Boss->life = 5;
				//m->count = 2000;
				//m->count2 = 80;				
				//m->setspeed = 1.5f;
				if(LoadedBossSong == false){
					LD3.LoadSprite(&WinApp, "gfx/lastbss1.put", BOSS_SPRITE_SET, 0);
					LD3.LoadSprite(&WinApp, "gfx/lastbss2.put", BOSS_SPRITE_SET, 80);
					FMUSIC_StopSong(song1);
					FMUSIC_PlaySong(mscBoss);
					Boss->HitBox.top = -45;
					Boss->HitBox.bottom = 90;
					Boss->HitBox.left = -120;
					Boss->HitBox.right = 140;
					Boss->lani = 0;
					Boss->lstart = 0;
					Boss->lend = 0;
					Boss->uani = 0;
					LoadedBossSong = true;
					m->setspeed = 2;
				}
			}
			if(xShift == LockXShift && yShift == LockYShift && LockShift){
				m->visible = true;
				xSkyShift += 4;
				if(xSkyShift > 32767) xSkyShift = 0;

				if(m->life > 1){
					
					if(m->count3 > 0){
					
						m->count --;
						m->x += m->setspeed;
						if(m->count <= 0){
						
							if(m->setspeed < 0){
								m->setspeed = 5;
								m->xFlip = true;							
							}
							else{
								m->setspeed = -5;
								m->xFlip = false;
							}						

							m->y = m->starty - 200;
							m->count= 240;
							m->count3--;
							if(m->count3 <= 0) m->setspeed = 2;
						}
						else{
							m->count2--;
							if(m->count3 == 1 && m->x-xShift > 260 && m->x-xShift < 266) m->count2 = 0;
							if(m->count2 <= 0){
								if(m->x-xShift > 20 && m->x-xShift < 280){
									if(m->life > 50)
										m->count2 = 15+rand()%10;
									else if(m->life > 20)
										m->count2 = 5+rand()%15;
									else
										m->count2 = 5+rand()%10;
									//FireProjectile(m->x, m->y+100, 270, 0, BOMB, m, 0);
									if(m->count3 == 1 && m->x-xShift > 260){
										for(n = 1; n <= 3; n++){
											NumMonsters++;
											nm = NumMonsters;
											Monster[nm].Init();
											Monster[nm].id			= DUD;
											Monster[nm].startx		= m->x;
											Monster[nm].starty		= m->y+100;
											SetDefaultValue(&Monster[nm]);
										}
									}
									else if(m->count3 != 1 || (m->x-xShift < 220)){
										NumMonsters++;
										nm = NumMonsters;
										Monster[nm].Init();
										Monster[nm].id			= BOMB;
										Monster[nm].startx		= m->x;
										Monster[nm].starty		= m->y+100;
										SetDefaultValue(&Monster[nm]);
									}
								}
							}
						}
					}
					else if(m->count3 == 0){
					
						if(m->x < m->startx-300) m->x += 2;
						else if(m->x > m->startx+200) m->x -= 2;
						else{
						
							m->y += m->setspeed;

							if(m->y > m->starty-50) m->setspeed = -2;
							if(m->y < m->starty - 200){
								m->setspeed = 2;
								rnd = rand()%2;
								if(rnd == 1){
									m->count3 = -1;
									m->count = 0;
								}
							}

							m->count2--;
							if(m->count2 <= 0){
								if(m->life > 50)
									m->count2 = 30+rand()%10;
								else
									m->count2 = 20+rand()%10;
								if(m->xFlip)
									FireProjectile(m->x+80, m->y+60, 0, 4, YELLOW_FIRE, m, 4);
								else
									FireProjectile(m->x-80, m->y+60, 180, 4, YELLOW_FIRE, m, 4);
							}					
						}				
					
					}
					else if(m->count3 < 0){
					
						if(m->x < m->startx+550)
							m->x += 5;
						else{
							if(m->life > 50)
								m->count3 = 5;
							else
								m->count3 = 3;
						}

					
					}
					
					if(Player->life <= 0){
						m->life = 100;
					}
				}
				else{					
					int rx = -rand()%100;
					int ry = 80-rand()%80;
					m->count--;
					m->life = 1;
					if(m->xFlip) m->x += 0.2f; else m->x -= 0.2f;
					m->y += 0.2f;
					if(m->count <= 0){
						CreateExplosion(int(m->x+rx), int(m->y+ry));						
						m->life = 1;
						if(m->count2 <= 0) m->count2 = 40;
						m->count = 10;
						m->count2--;
						if(m->y > m->starty+100){
							FMUSIC_StopSong(mscBoss);
							FadeToWhite = true;
							Fading = true;
							m->life = 0;
							LastScript = true;
							NumMonsters++;
							nm = NumMonsters;
							Monster[nm].Init();
							Monster[nm].id			= DUD;
							Monster[nm].startx		= 290;
							Monster[nm].starty		= 320;
							SetDefaultValue(&Monster[nm]);
							FMUSIC_StopSong(song1);
							FMUSIC_PlaySong(mscLevelComplete);
							LockShift = false;
						}
					}
				}				
			}
			break;
		}

		if(Monster[i].id == PLATFORM_Lr || Monster[i].id == PLATFORM_lR){

			m->x += m->setspeed*diff;
			float pv, lv;

			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;
			
			if(m->count2 == 0){
				if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv && lv >= 0){
					Player->x += m->setspeed*diff;
					Player->y = m->y-15;
					Player->velocity = 0;
					Player->friction = Player->groundfriction;
				}
			}
									
			//if(m->count <= 0)
			if(m->id == PLATFORM_Lr){
				if((m->setspeed < 0 && m->x < m->count3) || (m->setspeed > 0 && m->x > m->startx))
				{
					if(m->setspeed < 0)
						m->x = m->count3;
					else
						m->x = m->startx;
					
					m->oldspeed = m->setspeed;
					m->setspeed = 0;
					m->count = 20;				
				}				
				if(m->setspeed == 0 && m->count == 0) m->setspeed = -m->oldspeed;
			}
			else{
				if((m->setspeed < 0 && m->x < m->startx) || (m->setspeed > 0 && m->x > m->count3))
				{
					if(m->setspeed < 0)
						m->x = m->startx;
					else
						m->x = m->count3;
					
					m->oldspeed = m->setspeed;
					m->setspeed = 0;
					m->count = 20;				
				}
				if(m->setspeed == 0 && m->count == 0) m->setspeed = -m->oldspeed;
			}

			m->count -= diff;
			if(m->count <= 0) m->count = 0;
			
			if(m->count2 == 0){
				if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+Player->velocity+1 && PlatformVelocity >= 0){
					Player->y = m->y-15;
					Player->velocity = 0;
					Player->friction = Player->groundfriction;
				}
			}
			if(m->count2 > 0){
				m->count2 -= diff;
				if(m->count2 <= 0){
					m->count2 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0){
				m->life = 20;
				m->count2 = 500;
				m->visible = false;
			}
			if(m->count2 < 100 && m->count2 > 0){
				if(((int)m->count2 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
		}
		else if(Monster[i].id == PLATFORM_uD || Monster[i].id == PLATFORM_Ud){

			OnPlatform = false;
			
			pv = Player->velocity*diff;
			lv = PlatformVelocity*diff;

			if(m->count2 == 0){
				if(m->setspeed <= lv){
					if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && m->y >= y1-pv && m->y <= y2+pv){
						OnPlatform = true;
						PlatformVelocity = m->setspeed;
					}
				}
			}
		
			m->y += m->setspeed*diff;
												
			//if(m->count <= 0)
			if(m->id == PLATFORM_Ud){
				if((m->setspeed < 0 && m->y < m->count3) || (m->setspeed > 0 && m->y > m->starty))
				{
					if(m->setspeed < 0)
						m->y = m->count3;
					else
						m->y = m->starty;
					
					m->oldspeed = m->setspeed;
					m->setspeed = 0;
					m->count = 20;				
				}				
				if(m->setspeed == 0 && m->count == 0) m->setspeed = -m->oldspeed;
			}
			else{
				if((m->setspeed < 0 && m->y < m->starty) || (m->setspeed > 0 && m->y > m->count3))
				{
					if(m->setspeed < 0)
						m->y = m->starty;
					else
						m->y = m->count3;
					
					m->oldspeed = m->setspeed;
					m->setspeed = 0;
					m->count = 20;				
				}
				if(m->setspeed == 0 && m->count == 0) m->setspeed = -m->oldspeed;
			}

			if(m->count2 == 0){
				if(OnPlatform){
					Player->y = m->y-15;
					Player->velocity = 0;
					Player->friction = Player->groundfriction;
				}
			}			

			m->count -= diff;
			if(m->count <= 0) m->count = 0;

			if(m->count2 > 0){
				m->count2 -= diff;
				if(m->count2 <= 0){
					m->count2 = 0;
					m->visible = true;
				}
			}
			if(m->life <= 0){
				m->life = 20;
				m->count2 = 500;
				m->visible = false;
			}
			if(m->count2 < 100 && m->count2 > 0){
				if(((int)m->count2 & 15) < 3)
					m->visible = true;
				else
					m->visible = false;
			}
			
			//if(Player->x+16 >= m->x-20 && Player->x <= m->x + 40 && Player->y+15 >= m->y && Player->y+15 < m->y+Player->velocity){
				//Player->y = m->y-15;
				//Player->velocity = 0;
				//Player->friction = Player->groundfriction;
			//}			
		}
		else if(Monster[i].id == SPIKEBALL1 || Monster[i].id == SPIKEBALL2){
			
			m->count2 += (int)m->setspeed*diff;
			if(m->count2 <    0) m->count2 += 360;
			if(m->count2 >= 360) m->count2 -= 360;
			
			m->x = m->ox + Cosine[(int)m->count2]*40;
			m->y = m->oy - Sine[(int)m->count2]*40;
			
			m->count = 10;
			
			if((m->x+10 >= Player->x && m->x+10 <= Player->x+19) && (m->y+10 >= Player->y && m->y+10 <= Player->y+19)){
				Player->life -= 10;
				Splatter((int)m->x+10, (int)m->y+10, 20, 0, Player->BloodCol);				
			}	
		}
	}
		

	int i = 1;
	while(i <= NumMonsters)
	{
		m = &Monster[i];
		if(m->life <= 0 || m->y > 2000){
			if(m->ScoreValue == 0){
				//NumResetPlatforms++;
				//Platform[NumResetPlatforms] = Monster[i];
				//Platform[NumResetPlatforms].life = 100;
				if(Monster[i].id == BLOB_MINE) NumKills++;
			}
			else{
				Score += Monster[i].ScoreValue;
				NumKills++;
			}
			for(int n = i; n <= NumMonsters-1; n++){				 
				Monster[n] = Monster[n+1];
				if(Monster[n].id == CHOPPER){
					if(Monster[n].life > 0){
						MACunit = &Monster[n];
						FocusEntity = MACunit;
					}
				}
			}
			NumMonsters--;
		}
		else
			i++;
	}

	if(LastScript){
		LD3.LoadSprite(&WinApp, "gfx/xahn.put", XAHN_SPRITE_SET, 40);
		LoadScript("scripts/scm14sb.l3s");
	}
}

//=================================================================================================
//= void DrawMonsters()
//= This function draws the monsters
//=================================================================================================
void DrawMonsters()
{
	ENTITY	*m;
	int		n, nx, ny;
	float	PlatformVelocity = 10000;
	int		bx, by;     
	int		ex, ey;
	bool	visible = false;
	
	//- Draw the monsters	
	for(int i = 1; i <= NumMonsters; i++){
		m = &Monster[i];
		
		ex = int(m->x-xShift);
		ey = int(m->y-yShift);			
		visible = false;

		if(m->visible && ex >= -40 && ex <= xRes+40 && ey >= -80 && ey <= yRes+40) visible = true;
		if(visible) LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->lani, m->xFlip);			
		
		if(m->visible && visible){
		switch(Monster[i].id)
		{
		case FLETCHER:			
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);
			if(Talking && TalkSprite == 144)
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 47+(TalkId-TalkSprite), m->xFlip);
			else					
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 47, m->xFlip);
			break;
		case MARK:
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 69, m->xFlip);
			if(Talking && TalkSprite == 148){
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 67+(TalkId-TalkSprite), m->xFlip);
			}
			else
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);
					//LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 67, m->xFlip);		
			break;
		case RUSTY:
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);			
			if(Talking && TalkSprite == 150){
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 87+(TalkId-TalkSprite), m->xFlip);
			}
			else
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 87, m->xFlip);			
			break;
		case XAHN:
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);			
			if(Talking && TalkSprite == 146){
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 107+(TalkId-TalkSprite), m->xFlip);
			}
			else
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 107, m->xFlip);
			break;
		case GENERAL:
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);			
			if(Talking && TalkSprite == 152){
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 127+(TalkId-TalkSprite), m->xFlip);
			}
			else
				LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 127, m->xFlip);
			break;
		case JELLY_BLOB:

			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, false);
			break;

		case GBASLEEP:

			LD3.PutSprite(&WinApp, ex, ey+20, m->spriteset, (int)m->lani+1, false);
			break;

		case TROOP:

			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);
			break;

		case CHOPPER:

			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);
			break;

		case GREENDOOR:
			LD3.PutSprite(&WinApp, ex, ey-5, m->spriteset, (int)m->lstart+56, m->xFlip);
			LD3.PutSprite(&WinApp, ex, ey+20, m->spriteset, (int)m->lani+7, m->xFlip);			
			break;
		case BLUEDOOR:
			LD3.PutSprite(&WinApp, ex, ey-5, m->spriteset, (int)m->lstart+43, m->xFlip);
			LD3.PutSprite(&WinApp, ex, ey+20, m->spriteset, (int)m->lani+7, m->xFlip);
			break;
		case YELLOWDOOR:
			LD3.PutSprite(&WinApp, ex, ey-5, m->spriteset, (int)m->lstart+30, m->xFlip);
			LD3.PutSprite(&WinApp, ex, ey+20, m->spriteset, (int)m->lani+7, m->xFlip);
			break;
		case REDDOOR:
			LD3.PutSprite(&WinApp, ex, ey-5, m->spriteset, (int)m->lstart+17, m->xFlip);
			LD3.PutSprite(&WinApp, ex, ey+20, m->spriteset, (int)m->lani+7, m->xFlip);
			break;

		case VROCK_MONSTER:

			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->uani, m->xFlip);
		
			/*if(PlayingVRockSong == false && (m->visible && m->x-xShift >= -20 && m->x-xShift <= xRes && m->y-yShift >= -20 && m->y-yShift <= yRes)
				){
				FMUSIC_StopSong(song1);
				FMUSIC_PlaySong(mscVRock);
				PlayingVRockSong = true;
			}*/

			break;
		
		case PLATFORM_CIRCLE:

			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
			
			break;

		case SMASH:

			ex = int(m->x-xShift);
			ey = int(m->y-yShift);
			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 40, false);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 43, false);
			LD3.PutSprite(&WinApp, ex-20, ey+20, m->spriteset, 20, false);
			LD3.PutSprite(&WinApp, ex,    ey+20, m->spriteset, 26, false);
			LD3.PutSprite(&WinApp, ex+20, ey+20, m->spriteset, 21, false);
			LD3.PutSprite(&WinApp, ex-20, ey+40, m->spriteset, 20, false);
			LD3.PutSprite(&WinApp, ex,    ey+40, m->spriteset, 26, false);
			LD3.PutSprite(&WinApp, ex+20, ey+40, m->spriteset, 21, false);
			LD3.PutSprite(&WinApp, ex-20, ey+60, m->spriteset, 27, false);
			LD3.PutSprite(&WinApp, ex,    ey+60, m->spriteset, 22, false);
			LD3.PutSprite(&WinApp, ex+20, ey+60, m->spriteset, 23, false);
						
			break;

		case ELEVATOR:

			ex = int(m->x-xShift);
			ey = int(m->y-yShift);

			if(ShowPlayer == false && m->count > 0){
				LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, 0, 6, Player->xFlip);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, 0, (int)Player->lani, Player->xFlip);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, 0, (int)Player->uani, Player->xFlip);				
			}
				
			LD3.PutSprite(&WinApp, ex-m->count, ey, m->spriteset, 90, false);
			LD3.PutSprite(&WinApp, ex+20+m->count, ey, m->spriteset, 90, false);
			LD3.PutSprite(&WinApp, ex-m->count, ey+20, m->spriteset, 91, false);
			LD3.PutSprite(&WinApp, ex+20+m->count, ey+20, m->spriteset, 91, false);

			break;

		
		case SWING:

			for(n = 0; n <= 40; n += 10){
				nx = int(m->ox+Cosine[(int)m->count2]*n);
				ny = int(m->oy+fabs(Sine[(int)m->count2])*n);
				LD3.PutSprite(&WinApp, int(nx-xShift), int(ny-yShift), -1, SpikeBallChain, false);
			}			
			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->lani, m->xFlip);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
						
			break;

		case SWING2:

			for(n = 0; n <= 40; n += 10){
				nx = int(m->ox+Cosine[(int)m->count2]*n);
				ny = int(m->oy+fabs(Sine[(int)m->count2])*n);
				LD3.PutSprite(&WinApp, int(nx-xShift), int(ny-yShift), -1, SpikeBallChain, false);
			}			
			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
			LD3.PutSprite(&WinApp, ex, ey, m->spriteset, (int)m->lani, m->xFlip);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
			
			break;

		case SPIKEBALL1:

			for(n = 0; n <= 30; n += 10){
				nx = int(m->ox+Cosine[(int)m->count2]*n);
				ny = int(m->oy-Sine[(int)m->count2]*n);
				LD3.PutSprite(&WinApp, int(nx-xShift), int(ny-yShift), -1, SpikeBallChain, false);
			}
			
			LD3.PutSprite(&WinApp, ex, ey, -1, SpikeBall, false);
			
			break;

		case SPIKEBALL2:

			for(n = 0; n <= 30; n += 10){
				nx = int(m->ox+Cosine[(int)m->count2]*n);
				ny = int(m->oy-Sine[(int)m->count2]*n);
				LD3.PutSprite(&WinApp, int(nx-xShift), int(ny-yShift), -1, SpikeBallChain, false);
			}
			
			LD3.PutSprite(&WinApp, ex, ey, -1, SpikeBall, false);
			
			break;
		
		case PLATFORM_TRAP:

			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
			
			break;
			
		case ANCHOR:

			LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 49, false);
			LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 51, false);
						
			break;
		
		case BOSS1:

			if(xShift == BossXShift && yShift == BossYShift){
				
				/*if(m->y-yShift >= -20 && m->y-yShift <= yRes){
					if(m->xFlip){
						if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)-20, int(m->y-yShift), m->spriteset, 2, m->xFlip);
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)+20, int(m->y-yShift), m->spriteset, 0, m->xFlip);
					}
					else
					{
						if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)-20, int(m->y-yShift), m->spriteset, 0, m->xFlip);
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)+20, int(m->y-yShift), m->spriteset, 2, m->xFlip);
					}				
				}*/
				int c = 0;
				int y = 0;
				for(int x = 0; x <= 80; x += 20){
					LD3.PutSprite(&WinApp, ex+x, ey+y, m->spriteset, c, false);
					c++;
				}
				c = (int)m->uani;
				for(y = 20; y <= 40; y += 20){
					for(int x = 0; x <= 80; x += 20){
						LD3.PutSprite(&WinApp, ex+x, ey+y, m->spriteset, c, false);
						c++;
					}
				}
				LD3.DrawBox(&WinApp, 217, 2, int(217+m->life), 6, int(36+(m->life/20)), true);
				if(m->life < 100) LD3.DrawBox(&WinApp, int(218+m->life), 2, 317, 6, 34, true);
			}
			break;

		case BOSS2:

			if(xShift == BossXShift && yShift == BossYShift){
				if(m->y-yShift >= -20 && m->y-yShift <= yRes){
					if(m->xFlip){
						if(m->x-xShift >= -20 && m->x-xShift-20 <= xRes){
							LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 5, m->xFlip);
							LD3.PutSprite(&WinApp, ex, ey-20, m->spriteset, 7, m->xFlip);
						}
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes){
							LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 4, m->xFlip);
							LD3.PutSprite(&WinApp, ex+20, ey-20, m->spriteset, 6, m->xFlip);
						}
					}
					else
					{
						if(m->x-xShift >= -20 && m->x-xShift-20 <= xRes){
							LD3.PutSprite(&WinApp, ex, ey, m->spriteset, 4, m->xFlip);
							LD3.PutSprite(&WinApp, ex, ey-20, m->spriteset, 6, m->xFlip);
						}
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes){
							LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 5, m->xFlip);
							LD3.PutSprite(&WinApp, ex+20, ey-20, m->spriteset, 7, m->xFlip);
						}
					}				
				}
			}
			break;

		case BOSS3:

			
			if(m->visible){
			
				//- draw arms
				int ax = -20;
				n = Boss->uani;
				for(int y = 0; y <= 39; y += 20){
					for(int x = 0; x <= 39; x += 20){
						LD3.PutSprite(&WinApp, int(m->x-xShift+ax+x), int(m->y-yShift+y+20-abs(7-(((int)Boss->count3)&15))), BOSS_SPRITE_SET, n, false);
						n++;
					}
				}
				n = Boss->uani+3;
				ax = 40;
				for(int y = 39; y >= 0; y -= 20){
					for(int x = 39; x >= 0; x -= 20){
						LD3.PutSprite(&WinApp, int(m->x-xShift+ax+(40-x)), int(m->y-yShift+y-(15-abs(7-(((int)Boss->count3)&15)))), BOSS_SPRITE_SET, n, true);
						n--;
					}
				}
				
				//- draw head
				int n = 1;
				for(int y = 0; y <= 59; y += 20){
					for(int x = 0; x <= 59; x += 20){
						LD3.PutSprite(&WinApp, int(m->x-xShift+x), int(m->y-yShift+y), BOSS_SPRITE_SET, n, false);
						n++;
					}
				}
				
				LD3.DrawBox(&WinApp, 217, 2, int(217+m->life), 6, int(36+(m->life/20)), true);
				if(m->life < 100) LD3.DrawBox(&WinApp, int(218+m->life), 2, 317, 6, 34, true);
			
			}


			break;

		case BOSS4:

			//if(xShift == BossXShift && yShift == BossYShift){
				
				/*if(m->y-yShift >= -20 && m->y-yShift <= yRes){
					if(m->xFlip){
						if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)-20, int(m->y-yShift), m->spriteset, 2, m->xFlip);
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)+20, int(m->y-yShift), m->spriteset, 0, m->xFlip);
					}
					else
					{
						if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)-20, int(m->y-yShift), m->spriteset, 0, m->xFlip);
						if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
							LD3.PutSprite(&WinApp, int(m->x-xShift)+20, int(m->y-yShift), m->spriteset, 2, m->xFlip);
					}				
				}*/
				
				if(m->visible && m->x-xShift >= -20 && m->x-xShift <= xRes && m->y-yShift >= -20 && m->y-yShift <= yRes){
					//LD3.PutSprite(&WinApp, int(m->x-xShift), int(m->y-yShift), BOSS_SPRITE_SET, 9, m->xFlip);
					LD3.PutSprite(&WinApp, ex, ey, BOSS_SPRITE_SET, (int)Boss->uani, m->xFlip);
					LD3.DrawBox(&WinApp, 217, 2, int(217+m->life), 6, int(36+(m->life/20)), true);
				}
				
				if(m->life < 100) LD3.DrawBox(&WinApp, int(218+m->life), 2, 317, 6, 34, true);
			//}
			break;
		
		case LASTBOSS:

			if(m->visible && m->x-xShift >= -120 && m->x-xShift <= xRes+120 && m->y-yShift >= -120 && m->y-yShift <= yRes+120){
				//LD3.PutSprite(&WinApp, int(m->x-xShift), int(m->y-yShift), BOSS_SPRITE_SET, 9, m->xFlip);
				//LD3.PutSprite(&WinApp, int(m->x-xShift), int(m->y-yShift), BOSS_SPRITE_SET, (int)Boss->uani, m->xFlip);

				if(m->xFlip == false){
					//- draw neck
					n = 50;
					bx = ex-45;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 50;
					bx = ex-15;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 46;
					bx = ex-30;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					
					//- draw body
					n = 21;
					bx = ex;
					by = ey;
					for(int y = 0; y <= 80; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw foot
					n = 20;
					bx = ex+35;
					by = ey+85;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 60; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n+80, m->xFlip);
							n++;
						}
					}				
					//- draw leg
					n = 60;
					bx = ex+60;
					by = ey+60;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					/*n = 60;
					bx = ex+70;
					by = ey+80;				
					for(y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}*/
					//- draw leg shoulder
					n = 54;
					bx = ex+55;
					by = ey+40;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw arm
					n = 60;
					bx = ex;
					by = ey+60;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 60;
					bx = ex+10;
					by = ey+80;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw hand
					n = 0;
					bx = ex-25;
					by = ey+85;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n+80, m->xFlip);
							n++;
						}
					}				
					//- draw shoulder
					n = 54;
					bx = ex-5;
					by = ey+40;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw head
					n = 1;
					bx = ex-120;
					by = ey+30;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx+x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
				}
				else{
					//- draw neck
					n = 50;
					bx = ex+45;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 50;
					bx = ex+15;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 46;
					bx = ex+30;
					by = ey+45;
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 20; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					
					//- draw body
					n = 21;
					bx = ex;
					by = ey;
					for(int y = 0; y <= 80; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw foot
					n = 20;
					bx = ex-35;
					by = ey+85;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 60; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n+80, m->xFlip);
							n++;
						}
					}				
					//- draw leg
					n = 60;
					bx = ex-60;
					by = ey+60;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					/*n = 60;
					bx = ex+70;
					by = ey+80;				
					for(y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}*/
					//- draw leg shoulder
					n = 54;
					bx = ex-55;
					by = ey+40;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw arm
					n = 60;
					bx = ex;
					by = ey+60;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					n = 60;
					bx = ex-10;
					by = ey+80;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw hand
					n = 0;
					bx = ex+25;
					by = ey+85;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n+80, m->xFlip);
							n++;
						}
					}				
					//- draw shoulder
					n = 54;
					bx = ex+5;
					by = ey+40;				
					for(int y = 0; y <= 20; y += 20){
						for(int x = 0; x <= 40; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
					//- draw head
					n = 1;
					bx = ex+120;
					by = ey+30;				
					for(int y = 0; y <= 60; y += 20){
						for(int x = 0; x <= 80; x += 20){
							LD3.PutSprite(&WinApp, bx-x, by+y, BOSS_SPRITE_SET, n, m->xFlip);
							n++;
						}
					}
				}			
			}
			
			LD3.DrawBox(&WinApp, 217, 2, int(217+m->life), 6, int(36+(m->life/20)), true);
			if(m->life < 100) LD3.DrawBox(&WinApp, int(218+m->life), 2, 317, 6, 34, true);
			break;
		}
		if(m->id == PLATFORM_Lr || m->id == PLATFORM_lR){
			if(m->y-yShift >= -20 && m->y-yShift <= yRes){
				if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
					LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
				if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
					LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
			}
		}
		else if(m->id == PLATFORM_Ud || m->id == PLATFORM_uD)
		{			
			if(m->y-yShift >= -20 && m->y-yShift <= yRes){
				if(m->x-xShift-20 >= -20 && m->x-xShift-20 <= xRes)
					LD3.PutSprite(&WinApp, ex-20, ey, m->spriteset, 44, false);
				if(m->x-xShift+20 >= -20 && m->x-xShift+-20 <= xRes)
					LD3.PutSprite(&WinApp, ex+20, ey, m->spriteset, 46, false);
			}			
		}		
		}
	}
	
}

//=================================================================================================
//= void Splatter(int x, int y, int amount, float xdir)
//= This function assigns new red colored particles to the particle engine to give the appearance
//= of blood
//= x, y   - assign the rough coordinates of the new particles
//=        - The particles are randomly assigned +1, 0, or -1 to the x and y coordinates
//= amount - determines how many particles will be created
//= xdir   - determines how much push the particles have to the left or right
//=================================================================================================
void Splatter(int x, int y, int amount, float xdir, int col)
{
	//- Create blood splatter particles
	PARTICLE *bp;
	int angle;
	
	if(NumParticles+amount > 490) amount = 490-NumParticles;
	
	for(int i = NumParticles; i <= NumParticles+amount; i++){
		bp = &Particle[i];
		angle = 45+rand()%90;
		bp->x = float(x+(1-rand()%2));
		bp->y = float(y+(1-rand()%2));
		if(xdir == 0){
			xdir = (1-rand()%2);
			bp->xspeed =  Cosine[angle]*xdir;
		}
		else
			bp->xspeed =  Cosine[angle]*xdir;
		bp->yspeed = -Sine[angle];
		bp->yspeed *= (float)(rand()%20)/10;
		bp->timer  = 300;
		bp->yvelocity = 0;
		bp->col    = col+(2-rand()%4);
	}
	
	NumParticles += amount;	
}

//=================================================================================================
//= void Squirt(int x, int y, int amount, float xdir)
//= This function assigns new colored particles to the particle engine
//= x, y   - assign the rough coordinates of the new particles
//=        - The particles are randomly assigned +1, 0, or -1 to the x and y coordinates
//= amount - determines how many particles will be created
//= xdir   - determines how much push the particles have to the left or right
//=================================================================================================
void Squirt(int x, int y, int amount, float xdir, int col)
{
	//- Create blood splatter particles
	PARTICLE *bp;
	int angle;
	
	if(NumParticles+amount > 490) amount = 490-NumParticles;
	
	for(int i = NumParticles; i <= NumParticles+amount; i++){
		bp = &Particle[i];
		angle = 10;
		bp->x = x+i-NumParticles;
		bp->y = y;
		bp->xspeed =  Cosine[angle]*xdir;
		bp->yspeed = -Sine[angle];
		bp->yspeed *= (float)(rand()%20)/10;
		bp->timer  = 120;
		bp->yvelocity = 0;
		bp->col    = col+(2-rand()%4);
	}
	
	NumParticles += amount;	
}

//=================================================================================================
//= void ProcessBlood()
//= Draw and process the blood
//= This function also removes particles if their timer has reached zero
//=================================================================================================
void ProcessParticles()
{
	//- Draw and move the blood
	PARTICLE *bp;
	int delcount = 0;

	for(int i = 1; i <= NumParticles; i++){		
		bp = &Particle[i];
		LD3.PutPixel(&WinApp, int(bp->x-xShift), int(bp->y-yShift), bp->col);
		//bp->x += bp->xspeed*diff;
		//bp->y += bp->yspeed*diff;
		//bp->yvelocity += 0.009f;//*diff;
		//bp->yspeed += bp->yvelocity;//*diff;
		bp->x += bp->xspeed*diff;
		bp->y += bp->yspeed*diff;
		bp->yvelocity += 0.0015f*diff;
		bp->yspeed += bp->yvelocity*diff;
		bp->timer--;		
	}

	int i = 1;
	while(i <= NumParticles)
	{
		bp = &Particle[i];
		if(bp->timer <= 0 || bp->x-xShift < 0 || bp->y-yShift < 0 || bp->x-xShift >= xRes || bp->y-yShift >= yRes){
			for(int n = i; n <= NumParticles-1; n++){
				Particle[n] = Particle[n+1]; 				
			}
			NumParticles--;
		}
		else
			i++;
	}

	/*for(i = 1; i <= NumParticles; i++){
		bp = &Particle[i];
		if(bp->timer <= 0 || bp->x-xShift < 0 || bp->y-yShift < 0 || bp->x-xShift >= xRes || bp->y-yShift >= yRes){
			for(int n = i; n <= NumParticles-1; n++){
				Particle[n] = Particle[n+1]; 
				delcount++;
			}
		}
	}

	NumParticles -= delcount;
	if(NumParticles < 0) NumParticles = 0;*/
}

//=================================================================================================
//= void ProcessCannons()
//= This function processes the cannons by making them shoot when their timer reaches zero
//=================================================================================================
void ProcessCannons()
{
	//- Make the cannons shoot when their timer reaches zero
	int r, cx, cy;
	
	for(int i = 1; i <= NumCannons; i++)
	{
		Cannon[i].timer -= 1;
		
		if(Cannon[i].timer <= 0){
			if(Cannon[i].id == YELLOW_FIRE){
				Cannon[i].timer = 100;
				FireProjectile((float)Cannon[i].x, (float)Cannon[i].y, (int)Cannon[i].dir, 1, YELLOW_FIRE, &NullEntity, 0);
			}
			else if(Cannon[i].id == TOXIC_BUBBLE){
				r = rand()%5;
				if(r == 1) Cannon[i].timer = 70+rand()%30; else Cannon[i].timer = 1;
				FireProjectile((float)Cannon[i].x, (float)Cannon[i].y, (int)Cannon[i].dir, 0.5f, TOXIC_BUBBLE, &NullEntity, 0);
				if(r == 1){
					cx = Cannon[i].x-xShift;
					cy = Cannon[i].y-yShift;
					if(cx >= -20 && cx <= xRes && cy >= -20 && cy <= yRes){						
						FSOUND_PlaySound(FSOUND_FREE,sndBubbles);
					}
				}
			}
		}
	}
}

//=================================================================================================
//= void ScanMap()
//= Scan the map for monsters, items, etc.
//=================================================================================================
void ScanMap()
{
	//- Scan map for enemies
	int mi = 0, nm = 0, nl = 0, nc = 0, ne = 0, n = 0;
	long mpt = 0;

	int SkipEnemy = 0;		//- If difficulty < HARD then skip every other or more enemies when this reaches 10

	NumMonsters = 0;
	for(int my = 0; my < MAPHEIGHT-1; my++){
		for(int mx = 0; mx < MAPWIDTH-1; mx++){
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mi  = LD3.Map[mpt];
			if(mi > 140 && mi < 159) NumMonsters++;
		}
	}
	for(int i = 1; i <= NumMonsters; i++) Monster[i].Init();
	NumMonsters = 0;
		
	for(int ml = 0; ml <= 1; ml++)
	{		
		for(int my = 0; my < MAPHEIGHT; my++)
		{
			for(int mx = 0; mx < MAPWIDTH; mx++)
			{
				mpt = mx+(my<<MAPXLSHIFT)+(ml*55000);
				mi  = LD3.Map[mpt];

				if(mi >= 256 && mi <= 400){
					ScoreMap[mx][my] = 0;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
				}
				
				switch(mi)
				{
				case miLarryStart:					
					Player->x = (float)mx*20;
					Player->y = (float)my*20;
					Player->ox = Player->x;
					Player->oy = Player->y;
					Player->uani = 10;
					Marker.x = (int)Player->x;
					Marker.y = (int)Player->y;					
					LD3.Map[mpt] = 0;
					break;

				case miPositionOne:

					Position[0].x = mx*20;
					Position[0].y = my*20+5;
					break;

				case miPositionTwo:

					Position[1].x = mx*20;
					Position[1].y = my*20+5;
					break;

				case miExplosive:

					NumExplosives++;
					ne = NumExplosives;
					Explosive[ne].x = mx*20;
					Explosive[ne].y = my*20;
					
					LD3.Map[mpt] = 0;
					break;

				case miGreenDoor:

					NumMonsters++;
					nm = NumMonsters;
					Monster[nm].Init();
					Monster[nm].id			= GREENDOOR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					Monster[nm].count2		= mpt;
					Monster[nm].count3		= mpt+MAPWIDTH;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miBlueDoor:

					NumMonsters++;
					nm = NumMonsters;
					Monster[nm].Init();
					Monster[nm].id			= BLUEDOOR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					Monster[nm].count2		= mpt;
					Monster[nm].count3		= mpt+MAPWIDTH;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miYellowDoor:

					NumMonsters++;
					nm = NumMonsters;
					Monster[nm].Init();
					Monster[nm].id			= YELLOWDOOR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					Monster[nm].count2		= mpt;
					Monster[nm].count3		= mpt+MAPWIDTH;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miRedDoor:

					NumMonsters++;
					nm = NumMonsters;		
					Monster[nm].Init();
					Monster[nm].id			= REDDOOR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					Monster[nm].count2		= mpt;
					Monster[nm].count3		= mpt+MAPWIDTH;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miSpikePlant:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= SPIKEPLANT;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+5;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miSpider:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					if(Level > 6)
						Monster[nm].id			= SPIDER;
					else
						Monster[nm].id			= BLOODHEAD;

					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+5;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miRat:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= RAT;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+5;
					LD3.Map[mpt] = 0;
					//TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miBigSpider:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= BIGSPIDER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miBigSpider2:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= BIGSPIDER2;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+4;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miAlDog:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					if(Level > 6)
						Monster[nm].id			= ALDOG;
					else
						Monster[nm].id			= GREENBLOB;
					
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+5;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;


				case miElevatorDown:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= ELEVATORDOWN;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+4;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);

					NumElevators++;
					ne = NumElevators;
					Elevator[ne] = &Monster[nm];
					break;

				case miElevatorUp:

					NumMonsters++;
					nm = NumMonsters;					
					Monster[nm].Init();
					Monster[nm].id			= ELEVATORUP;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+4;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);

					NumElevators++;
					ne = NumElevators;
					Elevator[ne] = &Monster[nm];
					break;

				case miToxicBubbles_UP:

					NumCannons++;
					nc = NumCannons;

					Cannon[nc].id			= TOXIC_BUBBLE;
					Cannon[nc].x			= mx*20;
					Cannon[nc].y			= my*20+8;
					Cannon[nc].dir			= UP;
					Cannon[nc].spriteset	= YELLOW_FIRE_SPRITE_SET;
					Cannon[nc].spritenum	= TOXIC_BUBBLE;
					Cannon[nc].timer		= rand()%100;
					LD3.Map[mpt] = 0;
										
					break;

				case miToxicBubbles_LT:

					NumCannons++;
					nc = NumCannons;

					Cannon[nc].id			= TOXIC_BUBBLE;
					Cannon[nc].x			= mx*20+7;
					Cannon[nc].y			= my*20;
					Cannon[nc].dir			= LEFT;
					Cannon[nc].spriteset	= YELLOW_FIRE_SPRITE_SET;
					Cannon[nc].spritenum	= TOXIC_BUBBLE;
					Cannon[nc].timer		= rand()%100;
					LD3.Map[mpt] = 0;					
					
					break;

				case miToxicBubbles_RT:

					NumCannons++;
					nc = NumCannons;

					Cannon[nc].id			= TOXIC_BUBBLE;
					Cannon[nc].x			= mx*20-6;
					Cannon[nc].y			= my*20;
					Cannon[nc].dir			= RIGHT;
					Cannon[nc].spriteset	= YELLOW_FIRE_SPRITE_SET;
					Cannon[nc].spritenum	= TOXIC_BUBBLE;
					Cannon[nc].timer		= rand()%100;
					LD3.Map[mpt] = 0;
					
					break;

				case miSmashUp:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SMASH_UP;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);

					break;

				case miSmashDown:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SMASH_DOWN;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);

					break;

				case miTeleport1:
					NumTeleports++;
					nm = NumTeleports;
					Teleport[nm].x			= mx*20;
					Teleport[nm].y			= my*20;
					Teleport[nm].l			= 1;
					break;
				case miTeleport2:
					NumTeleports++;
					nm = NumTeleports;
					Teleport[nm].x			= mx*20;
					Teleport[nm].y			= my*20;
					Teleport[nm].l			= 2;
					break;
				case miTeleport3:
					NumTeleports++;
					nm = NumTeleports;
					Teleport[nm].x			= mx*20;
					Teleport[nm].y			= my*20;
					Teleport[nm].l			= 3;
					break;
				case miTeleport4:
					NumTeleports++;
					nm = NumTeleports;
					Teleport[nm].x			= mx*20;
					Teleport[nm].y			= my*20;
					Teleport[nm].l			= 4;
					break;

				case miSonicMonster:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SONIC_MONSTER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miBlobMine:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= BLOB_MINE;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miTroop:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= TROOP;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miJellyBlob:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= JELLY_BLOB;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miRockMonster:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= ROCK_MONSTER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;						
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miVRockMonster:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= VROCK_MONSTER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;


				case miFly:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= FLY_MONSTER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;
				
				case miMegaFly:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= MEGAFLY;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;
				
				case miIcedSpikehead:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= ICED_SPIKEHEAD;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;					
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;

				case miBoss:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= BOSS;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;		
					LD3.Map[mpt] = 0;
					TotalKills++;
					SetDefaultValue(&Monster[nm]);
					break;
				
				case miFletcher:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= FLETCHER;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;		
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);					
					break;

				case miMark:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= MARK;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;	
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					
					break;

				case miBadMark:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= BOSS;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;	
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					
					break;

				case miRusty:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= RUSTY;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					
					break;

				case miGeneral:
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					//if(Level == lvGOLDEN_ESTATE)
					//	Monster[nm].id			= GBASLEEP;
					//else
						Monster[nm].id			= GENERAL;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					
					break;

				case miXahn:
					
					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= XAHN;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					
					break;
				
				case miSwing:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SWING;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miSwing2:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SWING2;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;
				
				case miPlatform_Lr:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= PLATFORM_Lr;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miPlatform_lR:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= PLATFORM_lR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miPlatform_Ud:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= PLATFORM_Ud;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miPlatform_uD:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= PLATFORM_uD;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miPlatform_Trap:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= PLATFORM_TRAP;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miAnchor:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= ANCHOR;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miPlatform_CL:

					for(n = 0; n <= 1; n++){
						NumMonsters++;
						nm = NumMonsters;

						Monster[nm].Init();
						Monster[nm].x		= (float)mx*20;
						Monster[nm].y		= (float)my*20;
						Monster[nm].ox		= Monster[nm].x;
						Monster[nm].oy		= Monster[nm].y;
						Monster[nm].id		= PLATFORM_CIRCLE;
						Monster[nm].spriteset= -1;
						Monster[nm].lani	= 45;
						Monster[nm].lstart	= 45;
						Monster[nm].lend	= 45;
						Monster[nm].life	= 20;
						Monster[nm].dontfall= true;
						Monster[nm].xFlip	= false;
						Monster[nm].count	= 0;
						Monster[nm].setspeed= 1;
						Monster[nm].BloodCol= PlatformBlood;
						Monster[nm].HitBox.left		= -20;
						Monster[nm].HitBox.right	=  39;
						Monster[nm].HitBox.top		=   0;
						Monster[nm].HitBox.bottom	=  19;
					}
					
					Monster[nm].count2 = 0;
					Monster[nm-1].count2 = 180;
					LD3.Map[mpt] = 0;
					break;

				case miPlatform_CR:

					for(n = 0; n <= 1; n++){
						NumMonsters++;
						nm = NumMonsters;

						Monster[nm].Init();
						Monster[nm].x		= (float)mx*20;
						Monster[nm].y		= (float)my*20;
						Monster[nm].ox		= Monster[nm].x;
						Monster[nm].oy		= Monster[nm].y;
						Monster[nm].id		= PLATFORM_CIRCLE;
						Monster[nm].spriteset= -1;
						Monster[nm].lani	= 45;
						Monster[nm].lstart	= 45;
						Monster[nm].lend	= 45;
						Monster[nm].life	= 20;
						Monster[nm].dontfall= true;
						Monster[nm].xFlip	= false;
						Monster[nm].count	= 0;
						Monster[nm].setspeed= -1;
						Monster[nm].BloodCol= PlatformBlood;
						Monster[nm].HitBox.left		= -20;
						Monster[nm].HitBox.right	=  39;
						Monster[nm].HitBox.top		=   0;
						Monster[nm].HitBox.bottom	=  19;
					}
					
					Monster[nm].count2 = 0;
					Monster[nm-1].count2 = 180;
					LD3.Map[mpt] = 0;
					break;

				case miSpikeBall:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id			= SPIKEBALL1;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miSpikeBall2:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					Monster[nm].id		= SPIKEBALL2;
					Monster[nm].startx	= mx*20;
					Monster[nm].starty	= my*20;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;

				case miRotoPlatform_CL:

					for(n = -2; n <= 2; n += 4){
						NumMonsters++;
						nm = NumMonsters;

						Monster[nm].Init();
						Monster[nm].x		= (float)mx*20+(n*20);
						Monster[nm].y		= (float)my*20+(n*20);
						Monster[nm].ox		= (float)mx*20;
						Monster[nm].oy		= (float)my*20;
						Monster[nm].id		= ROTOPLATFORM;
						Monster[nm].spriteset= -1;
						Monster[nm].lani	= 48;
						Monster[nm].lstart	= 48;
						Monster[nm].lend	= 48;
						Monster[nm].life	= 20;
						Monster[nm].dontfall= true;
						Monster[nm].xFlip	= false;
						if(n == -2){Monster[nm].count = 0; Monster[nm].count2 = 1;}
						if(n ==  2){Monster[nm].count = 0; Monster[nm].count2 = -1;}
						Monster[nm].setspeed= 1;
						Monster[nm].BloodCol= PlatformBlood;
						Monster[nm].HitBox.left		= -20;
						Monster[nm].HitBox.right	=  39;
						Monster[nm].HitBox.top		=   0;
						Monster[nm].HitBox.bottom	=  19;
					}
					
					LD3.Map[mpt] = 0;
					break;

				case miRotoPlatform_CR:

					for(n = -2; n <= 2; n += 4){
						NumMonsters++;
						nm = NumMonsters;

						Monster[nm].Init();
						Monster[nm].x		= (float)mx*20+(n*20);
						Monster[nm].y		= (float)my*20+(n*20);
						Monster[nm].ox		= (float)mx*20;
						Monster[nm].oy		= (float)my*20;
						Monster[nm].id		= ROTOPLATFORM;
						Monster[nm].spriteset= -1;
						Monster[nm].lani	= 48;
						Monster[nm].lstart	= 48;
						Monster[nm].lend	= 48;
						Monster[nm].life	= 20;
						Monster[nm].dontfall= true;
						Monster[nm].xFlip	= false;
						if(n == -2){Monster[nm].count = 0; Monster[nm].count2 = 1;}
						if(n ==  2){Monster[nm].count = 0; Monster[nm].count2 = -1;}
						Monster[nm].setspeed= -1;
						Monster[nm].BloodCol= PlatformBlood;
						Monster[nm].HitBox.left		= -20;
						Monster[nm].HitBox.right	=  39;
						Monster[nm].HitBox.top		=   0;
						Monster[nm].HitBox.bottom	=  19;
					}

					LD3.Map[mpt] = 0;
					break;

				case miGoCart:

					NumMonsters++;
					nm = NumMonsters;

					Monster[nm].Init();
					if(Level == lvFROST_BITE)
						Monster[nm].id			= SNOWMOBILE;						
					else
						Monster[nm].id			= GO_CART;
					Monster[nm].startx		= mx*20;
					Monster[nm].starty		= my*20+7;
					LD3.Map[mpt] = 0;
					SetDefaultValue(&Monster[nm]);
					break;
				
				//case miBoulder_Left:

				//	LD3.Map[mpt] = 77;
				//	break;

				//case miBoulder_Right:

				//	LD3.Map[mpt] = 78;
				//	break;

				case miLaser:

					NumLasers++;
					nl = NumLasers;

					Laser[nl].x = mx;
					Laser[nl].y = my;
					Laser[nl].l = ml;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;

					break;

				case miTouchPlateWall:

					NumWalls++;
					nl = NumWalls;

					Wall[nl].x = mx;
					Wall[nl].y = my;
					Wall[nl].l = ml;				

					break;

				case miTreadmillLeft:
					if(ml == 1)	LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 5;
					break;
				case miTreadmillRight:
					if(ml == 1) LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 5;
					break;

				case miFireCannonUp:

					if(ml == 1){
						NumCannons++;
						nc = NumCannons;

						Cannon[nc].id	= YELLOW_FIRE;
						Cannon[nc].x	= mx*20;
						Cannon[nc].y	= my*20;
						Cannon[nc].dir	= UP;
						Cannon[nc].spriteset = 0;
						Cannon[nc].spritenum = YELLOW_FIRE_UP;
						Cannon[nc].timer	 = rand()%100;
						LD3.Map[mpt] = 0;
					}
					break;
				case miFireCannonDown:

					if(ml == 1){
						NumCannons++;
						nc = NumCannons;

						Cannon[nc].id	= YELLOW_FIRE;
						Cannon[nc].x	= mx*20;
						Cannon[nc].y	= my*20;
						Cannon[nc].dir	= DOWN;
						Cannon[nc].spriteset = 0;
						Cannon[nc].spritenum = YELLOW_FIRE_DOWN;
						Cannon[nc].timer	 = rand()%100;
						LD3.Map[mpt] = 0;
					}
					break;
				case miFireCannonLeft:

					if(ml == 1){
						NumCannons++;
						nc = NumCannons;

						Cannon[nc].id	= YELLOW_FIRE;
						Cannon[nc].x	= mx*20;
						Cannon[nc].y	= my*20;
						Cannon[nc].dir	= LEFT;
						Cannon[nc].spriteset = 0;
						Cannon[nc].spritenum = YELLOW_FIRE_LEFT;
						Cannon[nc].timer	 = rand()%100;
						LD3.Map[mpt] = 0;
					}
					break;
				case miFireCannonRight:

					if(ml == 1){
						NumCannons++;
						nc = NumCannons;

						Cannon[nc].id	= YELLOW_FIRE;
						Cannon[nc].x	= mx*20;
						Cannon[nc].y	= my*20;
						Cannon[nc].dir	= RIGHT;
						Cannon[nc].spriteset = 0;
						Cannon[nc].spritenum = YELLOW_FIRE_RIGHT;
						Cannon[nc].timer	 = rand()%100;
						LD3.Map[mpt] = 0;
					}
					break;


				case miL:
					ScoreMap[mx][my] = 100;
					break;
				case miA:
					ScoreMap[mx][my] = 100;
					break;
				case miR1:
					ScoreMap[mx][my] = 100;
					break;
				case miR2:
					ScoreMap[mx][my] = 100;
					break;
				case miY:
					ScoreMap[mx][my] = 100;
					break;
				case miHealthBottle:
					ScoreMap[mx][my] = 0;
					break;
				case miHealthBottle2X:
					ScoreMap[mx][my] = 0;
					break;
				case miSpeedBottle:
					ScoreMap[mx][my] = 0;
					break;
				case miSpeedBottle2X:
					ScoreMap[mx][my] = 0;
					break;
				case miPowerBottle:
					ScoreMap[mx][my] = 0;
					break;
				case miPowerBottle2X:
					ScoreMap[mx][my] = 0;
					break;
				case miJumpBottle:
					ScoreMap[mx][my] = 0;
					break;
				case miJumpBottle2X:
					ScoreMap[mx][my] = 0;
					break;
				case miAllInOneBottle:
					ScoreMap[mx][my] = 0;
					break;
				case miAllInOneBottle2X:
					ScoreMap[mx][my] = 0;
					break;
				case miEndFlag:
					ScoreMap[mx][my] = 1000;
					break;					
				case miExtraLife:
					ScoreMap[mx][my] = 0;
					break;
				case miMagazine:
					ScoreMap[mx][my] = 0;
					break;
				case miBoxOfShells:
					ScoreMap[mx][my] = 0;
					break;
				case miGreenCrystal:
					ScoreMap[mx][my] = 50;
					TotalCrystals++;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
					break;
				case miYellowCrystal:
					ScoreMap[mx][my] = 100;
					TotalCrystals++;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
					break;
				case miBlueCrystal:
					ScoreMap[mx][my] = 500;
					TotalCrystals++;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
					break;
				case miRedCrystal:
					ScoreMap[mx][my] = 1000;
					TotalCrystals++;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
					break;
				case miGrenade:
					ScoreMap[mx][my] = 0;
					break;
				
				case miSecret1:
					if(Secret[0] == false){
						Secret[0] = true;
						NumSecrets++;
					}
					break;
				case miSecret2:
					if(Secret[1] == false){
						Secret[1] = true;
						NumSecrets++;
					}
					break;
				case miSecret3:
					if(Secret[2] == false){
						Secret[2] = true;
						NumSecrets++;
					}
				break;
					case miSecret4:
					if(Secret[3] == false){
						Secret[3] = true;
						NumSecrets++;
					}
					break;

				}
				
				if(Difficulty == JUSTICE && (mi == miMagazine || mi == miBoxOfShells || mi == miRifle || mi == miGrenade || mi == miChopper || mi == miExtraLife)){
					LD3.Map[mpt] = 0;
					LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 1;				
					ScoreMap[mx][my] = 0;
				}
				if(nm == NumMonsters && Difficulty == JUSTICE && (Monster[nm].ScoreValue > 0 || Monster[nm].id == BLOB_MINE)){
					//- duplicate 4 more enemies
					for(int n = 1; n <= 4; n++) Monster[nm+n] = Monster[nm];
					NumMonsters += 4;
				}
			}
		}
	}

	for(int i = 1; i <= NumMonsters; i++){
		if(Monster[i].ScoreValue > 0 || Monster[i].id == BLOB_MINE){
			if(Difficulty == BABY)   SkipEnemy += 7;
			if(Difficulty == EASY)   SkipEnemy += 5;
			if(Difficulty == NORMAL) SkipEnemy += 2;
			if(Difficulty == UNFAIR) Monster[i].life *= 2;

			if(Monster[i].id == VROCK_MONSTER && Difficulty < HARD && Level < 3){
				Monster[i].ScoreValue = 0;
				Monster[i].life = 0;				
			}

			if(SkipEnemy >= 10){
				SkipEnemy -= 10;
				//NumMonsters--;
				//Monster[NumMonsters].Init();
				Monster[i].ScoreValue = 0;
				Monster[i].life = 0;				
			}
		}	
	}
	int i = 1;
	ne = 0;
	while(i <= NumMonsters)
	{
		if(Monster[i].life <= 0 || Monster[i].y > 2000){
			for(int n = i; n <= NumMonsters-1; n++){				 
				Monster[n] = Monster[n+1];
				if(Monster[n].id == ELEVATOR){
					ne++;
					Elevator[ne] = &Monster[n];
				}
			}
			NumMonsters--;
			TotalKills--;
		}
		else
			i++;
	}

	//if(Difficulty < HARD)
	//				Player->life		+= 50;
	//	else
	//			Player->life		+= 25;

	if(Difficulty == BABY)   {HealthAdd1 = 100; HealthAdd2 = 200;}
	if(Difficulty == EASY)   {HealthAdd1 =  75; HealthAdd2 = 150;}
	if(Difficulty == NORMAL) {HealthAdd1 =  50; HealthAdd2 = 200;}
	if(Difficulty == HARD)   {HealthAdd1 =  25; HealthAdd2 =  50;}
	if(Difficulty == UNFAIR) {HealthAdd1 =  10; HealthAdd2 =  20;}
	if(Difficulty == JUSTICE){HealthAdd1 =  25; HealthAdd2 =  50;}

	if(Difficulty == BABY)   MaxLife = 200.0f;
	if(Difficulty == EASY)   MaxLife = 150.0f;
	if(Difficulty == NORMAL) MaxLife = 100.0f;
	if(Difficulty == HARD)   MaxLife = 100.0f;
	if(Difficulty == UNFAIR) MaxLife = 50.0f;
	if(Difficulty == JUSTICE)MaxLife = 100.0f;
	
	for(i = 0; i <= 3; i++) Secret[i] = false;
}

//=================================================================================================
//= void SetDefaultValue(ENTITY *Entity)
//= Set the default values for the given entity
//=================================================================================================
void SetDefaultValue(ENTITY *Entity)
{
	//- Set default values
	
	Entity->x		= (float)Entity->startx;
	Entity->y		= (float)Entity->starty;
	Entity->ox		= Entity->x;
	Entity->oy		= Entity->y;

	int	rnd;
				
	switch(Entity->id)
	{
		case SPIKEPLANT:

			Entity->spriteset= YELLOW_FIRE_SPRITE_SET;
			Entity->lani	= 11;
			Entity->lstart	= 11;
			Entity->lend	= 11;
			Entity->life	= 500;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 52;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 10;
			Entity->HitBox.bottom	= 19;
			Entity->count = 0;
			
			break;

		case SONIC_MONSTER:
			
			Entity->spriteset= SONIC_MONSTER_SPRITE_SET;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 9;
			Entity->life	= 15;
			Entity->dontfall= true;
			Entity->ScoreValue = 200;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 1;
			Entity->HitBox.bottom	= 19;
			
			break;

		case BLOB_MINE:
			
			Entity->spriteset= BLOB_MINE_SPRITE_SET;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 3;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 195;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 14;
			Entity->HitBox.bottom	= 19;
			Entity->y += 5;
			
			break;

		case TROOP:
			
			Entity->spriteset= TROOP_SPRITE_SET;
			rnd = rand()%2;
			if(rnd == 1){
				Entity->uani	= 20;
				Entity->ustart	= 20;
			}
			else{
				Entity->uani	= 17;
				Entity->ustart	= 17;
			}
			Entity->lani	= 10;
			Entity->lstart	= 10;
			Entity->lend	= 17;
			Entity->life	= 35;
			Entity->dontfall= true;
			Entity->ScoreValue = 500;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			
			break;

		case VROCK_MONSTER:
			
			Entity->spriteset= VROCK_MONSTER_SPRITE_SET;
			Entity->uani	= 140;
			Entity->ustart	= 140;
			Entity->uend	= 148;			
			Entity->lani	= 150;
			Entity->lstart	= 150;
			Entity->lend	= 159;
			Entity->life	= 100;
			Entity->dontfall= true;
			Entity->ScoreValue = 1000;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			
			break;

		case JELLY_BLOB:
			
			Entity->spriteset=JELLY_BLOB_SPRITE_SET;
			Entity->uani	= 120;
			Entity->ustart	= 120;
			Entity->uend	= 129;
			Entity->lani	= 130;
			Entity->lstart	= 130;
			Entity->lend	= 130;
			Entity->life	= 40;
			Entity->dontfall= true;
			Entity->ScoreValue = 500;
			Entity->BloodCol = 72;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 9;
			
			break;

		case ROCK_MONSTER:
			
			Entity->spriteset= ROCK_MONSTER_SPRITE_SET;
			Entity->lani	= 30;
			Entity->lstart	= 30;
			Entity->lend	= 39;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->ScoreValue = 200;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 5;
			Entity->HitBox.bottom	= 19;					
			
			break;

		case FLY_MONSTER:
			
			Entity->spriteset= FLY_MONSTER_SPRITE_SET;
			Entity->lani	= 60;
			Entity->lstart	= 60;
			Entity->lend	= 61;
			Entity->life	= 5;
			Entity->dontfall= true;
			Entity->ScoreValue = 50;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 4;
			Entity->HitBox.right	= 13;
			Entity->HitBox.top		= 6;
			Entity->HitBox.bottom	= 13;
			
			break;
				
		case MEGAFLY:
			
			Entity->spriteset= MEGAFLY_SPRITE_SET;
			Entity->lani	= 35;
			Entity->lstart	= 35;
			Entity->lend	= 36;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->ScoreValue = 250;
			Entity->BloodCol = 37;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			
			break;
		
		case ICED_SPIKEHEAD:
		
			Entity->spriteset= ICED_SPIKEHEAD_SPRITE_SET;
			Entity->lani	= 50;
			Entity->lstart	= 50;
			Entity->lend	= 50;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 50;
			Entity->count   = 0;
			Entity->BloodCol= 75;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 7;
			Entity->HitBox.bottom	= 19;					
			
			break;

		case SPIDER:
			
			Entity->spriteset= SPIDER_SPRITE_SET;
			Entity->lani	= 100;
			Entity->lstart	= 100;
			Entity->lend	= 107;
			Entity->life	= 5;
			Entity->dontfall= true;
			Entity->ScoreValue = 30;
			Entity->BloodCol = 72;
			Entity->HitBox.left		= 5;
			Entity->HitBox.right	= 14;
			Entity->HitBox.top		= 11;
			Entity->HitBox.bottom	= 19;
			
			break;

		case RAT:
			
			Entity->spriteset= RAT_SPRITE_SET;
			Entity->lani	= 95;
			Entity->lstart	= 95;
			Entity->lend	= 100;
			Entity->life	= 5;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 35;
			Entity->HitBox.left		= 4;
			Entity->HitBox.right	= 15;
			Entity->HitBox.top		= 15;
			Entity->HitBox.bottom	= 19;
			break;

		case BIGSPIDER:
			
			Entity->spriteset= BIGSPIDER_SPRITE_SET;
			Entity->lani	= 90;
			Entity->lstart	= 90;
			Entity->lend	= 94;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 100;
			Entity->BloodCol = 35;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 15;
			break;

		case BIGSPIDER2:
			
			Entity->spriteset= BIGSPIDER_SPRITE_SET;
			Entity->lani	= 85;
			Entity->lstart	= 85;
			Entity->lend	= 89;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 100;
			Entity->BloodCol = 35;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 4;
			Entity->HitBox.bottom	= 19;
			break;

		case ALDOG:
			
			Entity->spriteset= ALDOG_SPRITE_SET;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 5;
			Entity->life	= 15;
			Entity->dontfall= true;
			Entity->ScoreValue = 100;
			Entity->BloodCol = 71;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 9;
			Entity->HitBox.bottom	= 19;
			
			break;

		case GREENBLOB:
			
			Entity->spriteset= GREEN_BLOB_SPRITE_SET;
			Entity->lani	= 140;
			Entity->lstart	= 140;
			Entity->lend	= 145;
			Entity->life	= 25;
			Entity->dontfall= true;
			Entity->ScoreValue = 125;
			Entity->BloodCol = 52;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			
			break;

		case BLOODHEAD:
			
			Entity->spriteset= BLOOD_HEAD_SPRITE_SET;
			Entity->lani	= 149;
			Entity->lstart	= 149;
			Entity->lend	= 152;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 50;
			Entity->BloodCol = 36;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 1;
			Entity->HitBox.bottom	= 19;
			
			break;

		case GBASLEEP:
			
			Entity->spriteset= -1;
			Entity->lani	= 79;
			Entity->lstart	= 79;
			Entity->lend	= 79;
			Entity->life	= 10;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 55;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			
			break;

		case ELEVATORDOWN:
			
			Entity->id		= ELEVATOR;
			Entity->spriteset= -1;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 0;
			Entity->life	= 500;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 22;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 39;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 39;
			Entity->count  = 0;
			Entity->count2 = 0;
			Entity->count3 = 2;
			
			break;

		case ELEVATORUP:
			
			Entity->id		= ELEVATOR;
			Entity->spriteset= -1;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 0;
			Entity->life	= 500;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 22;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 39;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 39;
			Entity->count  = 0;
			Entity->count2 = 0;
			Entity->count3 = -2;
			
			break;

		case CHOPPER:
			
			Entity->spriteset= MAC_SPRITE_SET;
			Entity->uani	= 30;
			Entity->ustart	= 30;
			Entity->uend	= 31;
			Entity->lani	= 32;
			Entity->lstart	= 32;
			Entity->lend	= 32;
			Entity->life	= 50;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 24;
			Entity->HitBox.left		= 3;
			Entity->HitBox.right	= 16;
			Entity->HitBox.top		= 3;
			Entity->HitBox.bottom	= 16;
			
			break;

		case GREENDOOR:			
			Entity->spriteset= DOORS_SPRITE_SET;
			Entity->lani	= 100;
			Entity->lstart	= 100;
			Entity->lend	= 100;
			Entity->life	= 1000;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 25;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->y += 5;
			break;
		case BLUEDOOR:			
			Entity->spriteset= DOORS_SPRITE_SET;
			Entity->lani	= 114;
			Entity->lstart	= 114;
			Entity->lend	= 114;
			Entity->life	= 1000;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 25;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->y += 5;
			break;
		case YELLOWDOOR:			
			Entity->spriteset= DOORS_SPRITE_SET;
			Entity->lani	= 128;
			Entity->lstart	= 128;
			Entity->lend	= 128;
			Entity->life	= 1000;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 25;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->y += 5;
			break;
		case REDDOOR:			
			Entity->spriteset= DOORS_SPRITE_SET;
			Entity->lani	= 142;
			Entity->lstart	= 142;
			Entity->lend	= 142;
			Entity->life	= 1000;
			Entity->dontfall= true;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 25;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->y += 5;
			break;

		case BOSS:
			
			Entity->BloodCol = 37;
			if(Level == lvFALL_EVENING){ Entity->id	= BOSS1; Entity->armor = 5.0f; Entity->BloodCol = 22;}
			if(Level == lvSHINY_FOREST){ Entity->id	= BOSS3; Entity->armor = 2.0f;}
			//if(Level == 6){	Entity->id	= BOSS3; Entity->armor = 5.0f; Entity->BloodCol = 70;}
			//if(Level == lvGOLDEN_ESTATE){	Entity->id	= BOSS4; Entity->armor = 2.0f; Entity->BloodCol = 42;}
			//if(Level == 9)	Entity->id	= BOSS5;
			//if(Level == 11)	Entity->id	= BOSS6;
			//if(Level == 12)	Entity->id	= BOSS7;
			if(Level == lvOTHER_WORLD){Entity->id	= LASTBOSS; Entity->armor = 10.0f; Entity->BloodCol = 98;}
			Entity->spriteset= BOSS_SPRITE_SET;
			Entity->uani	= 5;
			Entity->lani	= 0;
			Entity->lstart	= 0;
			Entity->lend	= 0;
			Entity->life	= 100;
			Entity->dontfall= true;
			Entity->ScoreValue = 1000;
			Entity->count   = 0;
			Entity->HitBox.top    =   0;
			Entity->HitBox.bottom =  59;
			Entity->HitBox.left   =   0;
			Entity->HitBox.right  =  99;
			Entity->y -= 60;

			BossXShift  = (int)Entity->x-(xRes/2);
			BossYShift  = (int)Entity->y-(yRes/2);
			if(BossYShift < 0) BossYShift = 0;
			if(BossXShift < 0) BossXShift = 0;
			
			
			Entity->visible = false;

			if(Entity->id == LASTBOSS){
				Entity->HitBox.top = -40;
				Entity->HitBox.bottom = 80;
				Entity->HitBox.left = -200;
				Entity->HitBox.right = 120;
				Entity->lani = 0;
				Entity->lstart = 0;
				Entity->lend = 0;
				Entity->uani = 0;
				Entity->visible = true;
				//Entity->y += 40;
				Boss = Entity;
				Entity->count = 0;
				Entity->count2 = 0;
				Entity->count3 = 0;
				Entity->speed = 0;
				Entity->setspeed = 0;
				Entity->maxspeed = 1;
				Entity->dontfall= false;				
			}
			if(Entity->id == BOSS3){
				Entity->y += 40;
				Entity->uani = 10;
				BossYShift += 40;
			}
			if(Entity->id == BOSS4){
				Entity->HitBox.top = 1;
				Entity->HitBox.bottom = 19;
				Entity->HitBox.left = 1;
				Entity->HitBox.right = 17;
				Entity->lani = 0;
				Entity->lstart = 0;
				Entity->lend = 5;
				Entity->uani = 6;
				Entity->visible = true;
				Entity->y += 60;
				Boss = Entity;
				Entity->speed = 0;
				Entity->setspeed = 0;
				Entity->maxspeed = 2;
				//LD3.LoadSprite(&WinApp, "gfx/badmark.put", BOSS_SPRITE_SET, 0);
				Entity->dontfall= false;
				BossXShift = (int)Entity->x-260;
				BossYShift = (int)Entity->y-180;
			}

			break;
			
		case BOMB:
			
			Entity->spriteset=YELLOW_FIRE_SPRITE_SET;
			Entity->uani	= 19;
			Entity->ustart	= 19;
			Entity->uend	= 19;
			Entity->lani	= 19;
			Entity->lstart	= 19;
			Entity->lend	= 19;
			Entity->life	= 5;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 22;
			Entity->HitBox.left		= 3;
			Entity->HitBox.right	= 15;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->visible = true;
			
			break;

		case DUD:
			
			Entity->spriteset=YELLOW_FIRE_SPRITE_SET;
			Entity->uani	= 19;
			Entity->ustart	= 19;
			Entity->uend	= 19;
			Entity->lani	= 19;
			Entity->lstart	= 19;
			Entity->lend	= 19;
			Entity->life	= 5;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->BloodCol = 22;
			Entity->HitBox.left		= 3;
			Entity->HitBox.right	= 15;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			Entity->visible = true;
			
			break;
				
		case FLETCHER:
			
			Entity->spriteset= FLETCHER_SPRITE_SET;
			Entity->lani	= 40;
			Entity->uani	= 49;
			Entity->lstart	= 40;
			Entity->lend	= 45;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->setspeed = 0;
			Entity->maxspeed = 2;
			Entity->speed = 0;
			Entity->BloodCol = 37;
			
				
			break;
			
		case MARK:
			
			Entity->spriteset= MARK_SPRITE_SET;
			Entity->lani	= 60;
			//Entity->uani	= 69;
			Entity->uani	= 67;
			Entity->lstart	= 60;
			Entity->lend	= 65;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->setspeed = 0;
			Entity->maxspeed = 1.7f;
			Entity->speed = 0;
			Entity->BloodCol = 37;
			
					
			break;

		case RUSTY:
			
			Entity->spriteset= RUSTY_SPRITE_SET;
			Entity->lani	= 80;
			Entity->uani	= 89;
			Entity->lstart	= 80;
			Entity->lend	= 85;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->setspeed = 0;
			Entity->maxspeed = 1.8f;
			Entity->speed = 0;
			Entity->BloodCol = 37;
			
					
			break;

		case GENERAL:
			
			Entity->spriteset= GENERAL_SPRITE_SET;
			Entity->lani	= 120;
			Entity->uani	= 129;
			Entity->lstart	= 120;
			Entity->lend	= 125;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->setspeed = 0;
			Entity->maxspeed = 2.0f;
			Entity->speed = 0;
			Entity->BloodCol = 37;
			
					
			break;

		case XAHN:
					
			Entity->spriteset= XAHN_SPRITE_SET;
			Entity->lani	= 100;
			Entity->uani	= 109;
			Entity->lstart	= 100;
			Entity->lend	= 104;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->ScoreValue = 0;
			Entity->setspeed = 0;
			Entity->maxspeed = 2;
			Entity->speed = 0;
			Entity->BloodCol = 37;
					
			break;
				
		case SWING:

			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->count2	= 0;
			Entity->setspeed= -2;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;					
			
			break;

		case SWING2:

			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->count2	= 180;
			Entity->setspeed= -2;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;					
			
			break;

		
		case SMASH_UP:

			Entity->id = SMASH;
			Entity->spriteset= -1;
			Entity->lani	= 42;
			Entity->lstart	= 42;
			Entity->lend	= 42;
			Entity->life	= 200;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= -1;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  59;
			
			break;
		
		case SMASH_DOWN:

			Entity->id = SMASH;
			Entity->spriteset= -1;
			Entity->lani	= 42;
			Entity->lstart	= 42;
			Entity->lend	= 42;
			Entity->life	= 200;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 180;
			Entity->setspeed= -1;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  59;
			
			break;
		
		case PLATFORM_Lr:

			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 200;
			Entity->count3	= Entity->startx-200;
			Entity->setspeed= -1;			
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;

		case PLATFORM_lR:

			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 200;
			Entity->count3	= Entity->startx+200;
			Entity->setspeed= 1;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;

		case PLATFORM_Ud:

			Entity->id		= PLATFORM_Ud;
			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 199;
			Entity->setspeed= -1;
			Entity->count3	= Entity->starty-200;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;

		case PLATFORM_uD:

			Entity->id		= PLATFORM_uD;
			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 199;
			Entity->count3	= Entity->starty+200;
			Entity->setspeed= 1;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;

		case PLATFORM_TRAP:

			Entity->spriteset= -1;
			Entity->lani	= 45;
			Entity->lstart	= 45;
			Entity->lend	= 45;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= 0;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;

		case ANCHOR:

			Entity->spriteset= -1;
			Entity->lani	= 50;
			Entity->lstart	= 50;
			Entity->lend	= 50;
			Entity->life	= 20;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= 0;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= -20;
			Entity->HitBox.right	=  39;
			Entity->HitBox.top		=   0;
			Entity->HitBox.bottom	=  19;
			
			break;		

		case SPIKEBALL1:

			Entity->spriteset= -1;
			Entity->lani	= SpikeBall;
			Entity->lstart	= SpikeBall;
			Entity->lend	= SpikeBall;
			Entity->life	= 200;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= -2;
			Entity->BloodCol= PlatformBlood;										
			Entity->count2 = 0;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 1;
			Entity->HitBox.bottom	= 18;					
			
			break;

		case SPIKEBALL2:

			Entity->spriteset= -1;
			Entity->lani	= SpikeBall;
			Entity->lstart	= SpikeBall;
			Entity->lend	= SpikeBall;
			Entity->life	= 200;
			Entity->dontfall= true;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= 2;
			Entity->BloodCol= PlatformBlood;										
			Entity->count2 = 0;
			Entity->HitBox.left		= 1;
			Entity->HitBox.right	= 18;
			Entity->HitBox.top		= 1;
			Entity->HitBox.bottom	= 18;					
			
			break;
		
		case GO_CART:

			Entity->spriteset= -1;
			Entity->lani	= 80;
			Entity->lstart	= 80;
			Entity->lend	= 80;
			Entity->life	= 20;
			Entity->dontfall= false;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= 0;
			Entity->BloodCol= PlatformBlood;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 8;
			Entity->HitBox.bottom	= 19;
			break;

		case SNOWMOBILE:

			Entity->spriteset= -1;
			Entity->lani	= 80;
			Entity->lstart	= 80;
			Entity->lend	= 80;
			Entity->life	= 100;
			Entity->dontfall= false;
			Entity->xFlip	= false;
			Entity->count	= 0;
			Entity->setspeed= 0;
			Entity->BloodCol= 74;
			Entity->HitBox.left		= 0;
			Entity->HitBox.right	= 19;
			Entity->HitBox.top		= 0;
			Entity->HitBox.bottom	= 19;
			break;
	}	
}

//==============================================================================================================
//= void FireProjectile(float x, float y, float angle, float speed, int PreDefinedWeapon, ENTITY *owner, int dy)
//= Fire a projectile
//= x, y			- the starting coordinates of the projectile
//= angle			- the angle at which the projectile will travel
//= speed			- the speed at which the projectile will travel
//= PreDefinedWeapon- If a valid predefinedweapon is given, certain properties of that projectile will be
//=					- assigned based on the code of the predefinedweapon
//=	dy				- this makes the projectile shift along the y axis based on the number given
//=					- the projectile's original y coordinate will not be effected when during collision
//=					- detection on the map
//=					- this helps to have the projectile moved down or up to match the object it is fired out of
//==============================================================================================================
void FireProjectile(float x, float y, int angle, float speed, int PreDefinedWeapon, ENTITY *owner, int dy)
{
	PROJECTILE *p;
	int  range = 25;
	int  mx, my, mp, mp2;
	long mpt;
	float ca = Cosine[angle];
	float sa = Sine[angle];
	bool hit = false;
	int	  left, right, top, bottom;
	float ca2;
	float sa2;
	float ca3;
	float sa3;
	float x2; float y2;
	float x3; float y3;
	bool hit2;
	bool hit3;
	
	switch(PreDefinedWeapon)
	{
	case SONIC_WAVE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= SONIC_WAVE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = 15;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = 22;
		p->anicount	    = 0.1f;
		p->spriteset    = SONIC_MONSTER_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0.0f;
		p->xflip	    = owner->xFlip;
		p->delafterlast = true;
		p->del			= false;
		p->dy			= dy;
		p->power		= 5;
		p->HitBox.top	= 2;
		p->HitBox.bottom= 16;
		p->HitBox.left  = 4;
		p->HitBox.right = 14;
		break;
	case BOMB:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= BOMB;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = true;
		p->explode	    = 1;
		p->radius	    = 0;
		p->firstsprite  = 19;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = 19;
		p->anicount	    = 0.0f;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0.0f;
		p->xflip	    = false;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 0;
		p->HitBox.top	= 1;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 3;
		p->HitBox.right = 15;
		break;
	case YELLOW_FIRE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= YELLOW_FIRE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		if(angle >= 0 && angle < 45)         p->firstsprite  = YELLOW_FIRE_RIGHT;
		else if(angle >=  45 && angle < 135) p->firstsprite  = YELLOW_FIRE_UP;
		else if(angle >= 135 && angle < 225) p->firstsprite  = YELLOW_FIRE_LEFT;
		else if(angle >= 225 && angle < 315) p->firstsprite  = YELLOW_FIRE_DOWN;
		else if(angle >= 315 && angle < 405) p->firstsprite  = YELLOW_FIRE_RIGHT;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 20;
		p->HitBox.top	= 4;
		p->HitBox.bottom= 15;
		p->HitBox.left  = 4;
		p->HitBox.right = 15;
		break;
	case TOXIC_BUBBLE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= TOXIC_BUBBLE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = TOXIC_BUBBLE;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 3;
		p->HitBox.top	= 7;
		p->HitBox.bottom= 14;
		p->HitBox.left  = 6;
		p->HitBox.right = 13;
		break;
	case LOOGIE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= LOOGIE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = true;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = LOOGIE;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = GREEN_BLOB_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 10;
		p->HitBox.top	= 7;
		p->HitBox.bottom= 14;
		p->HitBox.left  = 6;
		p->HitBox.right = 13;
		break;
	case COTTONSPIKE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= COTTONSPIKE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = COTTON_SPIKE;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 20;
		p->HitBox.top	= 7;
		p->HitBox.bottom= 14;
		p->HitBox.left  = 6;
		p->HitBox.right = 13;
		break;
	case GRENADE:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= GRENADE;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = true;
		p->explode	    = true;
		p->radius	    = 0;
		p->firstsprite  = GRENADE_SPRITE;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 0;
		p->HitBox.top	= 8;
		p->HitBox.bottom= 11;
		p->HitBox.left  = 6;
		p->HitBox.right = 15;
		break;
	case BALL:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= BALL;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = 3;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = 3;
		p->anicount	    = 0;
		p->spriteset    = BOSS_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0.0f;
		p->xflip	    = false;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 20;
		p->HitBox.top	= 0;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 0;
		p->HitBox.right = 19;
		break;
	case SPIKEBALL:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= SPIKEBALL;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = true;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite  = SpikeBall;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = SpikeBall;
		p->anicount	    = 0;
		p->spriteset    = -1;
		p->timer	    = 0;
		p->yvelocity    = 0.0f;
		p->xflip	    = false;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 20;
		p->HitBox.top	= 0;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 0;
		p->HitBox.right = 19;
		break;
	case FALLING_OBJECT:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= FALLING_OBJECT;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite	= 5;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 55;
		p->HitBox.top	= 0;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 0;
		p->HitBox.right = 19;
		break;

	case FALLING_GROUND:
		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= FALLING_GROUND;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 0;
		p->radius	    = 0;
		p->firstsprite	= dy;
		p->spritenum    = (float)p->firstsprite;
		p->lastsprite   = p->firstsprite;
		p->anicount	    = 0;
		p->spriteset    = -1;//YELLOW_FIRE_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= 0;
		p->power		= 0;
		p->HitBox.top	= 1;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 0;
		p->HitBox.right = 19;
		break;

	case MACHINEGUN:
		
		ca *= 20; sa *= 20;
		
		while(range > 0){
			x += ca;
			y += sa;
			mx  = (int)x / 20;
			my  = (int)y / 20;
			mpt = mx+(my<<MAPXLSHIFT)+55000;
		
			mp  = LD3.Map[mpt];
			mp2 = LD3.Map[mpt+MAPWIDTH];
			if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40)) break;
			for(int i = 0; i <= NumMonsters; i++){
				left	= (int)Monster[i].x + Monster[i].HitBox.left;
				right	= (int)Monster[i].x + Monster[i].HitBox.right;
				top		= (int)Monster[i].y + Monster[i].HitBox.top;
				bottom	= (int)Monster[i].y + Monster[i].HitBox.bottom;
				if(x >= left && x <= right && y >= top && y <= bottom){
					if(owner->id == LARRY){
						Splatter(int(Monster[i].x+9), (int)y, 40, -1.5, Monster[i].BloodCol);
						Monster[i].life		-= int(WeaponPower/Monster[i].armor);;
						Monster[i].aiState	 = aiANGRY;
						//Monster[i].target	 = owner->id;
						if(Monster[i].life <= WeaponPower) if((rand()%50/WeaponPower) == 1) Monster[i].aiState = aiSCARED;
					}
					else if(owner->id == CHOPPER){
						Splatter(int(Monster[i].x+9), (int)y, 40, -1.5, Monster[i].BloodCol);
						Monster[i].life		-= int(5/Monster[i].armor);;						
					}
					else{
						if(owner->id != Monster[i].id){
							Monster[i].life	-= 2;
							Splatter(int(Monster[i].x+9), (int)y, 40, -1.5, Monster[i].BloodCol);
						}
					}
					hit = true;
					break;
				}
			}
			if(hit) break;
			range--;
		}
		break;

	case SHOTGUN:
		
		ca *= 10; sa *= 10;
		ca2 = Cosine[angle-10]*10;
		sa2 = Sine[angle-10]*10;	
		ca3 = Cosine[angle+10]*10;
		sa3 = Sine[angle+10]*10;
		x2 = x; y2 = y;
		x3 = x; y3 = y;
		hit2 = false;
		hit3 = false;

		while(range > 0){
			x += ca; x2 += ca2; x3 += ca3;
			y += sa; y2 += sa2; y3 += sa3;
			//mx  = (int)x / 20;
			//my  = (int)y / 20;
			//mpt = mx+(my<<MAPXLSHIFT)+55000;
		
			//mp  = LD3.Map[mpt];
			//mp2 = LD3.Map[mpt+MAPWIDTH];
			//if((mp > 9 && mp < 40) || (mp >= 40 && mp2 > 9 && mp2 < 40)) break;
			for(int i = 1; i <= NumMonsters; i++){
				if(Monster[i].visible == true){
					left	= (int)Monster[i].x + Monster[i].HitBox.left;
					right	= (int)Monster[i].x + Monster[i].HitBox.right;
					top		= (int)Monster[i].y + Monster[i].HitBox.top;
					bottom	= (int)Monster[i].y + Monster[i].HitBox.bottom;
					if((x >= left && x <= right && y >= top && y <= bottom && hit == false) || (x2 >= left && x2 <= right && y2 >= top && y2 <= bottom && hit2 == false) || (x3 >= left && x3 <= right && y3 >= top && y3 <= bottom && hit3 == false)){
						//Splatter(Monster[i].x+10, (int)y, 40, ca);
						if(owner->id == LARRY){
							//gif(Monster[i].aiState != aiANGRY && Monster[i].id == VROCK_MONSTER) FSOUND_PlaySound(FSOUND_FREE,sndRock[1]);
							Monster[i].aiState	 = aiANGRY;
							//Monster[i].target	 = owner->
							if(Monster[i].life <= WeaponPower) if((rand()%50/WeaponPower) == 1)	Monster[i].aiState = aiSCARED;							
						}					
					}
					if(hit == false && x >= left && x <= right && y >= top && y <= bottom){
						Splatter((int)x, (int)y, 40, ca/8, Monster[i].BloodCol);
						Monster[i].life -= int(WeaponPower/Monster[i].armor);
						hit = true;
					}
					if(hit2 == false && x2 >= left && x2 <= right && y2 >= top && y2 <= bottom){
						Splatter((int)x, (int)y2, 40, ca2/8, Monster[i].BloodCol);
						Monster[i].life -= int(WeaponPower/Monster[i].armor);
						hit2 = true;
					}
					if(hit3 == false && x3 >= left && x3 <= right && y3 >= top && y3 <= bottom){
						Splatter((int)x, (int)y3, 40, ca3/8, Monster[i].BloodCol);
						Monster[i].life -= int(WeaponPower/Monster[i].armor);
						hit3 = true;
					}
					if(hit && hit2 && hit3) break;
				}
			}
			if(hit && hit2 && hit3) break;
			range--;
		}
		break;

	case SINGULARITY:

		NumProjectiles++;
		p = &Projectile[NumProjectiles];

		p->id			= SINGULARITY;
		p->x		    = x;
		p->y		    = y;
		p->xspeed	    =  Cosine[angle]*speed;
		p->yspeed	    = -Sine[angle]*speed;
		p->falling	    = false;
		p->explode	    = 1;
		p->radius	    = 0;
		p->firstsprite	= 43;
		p->spritenum    = 43;
		p->lastsprite   = 43;
		p->anicount	    = 0;
		p->spriteset    = LARRY_SPRITE_SET;
		p->timer	    = 0;
		p->yvelocity    = 0;
		p->xflip	    = false;//owner->xFlip;
		p->delafterlast = false;
		p->del			= false;
		p->dy			= dy;
		p->power		= 0;
		p->HitBox.top	= 0;
		p->HitBox.bottom= 19;
		p->HitBox.left  = 0;
		p->HitBox.right = 19;
		break;
	}
}

//==============================================================================================================
//= void CreateExplosion(int x, int y)
//= Creates and explosion
//==============================================================================================================
void CreateExplosion(int x, int y)
{
	NumExplosions++;
	long combo = 0;
	long ComboScore = 0;
	char Text1[80], Text2[80];

	ENTITY *m;
	LOCATION *e = &Explosion[NumExplosions];
	int xdist, ydist, dist, radius;

	e->x = x;
	e->y = y;
	e->l = 0;

	for(int i = 1; i <= NumMonsters; i++){
		m = &Monster[i];

		xdist = abs(int((m->x+9) - e->x));
		ydist = abs(int((m->y+9) - e->y));
		dist = (int)sqrt(pow(xdist,2)+pow(ydist,2));

		radius = m->HitBox.right - m->HitBox.left;
		radius >>= 1;
		radius += 40;

		if(dist < radius){
			m->life -= (radius-dist)/m->armor;
			m->speed += ((m->x+9) - e->x)/10;
			m->velocity += ((m->y+9) - e->y)/10;
			//Splatter(int(m->x+9), int(m->y+9), dist<<1, float((int)m->speed>>2), m->BloodCol);
			if(m->life <= 0 && m->ScoreValue > 0){
				combo ++;
				if(combo >= 2) ComboScore += long(m->ScoreValue*pow(combo, 2));
			}
		}
	}

	if(combo >= 2){
		memset(Text1, 0, 80);
		// TODO(davidgow): Get rid of Text2 and use snprintf() or similar.
		memset(Text2, 0, 80);
		if(combo == 2) sprintf(Text1, "COMBO!  +%ld", ComboScore);
		if(combo == 3) sprintf(Text1, "TRIPLE COMBO!!  +%ld", ComboScore);
		if(combo == 4) sprintf(Text1, "QUAD COMBO!!!  +%ld", ComboScore);
		if(combo == 5) sprintf(Text1, "MONSTER COMBO!!!!  +%ld", ComboScore);
		if(combo > 5 && combo < 10) sprintf(Text1, "FREAKIN' MONSTER COMBO!!!!  +%ld", ComboScore);
		if(combo >= 10) sprintf(Text1, "SUPER FREAKIN' MONSTER COMBO!!!!!  +%ld", ComboScore);

		WriteMessage(Text1);

		Score += ComboScore;
		
	}

	xdist = abs(int((Player->x+9) - e->x));
	ydist = abs(int((Player->y+9) - e->y));
	dist = int(sqrt(pow(xdist,2)+pow(ydist,2)));

	if(dist < 50){
		Player->life -= (50-dist);		
		Player->speed += ((Player->x+9) - e->x)/10;
		Player->velocity += ((Player->y+9) - e->y)/10;
		//Splatter(int(Player->x+9), int(Player->y+9), dist<<1, float((int)Player->speed>>2), Player->BloodCol);
	}

	/*if(dist < 50){
		Player->life -= (50-dist);
		
		dist = ((Player->x+9) - e->x)/10;

		if(dist < 0) Player->speed += -50-dist; else Player->speed += 50-dist;

		dist = ((Player->y+9) - e->y)/10;
		if(dist < 0) Player->velocity += -50-dist; Player->velocity += 50-dist;
		//Splatter(int(Player->x+9), int(Player->y+9), dist<<1, float((int)Player->speed>>2), Player->BloodCol);
	}*/

	//Splatter(x, y, 20, 0, 88);
	Splatter(x-10, y-10, 20, -1, 88);
	Splatter(x+10, y+5, 20, 1, 88);

	FSOUND_PlaySound(FSOUND_FREE,sndBoom);
}

//==============================================================================================================
//= void ProcessExplosions(int x, int y)
//= Draw and animate the explosions
//==============================================================================================================
void ProcessExplosions()
{
	LOCATION *e;
	int ne, de;

	de = 0;
	
	for(int i = 1; i <= NumExplosions; i++)
	{
		e = &Explosion[i];
		ne = (e->l>>2)<<2;
		
		for(int y = -20; y <= 0; y += 20){
			for(int x = -20; x <= 0; x += 20){
				if(e->x-xShift+x >= -20 && e->x-xShift+x <= xRes && e->y-yShift+y >= -20 && e->y-yShift+y <= yRes)
				LD3.PutSprite(&WinApp, int(e->x-xShift+x), int(e->y-yShift+y), EXPLOSION_SPRITE_SET, EXPLOSION_START+ne, false);
				ne++;
			}
		}
		e->l ++;

		if(e->l >= 31) de ++;
	}
	if(de) for(int i = de+1; i <= NumExplosions; i++)	Explosion[i-de] = Explosion[i];
	NumExplosions -= de;	
}


//==============================================================================================================
//= void ProcessProjectiles()
//= Draw and process the projectiles
//==============================================================================================================
void ProcessProjectiles()
{
	//- Draw and process the projectiles
	PROJECTILE *p;
	ENTITY     *m;
	int  mp, mp2;
	int  mx, my;
	long mpt;
	int  left, right, top, bottom;
	float xdist, ydist;
	float xspeed, yspeed;

	
	for(int i = 1; i <= NumProjectiles; i++)
	{
		p = &Projectile[i];	
	
		xspeed = p->xspeed*diff;
		yspeed = p->yspeed*diff;

		p->x += xspeed;
		p->y += yspeed;
		p->spritenum += p->anicount*diff;

		if(p->id == TOXIC_BUBBLE){
			p->xspeed += (1.5f-float(rand()%4))*0.01f*diff;
			p->yspeed -= 0.01f*diff;
		}

		left  = p->HitBox.left;
		right = p->HitBox.right;
		top   = p->HitBox.top;
		bottom= p->HitBox.bottom;

		if(p->id == SINGULARITY){
			for(int i = 1; i <= NumMonsters; i++){
				m = &Monster[i];
				if(m->ScoreValue){
					xdist = pow(fabs(m->x-p->x),2);
					ydist = pow(fabs(m->y-p->y),2);
					if(xdist < 250000 && ydist < 250000){
						xdist = (250000-xdist)*0.000007f;//pow((500-xdist), 2);
						ydist = (250000-ydist)*0.000007f;//pow((500-ydist), 2);
						if(m->x < p->x) m->x += xdist; else m->x -= xdist;
						if(m->y < p->y) m->y += ydist; else m->y -= ydist;
						m->velocity = 0;
					}
				}
			}
		}

		if(p->id == FALLING_OBJECT || p->id == FALLING_GROUND){
			mpt = ((int)((p->x+10)/20))+(((int)(p->y-20)/20)<<9)+55000;
			for(int y = 0; y <= MAPWIDTH; y += MAPWIDTH){
				mp  = LD3.Map[mpt+y]; mp2 = LD3.Map[mpt+y+MAPWIDTH];
				if(mp > 9 && mp2 > 9) p->del = true;
						
			}
		}	
		else{
			//mpt = ((int)(p->x/20))+((int)(p->y)/20)*MAPWIDTH+55000;
			for(int y = top; y <= bottom; y += bottom-top){
				for(int x = left; x <= right; x += right-left){
					mx  = ((int)p->x+x) / 20;
					my  = ((int)p->y+y-5) / 20;
					mpt = mx+(my<<MAPXLSHIFT)+55000;
					mp  = LD3.Map[mpt]; mp2 = LD3.Map[mpt+MAPWIDTH];
					if((mp > 9 && mp < 40) || (mp > 9 && mp < 256 && p->yspeed > 0) || (mp >= 40 && mp < 256 && mp2 > 9 && mp2 < 40)){
						if(p->id == SPIKEBALL || p->id == BALL){
							p->yspeed = -p->yspeed/2;
							p->yvelocity = 0;
							p->y += p->yspeed*diff;
							p->y += p->yspeed*diff;
						}
						else
							p->del = true;
					}
					//if(mp > 9) p->del = true
				}
			}
		}


		if(p->spritenum > p->lastsprite){
			if(p->delafterlast)
				p->del = true;
			else
			p->spritenum = (float)p->firstsprite;
		}

		if(!p->delafterlast){
			p->timer -= 1;
			if(p->timer <= 0) p->delafterlast = true;
		}
		
		if(p->falling){
			p->yspeed    += p->yvelocity*diff;
			p->yvelocity += 0.009f*diff;
		}

		if(p->x-xShift >= -20 && p->x-xShift <= xRes && p->y-yShift >= -20 && p->y-yShift <= yRes){
			LD3.PutSprite(&WinApp, int(p->x-xShift), int(p->y+p->dy-yShift), p->spriteset, int(p->spritenum+p->anicount), p->xflip);
		}
		else if(p->id == SPIKEBALL){
			p->xspeed = -p->xspeed;
		}

		if(p->power > 0 && p->id != GRENADE){
			int ldx = (int)Player->x;
			int ldy = (int)Player->y;
			int px  = (int)p->x;
			int py  = int(p->y+p->dy);
			if(Player->life > 0 && px+10 >= ldx && px+10 <= ldx+19 && py+10 >= ldy && py+10 <= ldy+19){
				p->del = true;
				Player->life -= p->power;
				if(Player->life <= 0){
					Player->life = 0;
					Splatter((int)Player->x+10, (int)Player->y+10, 200, 0, Player->BloodCol);
				}
				else
					Splatter((int)Player->x+10, (int)Player->y+10, 20, 0, Player->BloodCol);
			}
			if(p->id == SPIKEBALL && Boss->life > 0){
				int bx = (int)Boss->x;
				int by = (int)Boss->y;
				int top    = Boss->HitBox.top;
				int bottom = Boss->HitBox.bottom;
				int left   = Boss->HitBox.left;
				int right  = Boss->HitBox.right;
				if(px+10 >= bx+left && px+10 <= bx+right && py+10 >= by+top && py+10 <= by+bottom){
					Splatter((int)p->x+9, (int)p->y+6, 40, 0, Boss->BloodCol);
					Boss->life -= (float)p->power/Boss->armor;
					if(Boss->life <= 0){ Boss->count = 0; Boss->count2 = 0;}
					p->del = true;
				}
			}
		}
	}

	int i = 1;
	while(i <= NumProjectiles)
	{
		p = &Projectile[i];
		if(p->del || p->x-xShift < -200 || p->y-yShift < -200 || p->x-xShift >= xRes+200 || p->y-yShift >= yRes+200){			
			if(p->explode) CreateExplosion(int(p->x+9), int(p->y+p->dy+9));
			for(int n = i; n <= NumProjectiles-1; n++){
				Projectile[n] = Projectile[n+1]; 				
			}
			NumProjectiles--;
		}
		else
			i++;
	}
}

//==============================================================================================================
//= void WriteText(int x, int y, HFONT *Font, const char* Text, DWORD col, UINT format)
//= This function writes the message given to the video buffer
//= x, y	- the upper-left coordinate of where the text will be place on the screen
//= *Text	- a pointer to the character buffer holding the message
//= col		- the color of the text
//= format	- a UNIT structure that specifies how the text is placed on the screen
//=			- DT_CENTER centers the text
//=			- DT_RIGHT  right justifies the text
//==============================================================================================================
void WriteText(int x, int y, const char* Text, DWORD col, UINT format)
{
	RECT	FontRect;
	//HDC		xdc;

	FontRect.left   = x;
	FontRect.top    = y;
	FontRect.right  = xRes;
	FontRect.bottom = yRes;
	bool center = false;

	char Text1[80];

	strcpy(Text1, Text);
	SDL_strupr(Text1);

	if(format == DT_CENTER) center = true;
	if(format == DT_RIGHT) x = xRes-strlen(Text1)*7;
	//LD3.WriteText(&WinApp, x, y, Text1, center, true);
	LD3.WriteText(&WinApp, x+1, y+1, Text1, center, 0, true);
	LD3.WriteText(&WinApp, x, y, Text1, center, col, true);
	
	/*WinApp.lpddsback->GetDC(&xdc);
		SetBkMode(xdc, TRANSPARENT);
		SelectObject(xdc, Font);
		SetTextColor(xdc, col);
		DrawText(xdc, Text, -1, &FontRect, format);
	WinApp.lpddsback->ReleaseDC(xdc);*/
}

void WriteText(int x, int y, char* Text, DWORD col, UINT format, bool Scroll)
{
	RECT	FontRect;
	//HDC		xdc;

	FontRect.left   = x;
	FontRect.top    = y;
	FontRect.right  = xRes;
	FontRect.bottom = yRes;
	bool center = false;

	char Text1[80];

	strcpy(Text1, Text);
	SDL_strupr(Text1);

	if(format == DT_CENTER) center = true;
	if(format == DT_RIGHT) x = xRes-strlen(Text1)*7;
	//LD3.WriteText(&WinApp, x, y, Text1, center, true);
	LD3.WriteText(&WinApp, x+1, y+1, Text1, center, 0, true);
	LD3.WriteText(&WinApp, x, y, Text1, center, col, true);

	/*WinApp.lpddsback->GetDC(&xdc);
		SetBkMode(xdc, TRANSPARENT);
		SelectObject(xdc, Font);
		SetTextColor(xdc, col);
		DrawText(xdc, Text, -1, &FontRect, format);
	WinApp.lpddsback->ReleaseDC(xdc);*/
}

//==============================================================================================================
//= void DeactivateLasers()
//= Delete the lasers from the map
//==============================================================================================================
void DeactivateLasers()
{
	int  x, y, l;
	long mpt;
	for(int i = 1; i <= NumLasers; i++)
	{
		x = Laser[i].x;
		y = Laser[i].y;
		l = Laser[i].l;
		mpt = x+y*MAPWIDTH;
		LD3.Map[mpt+55000] = -LD3.Map[mpt+55000];
		LD3.Map[mpt+165000] = 1;
	}
}

//==============================================================================================================
//= void TurnOffWalls()
//= This is called when Larry steps on a touchplate, it deletes a certain type of wall from the map
//= Once the walls are turned off, check if any boulder or falling object is above it, if it is, activate it
//==============================================================================================================
void TurnOffWalls()
{
	int  x, y;
	long mpt;
	for(int i = 1; i <= NumWalls; i++)
	{
		x = Wall[i].x;
		y = Wall[i].y;
		mpt = x+y*MAPWIDTH+55000;
		LD3.Map[mpt] = -LD3.Map[mpt];

		long highest = mpt-MAPWIDTH*30;
		int  y2 = y;
		if(highest < 50000) highest = 50000+x;
		for(long mptc = mpt; mptc >= highest; mptc -= MAPWIDTH){
			int mp = LD3.Map[mptc];
		
			if(mp > 0 && mp < 162){
				if(mp == miFallingObject){
					LD3.Map[mptc] = -LD3.Map[mptc];
					FireProjectile((float)x*20, (float)y2*20, 270, 2, FALLING_OBJECT, &NullEntity, 0);
				}
				else if(mp == 77){
					LD3.Map[mptc] = -LD3.Map[mptc];
					NumMonsters++;
					int nm = NumMonsters;

					Monster[nm].x		= (float)x*20;
					Monster[nm].y		= (float)y2*20;
					Monster[nm].ox		= Monster[nm].x;
					Monster[nm].oy		= Monster[nm].y;
					Monster[nm].id		= BOULDER;
					Monster[nm].spriteset= -1;
					Monster[nm].lani	= 77;
					Monster[nm].lstart	= 77;
					Monster[nm].lend	= 78;
					Monster[nm].life	= 200;
					Monster[nm].dontfall= false;
					Monster[nm].xFlip	= false;
					Monster[nm].count	= 0;
					Monster[nm].setspeed= 0;
					Monster[nm].velocity= 0;
				}
				else if(mp == 78){
					LD3.Map[mptc] = -LD3.Map[mptc];
					NumMonsters++;
					int nm = NumMonsters;

					Monster[nm].x		= (float)x*20;
					Monster[nm].y		= (float)y2*20;
					Monster[nm].ox		= Monster[nm].x;
					Monster[nm].oy		= Monster[nm].y;
					Monster[nm].id		= BOULDER;
					Monster[nm].spriteset= -1;
					Monster[nm].lani	= 78;
					Monster[nm].lstart	= 78;
					Monster[nm].lend	= 78;
					Monster[nm].life	= 200;
					Monster[nm].dontfall= false;
					Monster[nm].xFlip	= false;
					Monster[nm].count	= 0;
					Monster[nm].setspeed= 0;
					Monster[nm].velocity= 0;
				}
				else
					break;
			}
			y2 -= 1;
		}
	}
	SteppedOnTouchPlate = true;
}

//==============================================================================================================
//= void WriteMessage(const char *msg)
//= Copy the given message to the global system message that displays on the screen
//= Set the system message timer to enough time for the player to read the message
//==============================================================================================================
void WriteMessage(const char *msg)
{
	strcpy(SysMsg, msg);
	SysMsgCount = 500;	
}

//==============================================================================================================
//= void CheckForItems()
//= Check if Larry is in contact with an item and process what that item does
//==============================================================================================================
void CheckForItems()
{
	//- Check if Larry is in contact with an item
	int mx, my, mx2, mp, mp2, score, nm;
	int tx, ty;
	long mpt;
	bool playsound = false;
	bool incurrent = false;
	bool isUnderWater = false;
	char text[20];
	mx  = (int)(Player->x+9) / 20;
	mx2 = (int)(Player->x+15) / 20;
	my = (int)(Player->y+9-4) / 20;
	mpt = mx+(my<<MAPXLSHIFT)+55000;
	mp = LD3.Map[mpt];	
	//if(mp > 200) playsound = true;
	score = ScoreMap[mx][my];
		
	switch(mp)
	{
	case miL:
		LetterBonus.L	= true;
		ScoreDisplay	+= score;
		GotBonus		= true;
		LD3.Map[mpt]	= 0;
		BonusDisplayTimer = 200;
		playsound		= true;
		LD3.Map[mpt+110000] = 1;
		break;
	case miA:
		LetterBonus.A	= true;
		ScoreDisplay	+= score;
		GotBonus		= true;
		LD3.Map[mpt]	= 0;
		BonusDisplayTimer = 200;
		playsound		= true;
		LD3.Map[mpt+110000] = 1;
		break;
	case miR1:
		LetterBonus.R1	= true;
		ScoreDisplay	+= score;
		GotBonus		= true;
		LD3.Map[mpt]	= 0;
		BonusDisplayTimer = 200;
		playsound		= true;
		LD3.Map[mpt+110000] = 1;
		break;
	case miR2:
		LetterBonus.R2	= true;
		ScoreDisplay	+= score;
		GotBonus		= true;
		LD3.Map[mpt]	= 0;
		BonusDisplayTimer = 200;
		playsound		= true;
		LD3.Map[mpt+110000] = 1;
		break;
	case miY:
		LetterBonus.Y	= true;
		ScoreDisplay	+= score;
		GotBonus		= true;
		LD3.Map[mpt]	= 0;
		BonusDisplayTimer = 200;
		playsound		= true;
		LD3.Map[mpt+110000] = 1;
		break;
	case miHealthBottle:
		if(Player->life < MaxLife){
			Player->life	+= HealthAdd1;
			LD3.Map[mpt]	= -LD3.Map[mpt];
			LD3.Map[mpt+110000] = 1;
			playsound		= true;
		}
		break;
	case miHealthBottle2X:
		if(Player->life < MaxLife){
			Player->life	+= HealthAdd2;
			LD3.Map[mpt]	= -LD3.Map[mpt];
			LD3.Map[mpt+110000] = 1;
			playsound		= true;
		}
		break;
	case miGreenCard:
		LD3.Map[mpt]	= -LD3.Map[mpt];
		LD3.Map[mpt+110000] = 1;		
		WriteMessage("You Got The Green KeyCard");
		HasGreenCard	= true;		
		playsound		= true;
		break;
	case miBlueCard:
		LD3.Map[mpt]	= -LD3.Map[mpt];
		LD3.Map[mpt+110000] = 1;		
		WriteMessage("You Got The Blue KeyCard");
		HasBlueCard		= true;		
		playsound		= true;
		break;
	case miYellowCard:
		LD3.Map[mpt]	= -LD3.Map[mpt];
		LD3.Map[mpt+110000] = 1;		
		WriteMessage("You Got The Yellow KeyCard");
		HasYellowCard	= true;		
		playsound		= true;
		break;
	case miRedCard:
		LD3.Map[mpt]	= -LD3.Map[mpt];
		LD3.Map[mpt+110000] = 1;		
		WriteMessage("You Got The Red KeyCard");
		HasRedCard		= true;		
		playsound		= true;
		break;
	case miSpeedBottle:
		Player->maxspeed  += 1.0f;
		SpeedTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		WriteMessage("S P E E D");
		FMUSIC_SetMasterSpeed(song1, 1.5f);
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miSpeedBottle2X:
		Player->maxspeed  += 2.0f;
		SpeedTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
	  	WriteMessage("S U P E R   S P E E D");
		FMUSIC_SetMasterSpeed(song1, 2.0f);
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miPowerBottle:
		//WeaponPower		= OldWeaponPower;
		OldWeaponPower	= WeaponPower;
		WeaponPower		*= 2;
		PowerTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];		
		WriteMessage("P O W E R !");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miPowerBottle2X:
		//WeaponPower		= OldWeaponPower;
		OldWeaponPower  = WeaponPower;
		WeaponPower		*= 4;
		PowerTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];		
		WriteMessage("M E G A   P O W E R !");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miJumpBottle:
		JumpVel			= oldJumpVel;
		oldJumpVel		= JumpVel;
		JumpVel			*= 1.5;
		JumpBTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		WriteMessage("M E G A   J U M P");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miJumpBottle2X:
		JumpVel			= oldJumpVel;
		oldJumpVel		= JumpVel;
		JumpVel			*= 2;
		JumpBTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		WriteMessage("G I G A  J U M P");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miAllInOneBottle:
		Player->life		+= 50;
		Player->maxspeed  += 1.0f;
		WeaponPower		= OldWeaponPower;
		OldWeaponPower	= WeaponPower;
		WeaponPower		*= 2;
		JumpVel			= oldJumpVel;
		oldJumpVel		= JumpVel;
		JumpVel			*= 1.5;
		SpeedTimer		= 1500;
		PowerTimer		= 1500;
		JumpBTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		WriteMessage("#@$%!");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miAllInOneBottle2X:
		Player->life		+= 100;
		Player->maxspeed  += 2.0f;
		WeaponPower		= OldWeaponPower;
		OldWeaponPower  = WeaponPower;
		WeaponPower		*= 3;
		JumpVel			= oldJumpVel;
		oldJumpVel		= JumpVel;
		JumpVel			*= 2;
		SpeedTimer		= 1500;
		PowerTimer		= 1500;
		JumpBTimer		= 1500;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		WriteMessage("#@$%! #@$%! #@$%!");
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miEndFlag:
		//Score			+= TimeBonus;
		LD3.Map[mpt]	= -LD3.Map[mpt];
		EndOfLevel		= true;
		EndOfLevelTimer = 1000;
		playsound		= true;
		//FMUSIC_StopSong(song1);
		//FMUSIC_PlaySong(mscLevelComplete);
		break;
	case miMakerFlag:
		LD3.Map[mpt]	= miMakerFlag+1;
		Marker.x = mx*20;
		Marker.y = my*20;
		RemoveEntities();
		playsound = true;
		break;
	case miTaggedFlag:
		playsound = false;
		break;
	case miExtraLife:
		LD3.Map[mpt]	= 0;
		Lives			+= 1;		
		FSOUND_PlaySound(FSOUND_FREE,diescum);//sndItemPickup);
		//FSOUND_StopSound(7);
		//FSOUND_PlaySound(7,diescum);
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miShotgun:
		LD3.Map[mpt]	= 0;
		HasShotgun		= true;
		WriteMessage("You Got The Shotgun");
		Shells			+= 30;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miRifle:
		LD3.Map[mpt]	= 0;
		HasMachinegun	= true;
		WriteMessage("You Got The Assault Rifle");
		Bullets			+= 30;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miMagazine:
		LD3.Map[mpt]	= 0;
		Bullets			+= 30;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miSingularity:
		LD3.Map[mpt]	= 0;
		HasMachinegun	= true;
		WriteMessage("You Got The Singulariy Discharge Rifle!!!");
		Singularities	+= 2;
		FSOUND_PlaySound(FSOUND_FREE,diescum);
		FSOUND_PlaySound(FSOUND_FREE,diescum);
		FSOUND_PlaySound(FSOUND_FREE,diescum);
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miSingularityAmmo:
		LD3.Map[mpt]	= 0;
		Singularities   += 1;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miBoxOfShells:
		LD3.Map[mpt]	= 0;
		Shells			+= 6;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;
	case miChopper:
		LD3.Map[mpt]	= 0;
		Choppers		+= 1;
		LD3.Map[mpt+110000] = 1;
		WriteMessage("You Got A Mini Assault Chopper");
		playsound		= true;
		break;
	case miGreenCrystal:
		LD3.Map[mpt]	= 0;
		LD3.Map[mpt+110000] = 1;
		CrystalCount++;
		playsound		= true;
		break;
	case miYellowCrystal:
		LD3.Map[mpt]	= 0;
		LD3.Map[mpt+110000] = 1;
		CrystalCount++;
		playsound		= true;
		break;
	case miBlueCrystal:
		LD3.Map[mpt]	= 0;
		LD3.Map[mpt+110000] = 1;
		CrystalCount++;
		playsound		= true;
		break;
	case miRedCrystal:
		LD3.Map[mpt]	= 0;
		LD3.Map[mpt+110000] = 1;
		CrystalCount++;
		playsound		= true;
		break;
	case miGrenade:
		LD3.Map[mpt]	= 0;
		Grenades		+= 1;
		LD3.Map[mpt+110000] = 1;
		playsound		= true;
		break;	
	case miDrainLifeBy5:
		if(Player->uani != SKELETON){
			Flash(128,255,128);
			Player->lani = SKELETON;
			Player->lstart = SKELETON;
			Player->lend = SKELETON;
			Player->uani = SKELETON;
			Player->velocity = 10.0f;
			Player->nocheck = true;
		}				
		Player->life = 0;
		break;
	case miDetonate:
		DetonateExplosives();
		LD3.Map[mpt] = 0;
		break;
	case miCurrentUp:
		Player->y		-= 4;
		Player->velocity= 0;
		Player->setspeed= -2;
		Player->friction= Player->groundfriction;
		Player->x		+= 2*diff;
		incurrent		= true;
		break;
	case miCurrentDown:
		Player->y		+= 4;
		Player->velocity= 0;
		Player->setspeed= 2;
		Player->friction= Player->groundfriction;
		Player->x		-= 2*diff;
		incurrent		= true;
		break;
	case miCurrentLeft:
		//Player->x		-= 1;
		Player->setspeed= -2;
		incurrent		= true;
		break;
	case miCurrentRight:
		//Player->x		+= 1;
		Player->setspeed= 2;
		incurrent		= true;
		break;
	case miSecret1:
		if(Secret[0] == false){
			Secret[0] = true;
			WriteMessage("You Found A Secret Area");
			Secrets++;
		}		
		break;
	case miSecret2:
		if(Secret[1] == false){
			Secret[1] = true;
			WriteMessage("You Found A Secret Area");
			Secrets++;
		}
		break;
	case miSecret3:
		if(Secret[2] == false){
			Secret[2] = true;
			WriteMessage("You Found A Secret Area");
			Secrets++;
		}
		break;
	case miSecret4:
		if(Secret[3] == false){
			Secret[3] = true;
			WriteMessage("You Found A Secret Area");
			Secrets++;
		}
		break;
	}

	if((mp == miTeleport1 || mp == miTeleport2 || mp == miTeleport3 || mp == miTeleport4)){
		//- find what teleport the player is on
		if(Phasing == 0 && (int)Player->x % 20 < 2 && (int)Player->y > my*20+4 && OnTeleport == false){
			for(int i = 1; i <= NumTeleports; i++){
				if(Teleport[i].x/20 == mx && Teleport[i].y/20 == my){
					NumTeleport = i;
					Phasing = 180;					
					FSOUND_PlaySound(FSOUND_FREE,sndTeleport);
					OnTeleport = true;
					break;
				}
			}
		}
	}
	else
		OnTeleport = false;

	/*if(Swimming == true && isUnderWater == false){
		Player->lstart  = 0;
		Player->lend	= 6;
		Player->lani	= 11;
		Player->uani	= 10;
		Player->gravity = 0.09f;
		Swimming = false;
	}*/
		

	if(incurrent && Player->lstart != 20){
		InCurrent = true;
		Player->lstart	= 20;
		Player->lend	= 34;
		Player->lani	= 21;		
	}
	if(InCurrent == true && incurrent == false){
		if(Player->lani >= 33 && Player->friction == Player->groundfriction){
			Player->lstart  = 0;
			Player->lend	= 6;
			Player->lani	= 0;
			Player->setspeed= 0;
			InCurrent = false;
		}		
	}

	if(LD3.Map[mpt] == miSpikesUp && Player->velocity < -0.5f && (int)Player->y % 20 < 7){
		Player->life = 0;
		Splatter((int)Player->x+9, (int)Player->y+9, 100, 0, Player->BloodCol);
		playsound = false;
	}
	
	if(mp >= miScript1 && mp <= miScript9){
		if(ScriptProcessed[mp-miScript1] == 0){
			LD3.Map[mpt]	= 0;
			ScriptProcessed[mp-miScript1] = 1;
			memset(ScriptFilename, 0, 80);
			sprintf(ScriptFilename, "scripts/%s%d.l3s", &LevelName[Level*80], mp-miScript1+1);
			LoadScript(ScriptFilename);
			//WriteMessage(ScriptFilename);
		}
		playsound = false;
	}
	//- Check for falling objects above Larry
	long highest = mpt-15360; //- 15360 = 512*30
	int  my2 = my;
	if(highest < 55000) highest = 55000+mx;
	for(long mptc = mpt; mptc >= highest; mptc -= MAPWIDTH){
		mp = LD3.Map[mptc];
		
		if(mp == miFallingObject){
			LD3.Map[mptc] = -LD3.Map[mptc];
			FireProjectile((float)mx*20, (float)my2*20, 270, 2, FALLING_OBJECT, &NullEntity, 0);
		}
		else if(mp == miBoulder_Left){
			LD3.Map[mptc] = -LD3.Map[mptc];
			NumMonsters++;
			nm = NumMonsters;

			Monster[nm].Init();
			Monster[nm].x		= (float)mx*20;
			Monster[nm].y		= (float)my2*20;
			Monster[nm].ox		= Monster[nm].x;
			Monster[nm].oy		= Monster[nm].y;
			Monster[nm].id		= BOULDER;
			Monster[nm].spriteset= -1;
			Monster[nm].lani	= 77;
			Monster[nm].lstart	= 77;
			Monster[nm].lend	= 78;
			Monster[nm].life	= 200;
			Monster[nm].dontfall= false;
			Monster[nm].xFlip	= false;
			Monster[nm].count	= 0;
			Monster[nm].setspeed= 0;
			Monster[nm].velocity= 0;
			Monster[nm].BloodCol= 132;
			Monster[nm].HitBox.left		= 0;
			Monster[nm].HitBox.right	= 19;
			Monster[nm].HitBox.top		= 0;
			Monster[nm].HitBox.bottom	= 19;
		}
		else if(mp == miBoulder_Right){
			LD3.Map[mptc] = -LD3.Map[mptc];
			NumMonsters++;
			nm = NumMonsters;

			Monster[nm].Init();
			Monster[nm].x		= (float)mx*20;
			Monster[nm].y		= (float)my2*20;
			Monster[nm].ox		= Monster[nm].x;
			Monster[nm].oy		= Monster[nm].y;
			Monster[nm].id		= BOULDER;
			Monster[nm].spriteset= -1;
			Monster[nm].lani	= 78;
			Monster[nm].lstart	= 78;
			Monster[nm].lend	= 78;
			Monster[nm].life	= 200;
			Monster[nm].dontfall= false;
			Monster[nm].xFlip	= false;
			Monster[nm].count	= 0;
			Monster[nm].setspeed= 0;
			Monster[nm].velocity= 0;
			Monster[nm].BloodCol= 132;
			Monster[nm].HitBox.left		= 0;
			Monster[nm].HitBox.right	= 19;
			Monster[nm].HitBox.top		= 0;
			Monster[nm].HitBox.bottom	= 19;
		}
				
		my2 -= 1;
	}

	if(Player->velocity >= 0){
		mpt = (my+1)*MAPWIDTH+55000;
		mp  = LD3.Map[mpt+mx];
		mp2 = LD3.Map[mpt+mx2];
		DidJump = false;

		switch(mp)
		{
		case miSpikes:
			//if(((int)Player->y+19) % 20 > 10){
				Player->life = 0;
				Splatter((int)Player->x+10, (int)Player->y+31, 100, 0, Player->BloodCol);
			//}
			playsound = false;
			break;
		case miLaserSwitch:
			DeactivateLasers();
			WriteMessage("Lasers Deactivated");
			LD3.Map[mpt+mx] = 74;			
			FSOUND_PlaySound(FSOUND_FREE,sndShorted);
			break;
		case miTouchPlate:
			if(SteppedOnTouchPlate == 0) TurnOffWalls();
			break;
		case miMakerFlag:
			LD3.Map[mpt+mx]	= miMakerFlag+1;
			Marker.x = mx*20;
			Marker.y = my*20;
			RemoveEntities();
			playsound = true;
			break;
		case miJumpFan:
			Player->velocity = -75;
			break;
		case miTreadmillLeft:
			Player->x -= 0.4f;
			Player->inertialx = -0.4f;			
			break;
		case miTreadmillRight:
			Player->x += 0.4f;
			Player->inertialx = 0.4f;
			break;
		case miFallingGround:
			if(Player->y >= my*20+5){
				LD3.Map[mpt+mx] = -LD3.Map[mpt+mx];
				FireProjectile((float)mx*20, (float)my*20+20, 270, 2, FALLING_GROUND, &NullEntity, -LD3.Map[mpt+mx]);
				if(mp2 == miFallingGround && mpt+mx != mpt+mx2){
					LD3.Map[mpt+mx2] = -LD3.Map[mpt+mx2];
					FireProjectile((float)mx2*20, (float)my*20+20, 270, 2, FALLING_GROUND, &NullEntity, -LD3.Map[mpt+mx]);
				}
				
			}
		}
	}
	else
	{
		mpt = mx+(my-1)*MAPWIDTH+55000;
		mp = LD3.Map[mpt];
		
		switch(mp)
		{
		case miSpikes:
			Player->life = 0;
			Splatter((int)Player->x+10, (int)Player->y+31, 100, 0, Player->BloodCol);
			playsound = false;
			break;
		}
	}

	if(playsound) FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);		
	
	Score += score;
	ScoreMap[mx][my] = 0;

	if(CrystalCount == TotalCrystals && TotalCrystals > 0){
		Score += 50000;
		CrystalBonusDisplayTimer = 500;
		CrystalCount = 0;
	}

	if(Player->x < 0) Player->x = 0;
	if(Player->x > 10000){
		Player->x = 10000;
		EndOfLevel		= true;
		EndOfLevelTimer = 1000;
		//FMUSIC_StopSong(song1);
		//FMUSIC_PlaySong(mscLevelComplete);
	}
}

//==============================================================================================================
//= void DetonateExplosives()
//= Make the explosives turn into explosions
//==============================================================================================================
void DetonateExplosives()
{
	int x, y;

	if(NumExplosives > 0){
		x = Explosive[1].x;
		y = Explosive[1].y;
		CreateExplosion(x, y);
		for(int n = 1; n <= NumExplosives-1; n++)
		{
			Explosive[n] = Explosive[n+1];
		}
		NumExplosives--;
	}
}

//==============================================================================================================
//= void NextLevel(int level)
//= Reset the map and load in the data for the level number given
//==============================================================================================================
void NextLevel(int level)
{
	//- Load in the data for the next level
	//DWORD FrameStartTime;
	char ScriptFilename[80];
	char text[40];
	char word1[20], word2[20];
	char TileSetFilename2[80];
	memset(TileSetFilename2, 0, 80);
	bool TileSet2 = false;

	int  y1, y2, spd;

	//LD3.FadeOut(&WinApp);
	//for(int v = MusicVolume; v >= 0; v--){
	//	FrameStartTime = GetTickCount();
	//	while(GetTickCount() - FrameStartTime < 10);
	//	FMUSIC_SetMasterVolume(song1, v);
	//}
	FMUSIC_StopAllSongs();
		
	if(Level != -1) SaveGame();

	if(Level == 0){
		Intro();
		NumWeapons = 0;
		SelectedWeapon = 0;
		Player->uani = 50;
	}	
	else{
		NumWeapons = 3;
		SelectedWeapon = 1;
		Player->uani = 10;
	}
		

	//------------------------------------------------
	//- Reset Values
	//------------------------------------------------
		CutScene = false;
		ProScript = false;
		Talking = false;
		LockInput = false;
		ProScript = false;
		LoadedBossSong = false;
		NumMonsters = 0;
		NumProjectiles = 0;
		NumCannons = 0;
		NumLasers = 0;
		NumWalls = 0;
		NumParticles = 0;
		NumElevators = 0;
		TreadMillDeath = 0;
		NumExplosives = 0;
		
		TotalCrystals = 0;
		GameOverTimer = 0;
		EndOfLevelTimer = 0;
		Player->life  = MaxLife;
		Player->speed = 0;
		Player->lani  = Player->lstart;
		EndOfLevel    = false;
		Player->shoot = false;
		Player->nocheck = false;
		Player->xFlip = false;
		Player->setspeed = 0;
		LockShift = false;

		HasGreenCard	= false;
		HasBlueCard		= false;
		HasYellowCard	= false;
		HasRedCard		= false;
		DumpGreenCard	= true;
		DumpBlueCard	= true;
		DumpYellowCard	= true;
		DumpRedCard		= true;

		xShiftMin = xRes/2-10;
		xShiftMax = xRes/2+10;					

		LetterBonus.L  = false;
		LetterBonus.A  = false;
		LetterBonus.R1 = false;
		LetterBonus.R2 = false;
		LetterBonus.Y  = false;

		xShift = 0;
		yShift = 0;

		SkipScene      = false;

		NumKills	= 0;
		TotalKills	= 0;

		Focus = 0;
		FocusEntity = Player;				

		for(int p = 0; p <= NumPlayers; p++){
			ShowPlayer = true;
			CrystalCount = 0;
			Elevating = false;
		}

		memset(ScoreMap, 0, sizeof(int)*50000);
		memset(ScriptProcessed, 0, 10);
		memset(&Monster[1], 0, sizeof(ENTITY)*499);
		memset(Platform, 0, sizeof(ENTITY)*500);
		memset(Cannon, 0, sizeof(CANNON)*50);	
		memset(Laser, 0, sizeof(LOCATION)*50);
		memset(Wall, 0, sizeof(LOCATION)*50);
		memset(Projectile, 0, sizeof(PROJECTILE)*500);
		memset(Particle, 0, sizeof(PARTICLE)*500);
		memset(Explosion, 0, sizeof(LOCATION)*50);
		memset(Position, 0, sizeof(LOCATION)*10);
		memset(Explosive, 0, sizeof(LOCATION)*50);
		memset(TileSetFilename, 0, 80);
		memset(LevelMessage, 0, 80);

		LD3.AutoXShift = 0;
		HasWaterGun = false;

		for(int i = 0; i <= 3; i++) Secret[i] = false;
		Secrets = 0;
		NumSecrets = 0;

		SpeedTimer = 0;
		Player->maxspeed = 2;
		FMUSIC_SetMasterSpeed(song1, 1.0f);
		if(PowerTimer) WeaponPower = OldWeaponPower;
		PowerTimer = 0;
		if(JumpBTimer) JumpVel = oldJumpVel;
		JumpBTimer = 0;

		LD3.AniSpeed = 0.1f;
		LD3.SkyScrollVertical = 0;
		
		LD3.LoadSprite(&WinApp, "gfx/lary.put", LARRY_SPRITE_SET, 0);
		LD3.LoadSprite(&WinApp, "gfx/bones.put", FLETCHER_SPRITE_SET, 40);
		LD3.LoadSprite(&WinApp, "gfx/mark.put", MARK_SPRITE_SET, 60);
		LD3.LoadSprite(&WinApp, "gfx/rusty.put", RUSTY_SPRITE_SET, 80);		
	//------------------------------------------------
	//- End Reset Values
	//------------------------------------------------

	LD3.SetSkyScroll(0, 255, 2);

	//------------------------------------------------
	//- Read the level attributes
	//------------------------------------------------
		if(Level > -1){
		/*itoa(Level, text, 10);
		memset(ScriptFilename, 0, 80);

		strcpy(ScriptFilename, "maps/map");
		strcat(ScriptFilename, text);
		strcat(ScriptFilename, ".dat");

		WriteMessage(ScriptFilename);*/

		ifstream mfile(&LevelData[Level*80], ios::binary);
		
			memset(word1, 0, 20);
			memset(word2, 0, 20);
			ReadWord(&mfile, word1);
			SDL_strupr(word1);

			do{
				memset(text, 0, 40);
				
				//- check what map attribute is being changes
				if(strcmp(word1, "PALETTE:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "gfx/palettes/");
					strcat(text, word2);
					strcpy(PaletteFilename, text);
				}
				if(strcmp(word1, "TILESET:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					strcpy(TileSetFilename, text);
				}
				if(strcmp(word1, "TILESET2:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					strcpy(TileSetFilename2, text);
					TileSet2 = true;
				}				
				if(strcmp(word1, "OBJSET:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					LD3.LoadTileSet(&WinApp, text, 401);
				}
				if(strcmp(word1, "BACKGRND:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "gfx/backgrounds/");
					strcat(text, word2);
					LD3.LoadSky(&WinApp, text, 0, 0);
				}
				if(strcmp(word1, "MUSIC:") == 0){
					ReadQuote(&mfile, word2);
					strcpy(text, "music/");
					strcat(text, word2);
					song1   = FMUSIC_LoadSong(text);
				}
				if(strcmp(word1, "MESSAGE:") == 0){
					ReadQuote(&mfile, word2); SDL_strupr(word2);
					strcpy(LevelMessage, word2);
					WriteMessage (word2);
				}
				if(strcmp(word1, "BOSSSET:") == 0){
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					LD3.LoadSprite(&WinApp, text, BOSS_SPRITE_SET, 0);
				}
				if(strcmp(word1, "LARRYSET:") == 0){
					//NOTE(davidgow): filenames will be lowercase
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					LD3.LoadSprite(&WinApp, text, LARRY_SPRITE_SET, 0);
					LD3.LoadSprite(&WinApp, "gfx/bones.put", FLETCHER_SPRITE_SET, 40);
					LD3.LoadSprite(&WinApp, "gfx/mark.put", MARK_SPRITE_SET, 60);
				}
				if(strcmp(word1, "BONESSET:") == 0){
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					LD3.LoadSprite(&WinApp, text, FLETCHER_SPRITE_SET, 40);					
					LD3.LoadSprite(&WinApp, "gfx/mark.put", MARK_SPRITE_SET, 60);
				}
				if(strcmp(word1, "RUSTYSET:") == 0){
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "gfx/");
					strcat(text, word2);
					LD3.LoadSprite(&WinApp, text, RUSTY_SPRITE_SET, 80);
				}
				if(strcmp(word1, "BOSSSONG:") == 0){
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "music/");
					strcat(text, word2);
					mscBoss = FMUSIC_LoadSong(text);
				}
				if(strcmp(word1, "LOADSPRITE:") == 0){
					ReadQuote(&mfile, word2); SDL_strlwr(word2);
					strcpy(text, "gfx/");
					strcat(text, word2);

					y1 = ReadValue(&mfile);
					y2 = ReadValue(&mfile);

					LD3.LoadSprite(&WinApp, text, y1, y2);					
				}
				if(strcmp(word1, "OBJNAME:") == 0){
					ReadQuote(&mfile, word2); SDL_strupr(word2);
					memset(ObjectName, 0, 20);
					strcpy(ObjectName, word2);
				}
				if(strcmp(word1, "SETSKYSCROLL") == 0){
					y1  = ReadValue(&mfile);
					y2  = ReadValue(&mfile);
					spd = ReadValue(&mfile);
					LD3.SetSkyScroll(y1, y2, spd);
				}
				if(strcmp(word1, "SKYSCROLLVERTICAL") == 0){
					LD3.SkyScrollVertical = ReadValue(&mfile);
				}
				if(strcmp(word1, "SHIFTPALETTE") == 0){
					ShiftPalette = ReadValue(&mfile);
				}
				if(strcmp(word1, "VERTICALSCROLLSHIFT") == 0){
					LD3.SkyVerticalScrollShift = ReadValue(&mfile);
				}
				if(strcmp(word1, "AUTOXSHIFT") == 0){
					LD3.AutoXShift = ReadValue(&mfile);
				}
				if(strcmp(word1, "HASWATERGUN") == 0){
					HasWaterGun = ReadValue(&mfile);
				}
				if(strcmp(word1, "ANISPEED") == 0){
					LD3.AniSpeed = float(ReadValue(&mfile))/100;
				}				
				if(strcmp(word1, "TREADMILLDEATH") == 0){
					TreadMillDeath = ReadValue(&mfile);
				}
													
				memset(word1, 0, 20);
				memset(word2, 0, 20);
				ReadWord(&mfile, word1);
				SDL_strupr(word1);
				
			} while(strcmp(word1, "END") != 0);


			mfile.close();
		}
	//------------------------------------------------
	//- End reading level attributes
	//------------------------------------------------

	

	switch(level)
	{
	case -1:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/blank.ld3");
		strcpy(TileSetFilename, "gfx/ld3font.put");
		song1   = FMUSIC_LoadSong("music/title.it");
		LD3.LoadSky(&WinApp, "gfx/backgrounds/title.bmp", 0, 0);
		LD3.SetSkyScroll(0, 59, 16);
		LD3.SetSkyScroll(60, 255, 16);
		break;	/*
	case 99:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/maptut.ld3");
		strcpy(TileSetFilename, "gfx/ld3tile1.put");		
		WriteMessage("Training");
		break;
	case 0:
		Score = 0;
		ScoreDisplay = 0;
		Lives = 5;
		Player->life = MaxLife;

		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/mapr1.ld3");
		strcpy(TileSetFilename, "gfx/ld3tile0.put");
		song1   = FMUSIC_LoadSong("music/scent.mid");
		WriteMessage("Two Weeks Later");
		break;
	case 1:
			
		Score = 0;
		ScoreDisplay = 0;
		Lives = 5;
		Player->life = MaxLife;

		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/map01.ld3");
		strcpy(TileSetFilename, "gfx/ld3tile1.put");
		song1   = FMUSIC_LoadSong("music/spikevalley.it");		
		LD3.LoadTileSet(&WinApp, "gfx/ld3objs1.put", 200);
		WriteMessage("Spike Valley");
		break;
	case 2:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		
		LD3.LoadMap("maps/map02.ld3");
		strcpy(TileSetFilename, "gfx/ld3tileh.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objs2.put", 200);
		song1   = FMUSIC_LoadSong("music/spikevalley2.it");
		WriteMessage("Fall Evening");
		break;
	case 3:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		

		LD3.LoadSky(&WinApp, "gfx/backgrounds/oceandrop.bmp", 0, 1);
		LD3.SetSkyScroll(100, 159, 3);
		LD3.SkyScrollVertical = true;
		LD3.SkyVerticalScrollShift = 2;
	
		
		LD3.LoadMap("maps/map04.ld3");
		strcpy(TileSetFilename, "gfx/ld3tileo.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objs3.put", 200);
		song1 = FMUSIC_LoadSong("music/bossmix.it");
		WriteMessage("Ocean Drop");	
		break;
	/*case 4:
		LD3.LoadMap("maps/map05.ld3");
		strcpy(TileSetFilename, "gfx/ld3tileo.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objs3.put", 200);
		song1 = FMUSIC_LoadSong("music/bossmix.it");
		WriteMessage("Crane Traffic");
		break;
	case 5:
		LD3.LoadMap("maps/mapf1.ld3");
		strcpy(TileSetFilename, "gfx/ld3tilec.put");
		LD3.LoadSprite(&WinApp, "gfx/ld3fly.put", FLY_MONSTER_SPRITE_SET, 60);
		//LD3.LoadSky(&WinApp, "gfx/bigback1.bmp", 1);
		song1   = FMUSIC_LoadSong("music/shinyforest.it");
		//FMUSIC_SetMasterVolume(song1, 196);
		WriteMessage("Infested Forest");
		LD3.LoadSprite(&WinApp, "gfx/ld3fly.put", FLY_MONSTER_SPRITE_SET, 60);
		break;
	case 6:
		LD3.LoadMap("maps/mapf2.ld3");
		strcpy(TileSetFilename, "gfx/ld3tilec.put");
		LD3.LoadSprite(&WinApp, "gfx/ld3fly.put", FLY_MONSTER_SPRITE_SET, 60);
		//LD3.LoadSky(&WinApp, "gfx/bigback1.bmp", 1);
		song1   = FMUSIC_LoadSong("music/shinyforest.it");
		//FMUSIC_SetMasterVolume(song1, 196);
		LD3.LoadSprite(&WinApp, "gfx/ld3fly.put", FLY_MONSTER_SPRITE_SET, 60);
		WriteMessage("Shiny Cave");	
		break;
	case 4:
		//FMUSIC_PlaySong(mscLevelIntro);		
		//LD3.LoadSplashScreen(&WinApp, "gfx/levelintro/level4.bmp", 5000);
		//FMUSIC_StopSong(mscLevelIntro);
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		
		
		LD3.LoadMap("maps/mapf2.ld3");
		LD3.LoadSprite(&WinApp, "gfx/laryswim.put", LARRY_SPRITE_SET, 0);
		strcpy(TileSetFilename, "gfx/ld3tilef.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objss.put", 200);
		LD3.LoadSprite(&WinApp, "gfx/ld3boss4.put", BOSS_SPRITE_SET, 0);
		//song1   = FMUSIC_LoadSong("music/shinyforest.it");
		song1   = FMUSIC_LoadSong("music/upassage.it");
		WriteMessage("The Shiny Forest");	
		break;
	case 5:
		strcpy(PaletteFilename, "gfx/palettes/grada.pal");

		LD3.LoadMap("maps/mapg1.ld3");
		strcpy(TileSetFilename, "gfx/ld3tileg.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objsg.put", 200);
		song1   = FMUSIC_LoadSong("music/mustyway.it");
		WriteMessage("Golden Estate");	
		break;
	case 6:
		strcpy(PaletteFilename, "gfx/palettes/grada.pal");

		LD3.LoadMap("maps/mapg2.ld3");
		strcpy(TileSetFilename, "gfx/ld3tile6.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objsg.put", 200);
		song1   = FMUSIC_LoadSong("music/crossways.it");		
		WriteMessage("Aqueduct Crossways");	
		break;
	case 7:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/map06.ld3");
		strcpy(TileSetFilename, "gfx/ld3tile3.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objss.put", 200);
		song1   = FMUSIC_LoadSong("music/vertigo.it");
		WriteMessage("Frost Bite");	
		break;
	case 8:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/map07.ld3");
		strcpy(TileSetFilename, "gfx/ld3tiled.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objss.put", 200);
		LD3.LoadSprite(&WinApp, "gfx/larysnow.put", LARRY_SPRITE_SET, 0);
		//song1   = FMUSIC_LoadSong("music/melancholy2.it");
		//song1   = FMUSIC_LoadSong("music/upassage.it");
		song1   = FMUSIC_LoadSong("music/metropolis.it");
		WriteMessage("Frozen Slush");	
		break;	
	case 9:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/mapz1.ld3");
		LD3.LoadSprite(&WinApp, "gfx/lary.put", LARRY_SPRITE_SET, 0);
		strcpy(TileSetFilename, "gfx/ld3tilez.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objsz.put", 200);
		song1   = FMUSIC_LoadSong("music/other.it");
		WriteMessage("The Other World");
		break;
	case 10:
		LD3.LoadMap("maps/mapy1.ld3");
		strcpy(TileSetFilename, "gfx/ld3tiley.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objsz.put", 200);
		song1   = FMUSIC_LoadSong("music/metropolis.it");
		WriteMessage("Alien Metropolis");
		break;
	case 11:
		strcpy(PaletteFilename, "gfx/palettes/grad.pal");
		LD3.LoadMap("maps/mapz3.ld3");
		strcpy(TileSetFilename, "gfx/ld3tilez.put");
		LD3.LoadTileSet(&WinApp, "gfx/ld3objsz.put", 200);
		LD3.LoadSprite(&WinApp, "gfx/xahn.put", XAHN_SPRITE_SET, 40);
		song1   = FMUSIC_LoadSong("music/laststand.it");
		WriteMessage("Xahn's Last Stand");
		break;	*/
	}

	//LD3.LoadTileSet(&WinApp, "gfx/blank10.put", 240);
	LD3.LoadTileSet(&WinApp, TileSetFilename, 0);
	if(TileSet2) LD3.LoadTileSet(&WinApp, TileSetFilename2, 162);
	LD3.LoadPalette(&WinApp, PaletteFilename);

	memcpy(FadePalette, WinApp.palette, sizeof(SDL_Color)*256);
	
	SDL_FreePalette(WinApp.lpddpal);
	WinApp.lpddpal = SDL_AllocPalette(256);
	SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
	SDL_SetSurfacePalette(WinApp.lpddsback, WinApp.lpddpal);
	//TODO(davidgow): Again, this was the primary surface.

	/*if(level == -1){
		//LD3.LoadSky(&WinApp, "gfx/backgrounds/titletop.bmp", 0, 1);
		//LD3.LoadSky(&WinApp, "gfx/backgrounds/titlemid.bmp", 1, 1);
		//LD3.LoadSky(&WinApp, "gfx/backgrounds/titlelow.bmp", 2, 1);
		//LD3.LoadSky(&WinApp, "gfx/backgrounds/titlelow2.bmp", 3, 1);
		level = -rand()%15;
	}	
	if(abs(level) == 0 || abs(level) == 9){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/darktop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/darkmid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/darklow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/darklow2.bmp", 3, 1);
	}
	else if(abs(level) > 0 && abs(level) <= 2){
		if(level < 2)
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greentop.bmp", 0, 1);
		else
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greentop2.bmp", 0, 1);
		if(level < 2) 
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenmid.bmp", 1, 1);
		else
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenmid2.bmp", 1, 1);
		if(level < 2) 
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenlow.bmp", 2, 1);
		else
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenlow-2.bmp", 2, 1);
		if(level < 2) 
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenlow2.bmp", 3, 1);
		else
			LD3.LoadSky(&WinApp, "gfx/backgrounds/greenlow3.bmp", 3, 1);
	}
	else if(abs(level) >= 3 && abs(level) <= 4){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/bluetop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/bluemid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/bluelow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/bluelow2.bmp", 3, 1);
	}
	else if(abs(level) >= 5 && abs(level) <= 5){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormtop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormmid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormlow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormlow2.bmp", 3, 1);		
	}
	else if(abs(level) >= 6 && abs(level) <= 7){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormtop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormmid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormlow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/stormlow2.bmp", 3, 1);
	}
	else if(abs(level) >= 9 && abs(level) <= 10){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/purpletop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/purplemid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/purplelow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/purplelow2.bmp", 3, 1);
	}
	else if(abs(level) == 11){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/othertop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/othermid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/otherlow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/otherlow2.bmp", 3, 1);
	}
	else if(abs(level) >= 12){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/metrotop.bmp", 0, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/metromid.bmp", 1, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/metrolow.bmp", 2, 1);
		LD3.LoadSky(&WinApp, "gfx/backgrounds/metrolow2.bmp", 3, 1);
	}*/
	/*if(Level == -1 || Level == 1 || Level == 2 || Level == 4){
		LD3.LoadSky(&WinApp, "gfx/backgrounds/spikevalley.bmp", 0, 1);
		//LD3.LoadSky(&WinApp , "gfx/backgrounds/oceandrop.bmp", 0, 1);
		if(Level == 2) LD3.LoadSky(&WinApp, "gfx/backgrounds/fallevening.bmp", 0, 1);
		LD3.SetSkyScroll(0, 15, 6);
		LD3.SetSkyScroll(16, 30, 4);
		LD3.SetSkyScroll(41, 64, 5);
		LD3.SetSkyScroll(65, 145, 4);
		LD3.SetSkyScroll(146, 165, 3);
		LD3.SetSkyScroll(166, 185, 2);
		LD3.SetSkyScroll(186, 255, 1);
	}*/

	//if(Level == 1) LD3.LoadMap("maps/map1.ld3");
	//if(Level == 2) LD3.LoadMap("maps/map2.ld3");

	if(Level > -1) LD3.LoadMap(&LevelFilename[Level*80]);

	//LD3.LoadSky(&WinApp, "gfx/backgrounds/gstate.bmp", 0, 2);
	/*LD3.SetSkyScroll(0, 20, 7);
	LD3.SetSkyScroll(21, 40, 6);
	LD3.SetSkyScroll(41, 51, 5);
	LD3.SetSkyScroll(52, 77, 4);
	LD3.SetSkyScroll(78, 92, 3);
	LD3.SetSkyScroll(93, 150, 2);
	LD3.SetSkyScroll(151, 255, 1);*/

	//LD3.SetSkyScroll(0, 255, 2);
	//LD3.SkyScrollVertical = true;
	//LD3.SkyVerticalScrollShift = 2;
	
	//LD3.SetSkyScroll(100, 159, 3);

	//LD3.SkyScrollVertical = true;
	//LD3.SkyVerticalScrollShift = 2;
	
	ScanMap();
	
	//LD3.FadeIn(&WinApp);
	//if(level == 1) LD3.LoadLevelSplashScreen(&WinApp, 1);
	//if(level == 2) LD3.LoadLevelSplashScreen(&WinApp, 2);	

	memset(ScriptFilename, 0, 80);
	strcpy(ScriptFilename, "scripts/");
	strcat(ScriptFilename, &LevelName[Level*80]);
	memset(text, 0, 20);
	strcat(ScriptFilename, "0");
	strcat(ScriptFilename, ".l3s");
	LoadScript(ScriptFilename);
	
	if(ProScript == false || level < 0){
		MusicVolume = maxMusicVolume;
		MusicFade = MusicVolume;
		FMUSIC_SetMasterVolume(song1, MusicVolume);
		FMUSIC_PlaySong(song1);
	}
	else
		MusicFade = 0;

	if(Level >= 0) Flash(255, 255, 255);
	
}

//==============================================================================================================
//= void ResetEntities()
//= Scan the map for any negative numbers and set the to positive
//= When Larry is in contact with an item, that item is set to it's negative value to be reset by the marker
//==============================================================================================================
void ResetEntities()
{
	int  mp;
	long mpt;
	
	for(int my = 0; my <= MAPHEIGHT-1; my++){
		for(int mx = 0; mx <= MAPWIDTH-1; mx++){
		
			mpt = mx+(my<<MAPXLSHIFT)+55000;
			mp = LD3.Map[mpt];
	
			if(LD3.Map[mpt] < 0) LD3.Map[mpt] = -LD3.Map[mpt];
			if(LD3.Map[mpt] >= 256 && LD3.Map[mpt]  <= 400) LD3.Map[mx+(my<<MAPXLSHIFT)+165000] = 4;
			
			if(DumpGreenCard)		HasGreenCard	= false;
			if(DumpBlueCard)		HasBlueCard		= false;
			if(DumpYellowCard)		HasYellowCard	= false;
			if(DumpRedCard)			HasRedCard		= false;

			if(LD3.Map[mpt] == 31) LD3.Map[mpt+110000] = 4;
			if(LD3.Map[mpt] == 74) LD3.Map[mpt] = 73;
			if(LD3.Map[mpt] == miGoCart){
				NumMonsters++;
				int nm = NumMonsters;

				Monster[nm].x		= (float)mx*20;
				Monster[nm].y		= (float)my*20+7;
				Monster[nm].ox		= Monster[nm].x;
				Monster[nm].oy		= Monster[nm].y;
				Monster[nm].id		= GO_CART;
				Monster[nm].spriteset= -1;
				Monster[nm].lani	= 80;
				Monster[nm].lstart	= 80;
				Monster[nm].lend	= 80;
				Monster[nm].life	= 20;
				Monster[nm].dontfall= false;
				Monster[nm].xFlip	= false;
				Monster[nm].count	= 0;
				Monster[nm].setspeed= 0;
				LD3.Map[mpt] = -LD3.Map[mpt];					
			}
		}
	}
	for(int i = 1; i <= NumMonsters; i++){
		Monster[i].aiState = aiCALM;
		if(Monster[i].id == ANCHOR){
			Monster[i].x = Monster[i].startx;
			Monster[i].y = Monster[i].starty;
			Monster[i].life = 100;
			Monster[i].setspeed = 0;
			Monster[i].visible = true;
			Monster[i].velocity = 0;
			Monster[i].count = 0;
		}
	}
	/*for(int i = 1; i <= NumResetPlatforms; i++){
		NumMonsters++;
		Monster[NumMonsters] = Platform[i];
		//Monster[NumMonsters].count = 0
		SetDefaultValue(&Monster[NumMonsters]);
	}*/
	NumResetPlatforms = 0;
	if(SteppedOnTouchPlate == 1) SteppedOnTouchPlate = 0;
}

//==============================================================================================================
//= void RemoveEntities()
//= Scan the map for any negative numbers and set them to zero
//= When Larry is in contact with an item, that item is set to it's negative value to be reset by the marker
//= When Larry hits a new marker, it sets the negatives to zero so they won't be reset if Larry dies
//= This way simulates a save whenever Larry hits a new maker
//==============================================================================================================
void RemoveEntities()
{
	for(long l = 50000; l <= 100000; l++){
		if(LD3.Map[l] < 0 && -LD3.Map[l] != miGoCart) LD3.Map[l] = 0;	
	}
	NumResetPlatforms = 0;
	if(SteppedOnTouchPlate == 1) SteppedOnTouchPlate = 2;

	//- If the player has one of the cards, save it by not dumping it when the player dies
	if(HasGreenCard)	DumpGreenCard = false;
	if(HasBlueCard)		DumpBlueCard = false;
	if(HasYellowCard)	DumpYellowCard = false;
	if(HasRedCard)		DumpRedCard = false;
}

//==============================================================================================================
//= void DisplayHUD()
//= Display the heads-up display
//==============================================================================================================
void DisplayHUD()
{
	char Text1[80], Text2[80];
	int  cosine = Cosine[Phasing]*(360-Phasing);
	int  sine = Sine[Phasing]*(360-Phasing);
	float LifeBarFactor = (MaxLife/100);
	int   LifeBarX = 2+int(Player->life/LifeBarFactor);
	LifeBarFactor *= 10;
	
		
		if(Player->life > MaxLife) Player->life = MaxLife;
		if(Player->life > 0){
			if(Phasing){
				LD3.PutSprite(&WinApp, int(Player->x)-xShift+cosine, int(Player->y)-yShift+sine, 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift-cosine, int(Player->y)-yShift-sine, 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift+cosine, int(Player->y)-yShift-sine, 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift-cosine, int(Player->y)-yShift+sine, 0, 37+((Phasing>>1) & 1), false);
			}
							
			if(ShowPlayer){
				if(Player->x-xShift > -20 && Player->x-xShift < xRes && Player->y-yShift > -20 && Player->y-yShift < yRes){
					LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, Player->spriteset, (int)Player->lani, Player->xFlip);
					if(InCurrent == false){						
						if(Player->shoot == false && Player->uani != SKELETON){
							if(Talking == false && ProScript == false)
								LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, Player->spriteset, 6, Player->xFlip);
							else if(Talking == true && TalkSprite == 142)
								LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, Player->spriteset, 7+(TalkId-TalkSprite), Player->xFlip);
							else
								LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, Player->spriteset, 7, Player->xFlip);
						}
						LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, Player->spriteset, (int)Player->uani, Player->xFlip);
					}					
				}
			}
			if(Phasing){
				LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, 0, 35+((Phasing>>1) & 3), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift+(cosine<<2), int(Player->y)-yShift+sine, 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift-(cosine<<2), int(Player->y)-yShift-sine, 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift+sine, int(Player->y)-yShift-(cosine<<2), 0, 37+((Phasing>>1) & 1), false);
				LD3.PutSprite(&WinApp, int(Player->x)-xShift-sine, int(Player->y)-yShift+(cosine<<2), 0, 37+((Phasing>>1) & 1), false);
												
				Phasing += PhaseCount;
				if(Phasing == 320) FSOUND_PlaySound(FSOUND_FREE,sndThunder);

				if(Phasing == 350){
					for(int i = 1; i <= NumTeleports; i++){
						if((Teleport[i].x != Teleport[NumTeleport].x || Teleport[i].y != Teleport[NumTeleport].y) && Teleport[i].l == Teleport[NumTeleport].l){
							Player->x = Teleport[i].x;
							Player->y = Teleport[i].y;
							FadeToWhite = true; Fading = true;
							break;							
						}							
					}
				}
				if(Phasing > 360){
					Phasing -= 360;
					PhaseCount = 1;
					Phasing = false;
					ShowPlayer = true;
					FadeToWhite = false;
					FadeOutOfWhite = true;
					Fading = true;
					//- find what teleport the player is to goto
					
				}
			}
		}
		else if(Fading == false){
			if(GameOverTimer == 0) GameOverTimer = 1000;
			if(GameOverTimer > 500){
				if(Player->uani == SKELETON){
					int mx  = (int)(Player->x+9) / 20;
					int my = (int)(Player->y+9-4) / 20;
					int mpt = mx+(my<<MAPXLSHIFT)+55000;
					int mp = LD3.Map[mpt];
					if(mp == miDrainLifeBy5) Player->velocity -= 0.2f;
					Player->y += Player->velocity;
					LD3.PutSprite(&WinApp, int(Player->x)-xShift, int(Player->y)-yShift, 0, (int)Player->uani, Player->xFlip);
				}
				if(Lives <= 1){
					WriteText(0, 40, "GAME OVER", 37, DT_CENTER);
					Lives = 0;
				}

				GameOverTimer--;
				if(GameOverTimer <= 900 && Lives <= 0) EndGame = true;

				if(GameOverTimer <= 900 && Lives > 1){
					Player->life = MaxLife;
					LockShift = false;					
					Player->x = (float)Marker.x;
					Player->y = (float)Marker.y;
					Player->lstart  = 0;
					Player->lend	= 6;
					Player->lani	= 0;
					Player->setspeed= 0;
					Player->nocheck = false;
					Player->xFlip = false;
					InCurrent = false;
					xShift = (int)Player->x - 160;
					yShift = (int)Player->y - 120;
					oxShift = xShift;
					oyShift = yShift;
					Player->lani = Player->lstart;
					Player->uani = Player->ustart;
					Player->shoot= false;
					PlayedDeathSound = false;
					GameOverTimer = 0;
					ResetEntities();
					Lives--;
					FadeOutOfBlack = true;
					Fading = true;
					SpeedTimer = 0;
					Player->maxspeed = 2;
					FMUSIC_SetMasterSpeed(song1, 1.0f);
					if (PowerTimer > 0) WeaponPower = OldWeaponPower;
					PowerTimer = 0;
					if(JumpBTimer > 0) JumpVel = oldJumpVel;
					JumpBTimer = 0;
					Focus = 0;
					FocusEntity = Player;

					WriteMessage(LevelMessage);
					FMUSIC_StopSong(mscBoss);
					FMUSIC_PlaySong(song1);
					LoadedBossSong = false;
					PlayedEndOfLevelMusic = false;
				}				
			}
		}
		if(Player->life <= 0){
			Player->life = 0;
			if(Fading == false){
				FadeToBlack = true;
				Fading = true;
			}
		}

	//----------------------------------------------
	//- Display Level Complete if Larry finished
	//- the level or display Game Over if Larry died
	//----------------------------------------------
		if(EndOfLevel){
			Player->uani = Player->ustart;
			Player->lani = Player->lstart;
			Player->shoot = false;
			WriteText(0, 40, "Level Complete", 90, DT_CENTER);
			WriteText(0, 70, "Bonus +1000", 15, DT_CENTER);
			
			memset(Text1, 0, 80); memset(Text2, 0, 80);
			sprintf(Text1, "Secrets Found %d/%d", Secrets, NumSecrets);

			if(PlayedEndOfLevelMusic == false){
				FMUSIC_StopSong(song1);
				FMUSIC_StopSong(mscBoss);				
				FMUSIC_PlaySong(mscLevelComplete);
				FMUSIC_SetLooping(mscLevelComplete, false);
				PlayedEndOfLevelMusic = true;
			}
			
			WriteText(0, 110, Text1, 90, DT_CENTER);
			//memset(Text1, 0, 80); memset(Text2, 0, 80);
			//itoa(TimeBonus, Text2, 10);
			//strcpy(Text1, "Time Bonus +");
			//strcat(Text1, Text2);
			//WriteText(0, 100, &g_Font, Text1, 15, DT_CENTER);
			//TimeDeductCount = 0;
			EndOfLevelTimer--;
			if(EndOfLevelTimer <= 0){
				
				memset(ScriptFilename, 0, 80);
				strcpy(ScriptFilename, "scripts/");
				strcat(ScriptFilename, &LevelName[Level*80]);
				strcat(ScriptFilename, "end.l3s");
				LoadScript(ScriptFilename);

				EndOfLevel = false;
				//if(Level == 5){
				//	LD3.LoadPalette(&WinApp, PaletteFilename);
				//	EndGame = true;
				//}
				//else{

				LetterBonus.L  = false;
				LetterBonus.A  = false;
				LetterBonus.R1 = false;
				LetterBonus.R2 = false;
				LetterBonus.Y  = false;
				
				if(ProScript == false){
					Level++;
					NextLevel(Level);
					Player->life = MaxLife;
					LetterBonus.L  = false;
					LetterBonus.A  = false;
					LetterBonus.R1 = false;
					LetterBonus.R2 = false;
					LetterBonus.Y  = false;
				}
				//}
			}
		}	
		
		//----------------------------------------------
		//- End Displaying Level Complete or Game Over
		//----------------------------------------------
			
			if(SysMsgCount > 0 && ProScript == false){				
				WriteText(0, 40, SysMsg, 15, DT_CENTER);
				SysMsgCount--;	
			}

		//--------------------------------------------
		//- Add the score display and play the ding
		//- sound if it is smaller than the score
		//--------------------------------------------
			if(ScoreDisplay < Score){
				ScoreDisplay += 50;
				if(ScoreDisplay >= Score) ScoreDisplay = Score;
				FSOUND_PlaySound(FSOUND_FREE,sndDing);

				if((int)ScoreDisplay / 25000 > ExtraLifeScore){
					ExtraLifeScore = (int)ScoreDisplay/25000;
					Lives++;					
					FSOUND_PlaySound(FSOUND_FREE,diescum);
				}
			}
		//--------------------------------------------
		//- End adding score display
		//--------------------------------------------
		
			//- Display Time Bonus
				//TimeDeductCount++;
				//if(TimeDeductCount >= 50){
				//	TimeDeductCount = 0;
				//	TimeBonus -= 10;
				//	if(TimeBonus < 0) TimeBonus = 0;
				//}
				//memset(Text1, 0, 80); memset(Text2, 0, 80);
				//strcpy(Text1, "Time Bonus: ");
				//itoa(TimeBonus, Text2, 10);
				//strcat(Text1, Text2);
				//WriteText(0, 230, &g_Font, Text1, 90, DT_RIGHT);
				//memset(Text1, 0, 80);
				//itoa(TimeBonus, Text1, 10);
				//WriteText(0, 230, &g_Font, Text1, 15, DT_RIGHT);
		
		//--------------------------------------------
		//- Draw Status Background
		//--------------------------------------------
			if(StatusState > 4){
				//LD3.DrawBox(&WinApp, 0, yRes-30, 40, yRes, 15, false);
				LD3.DrawBox(&WinApp, 0, yRes-29, 39, yRes, 69, true);
				//LD3.DrawBox(&WinApp, 0, yRes-21, xRes, yRes, 15, false);
				LD3.DrawBox(&WinApp,   0, yRes-20,  127, yRes, 68, true);
				LD3.DrawBox(&WinApp, 128, yRes-20,  180, yRes, 69, true);
				LD3.DrawBox(&WinApp, 181, yRes-20,  234, yRes, 68, true);
				LD3.DrawBox(&WinApp, 235, yRes-20, xRes, yRes, 69, true);
				//Lives = 13;
				//NumKills = 130;
			}			
			if(StatusState >= 0){
				//--------------------------------------------
				//- Display KeyCards
				//--------------------------------------------
					if(HasGreenCard)	LD3.PutSprite(&WinApp, -4, yRes-41, -1, miGreenCard, false);
					if(HasBlueCard)		LD3.PutSprite(&WinApp,  6, yRes-41, -1, miBlueCard, false);
					if(HasYellowCard)	LD3.PutSprite(&WinApp, 16, yRes-41, -1, miYellowCard, false);
					if(HasRedCard)		LD3.PutSprite(&WinApp, 26, yRes-41, -1, miRedCard, false);
				//--------------------------------------------
				//- End Display KeyCards
				//--------------------------------------------
			}
			if(StatusState > 0){
				memset(Text1, 0, 80); memset(Text2, 0, 80);
				sprintf(Text1, "%d/%d", CrystalCount, TotalCrystals);
				LD3.PutSprite(&WinApp, 127, yRes-28, -1, miGreenCrystal, false);
				LD3.PutSprite(&WinApp, 137, yRes-28, -1, miYellowCrystal, false);
				LD3.PutSprite(&WinApp, 147, yRes-28, -1, miBlueCrystal, false);
				LD3.PutSprite(&WinApp, 157, yRes-28, -1, miRedCrystal, false);
				WriteText(133, yRes-9, Text1, 15, 0);
			}
			if(StatusState > 1){
				memset(Text1, 0, 80); memset(Text2, 0, 80);
				sprintf(Text1, "%d/%d", NumKills, TotalKills);
				WriteText(185, yRes-17, "Kills", 28, 0);
				WriteText(185, yRes-9, Text1, 15, 0);
			}
			if(StatusState > 2){
				memset(Text1, 0, 80); memset(Text2, 0, 80);
				sprintf(Text1, "%d/%d", Secrets, NumSecrets);
				WriteText(285, yRes-17, "S:", 28, 0);
				WriteText(300, yRes-17, Text1, 15, 0);
			}
			if(StatusState > 3){
				if(LetterBonus.L)  WriteText(230, yRes-17, "L", 10, 0);
				if(LetterBonus.A)  WriteText(240, yRes-17, "A", 10, 0);
				if(LetterBonus.R1) WriteText(250, yRes-17, "R", 10, 0);
				if(LetterBonus.R2) WriteText(260, yRes-17, "R", 10, 0);
				if(LetterBonus.Y)  WriteText(270, yRes-17, "Y", 10, 0);			
			}

		//--------------------------------------------
		//- End Draw Status Background
		//--------------------------------------------

		//--------------------------------------------
		//- Display Score
		//--------------------------------------------
				memset(Text1, 0, 80); memset(Text2, 0, 80);
				sprintf(Text1, "Score: %ld", ScoreDisplay);
				WriteText(2, yRes-9, Text1, 90, DT_RIGHT);
				memset(Text1, 0, 80);
				sprintf(Text1, "%ld", ScoreDisplay);
				WriteText(50, yRes-9, Text1, 15, DT_RIGHT);
		//--------------------------------------------
		//	end displaying score
		//--------------------------------------------

		//--------------------------------------------
		//- Draw the life bar
		//--------------------------------------------
			LD3.DrawBox(&WinApp, 2, yRes-9, LifeBarX, yRes-5, int(50+(Player->life/LifeBarFactor)), true); 
			if(Player->life < MaxLife) LD3.DrawBox(&WinApp, LifeBarX, yRes-9, 102, yRes-5, 49, true); 
		//--------------------------------------------
		//- End drawing life bar
		//--------------------------------------------

		//--------------------------------------------
		//- Display life bar
		//--------------------------------------------
			memset(Text1, 0, 80);
			sprintf(Text1, "%d", Lives);
			WriteText(106, yRes-9, "x", 28, 0);
			WriteText(113, yRes-9, Text1, 15, 0);
		//--------------------------------------------
		//- End Display life bar
		//--------------------------------------------

		//--------------------------------------------
		//- Display Ammo
		//--------------------------------------------
			memset(Text1, 0, 80);
			WriteText(2,  yRes-16, "Ammo:", 28, 0);
			int AmmoValue = -1;
			switch(Weapon[SelectedWeapon])
			{
			case SHOTGUN:
				AmmoValue = Shells;
				break;
			case MACHINEGUN:
				AmmoValue = Bullets;
				break;
			case MAC:
				AmmoValue = Choppers;
				break;
			}
			sprintf(Text1, "%d", AmmoValue);
			WriteText(40, yRes-16, Text1, 15, 0);
			//WriteText(2, 224, "Grenades:", 28, 0);
			LD3.PutSprite(&WinApp, 60, yRes-30, -1, miGrenade, false);
			WriteText(75, yRes-16, "x", 28, 0);
			memset(Text1, 0, 80);
			sprintf(Text1, "%d", Grenades);
			WriteText(82, yRes-16, Text1, 15, 0);
		//--------------------------------------------
		//- End Display Ammo
		//--------------------------------------------
		
			if(ControllingMAC){
				
				WriteText(2, 2, "MAC Unit", 15, 0);
				
				LD3.DrawBox(&WinApp, 2, 10, int(2+MACunit->life), 18, int(162+(MACunit->life/10)), true); 
				if(MACunit->life < 50) LD3.DrawBox(&WinApp, int(2+MACunit->life), 10, 52, 18, 161, true); 
				LD3.DrawBox(&WinApp, 2, 19, int(2+MACammo), 27, int(226+(MACammo/10)), true); 
				if(MACammo < 50) LD3.DrawBox(&WinApp, int(2+MACammo), 19, 52, 27, 225, true); 
				LD3.DrawBox(&WinApp, 2, 28, int(2+MACfuel), 36, int(130+(MACfuel/10)), true); 
				if(MACfuel < 50) LD3.DrawBox(&WinApp, int(2+MACfuel), 28, 52, 36, 129, true); 

				WriteText(12, 11, "LIFE", 15, 0);
				WriteText(12, 20, "AMMO", 15, 0);				
				WriteText(12, 29, "FUEL", 15, 0);
			}

		//--------------------------------------------
		//- Display Bonus Letters Captured
		//--------------------------------------------
			if(BonusDisplayTimer){
				int xstart = int(xRes*0.34375f);
				int xadd   = 20*xRes/320;
				if(LetterBonus.L)  LD3.PutSprite(&WinApp, xstart, 50, -1, miL,  false); xstart += xadd;
				if(LetterBonus.A)  LD3.PutSprite(&WinApp, xstart, 50, -1, miA,  false); xstart += xadd;
				if(LetterBonus.R1) LD3.PutSprite(&WinApp, xstart, 50, -1, miR1, false); xstart += xadd;
				if(LetterBonus.R2) LD3.PutSprite(&WinApp, xstart, 50, -1, miR2, false); xstart += xadd;
				if(LetterBonus.Y)  LD3.PutSprite(&WinApp, xstart, 50, -1, miY,  false); xstart += xadd;
				
				if(GotBonus && LetterBonus.L && LetterBonus.A && LetterBonus.R1 && LetterBonus.R2 && LetterBonus.Y){
					//FSOUND_StopSound(8);
					//FSOUND_PlaySound(8,sndBonus);
					Score += 25000;					
				}
				if(LetterBonus.L && LetterBonus.A && LetterBonus.R1 && LetterBonus.R2 && LetterBonus.Y)
					WriteText(0, 70, "BONUS +25000", 90, DT_CENTER);
				BonusDisplayTimer--;
				/*if(BonusDisplayTimer <= 0){
					if(LetterBonus.L && LetterBonus.A && LetterBonus.R1 && LetterBonus.R2 && LetterBonus.Y){					
					LetterBonus.L  = false;
					LetterBonus.A  = false;
					LetterBonus.R1 = false;
					LetterBonus.R2 = false;
					LetterBonus.Y  = false;
					}
				}*/
				GotBonus = false;
			}
		//--------------------------------------------
		//- End Display Bonus Letters Captured
		//--------------------------------------------

		//--------------------------------------------
		//- Display Crystal Bonus
		//--------------------------------------------
			if(CrystalBonusDisplayTimer){
				memset(Text1, 0, 20);
				strcpy(Text1, "All ");
				strcat(Text1, ObjectName);
				strcat(Text1, " Collected");
				WriteText(0, 50, Text1, 15, DT_CENTER);
				WriteText(0, 70, "BONUS +50000", 90, DT_CENTER);
				CrystalBonusDisplayTimer--;
			}
		//--------------------------------------------
		//- End Display Crystal Bonus
		//--------------------------------------------	
}

void mnuTitle()
{
	//- Show and process the title
	int Arrow = 0;
	int Selection = 0;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = false;
	int Col[6];	int px[6];
	bool SlideIn = true;
	bool SlideOut = false;
	bool SlideChange = false;
	bool ResetSlide = false;
	
	Col[0] = 40;
	px[0] = 340;
	for(int i = 1; i <= 5; i++){
		Col[i] = 15;
		px[i] = 340+i*20;
	}

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		WriteText(px[0],  80, "New", Col[0], 0);
		WriteText(px[1],  90, "Load", Col[1], 0);
		WriteText(px[2], 100, "Options", Col[2], 0);
		WriteText(px[3], 110, "Scores", Col[3], 0);
		WriteText(px[4], 140, "Exit Game", Col[4], 0);
		WriteText(px[4]-100, 220, "Copyright(C) 2004", Col[5], 0);

		SlideChange = false;
		if(SlideIn){
			for(int i = 0; i <= 5; i++){
				if(px[i] > 200){
					px[i] -= (8-i)<<1;
					SlideChange = true;
					if(px[i] < 200) px[i] = 200;
				}
			}
		}
		else if(SlideOut){
			for(int i = 0; i <= 5; i++){
				if(px[i] < 420){
					px[i] += (i+4)<<1;
					SlideChange = true;
				}
			}
		}

		if(SlideChange == false){
			if(SlideIn) SlideIn = false;
			if(SlideOut){
				SlideOut = false;				
				if(Selection == 0){
					ResetSlide = true;
					if(mnuChooseSlot(false))
						break;
					else HoldTimer = HoldDelay;
				}
				if(Selection == 1){
					ResetSlide = true;
					if(mnuChooseSlot(true))
						break;
					else
						HoldTimer = HoldDelay;
				}
				if(Selection == 4){EndGame = true; break;}
				if(Selection == 2){mnuOptions(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 3){mnuScores();  HoldTimer = HoldDelay; ResetSlide = true;}
				if(ResetSlide){
					px[0] = 340;
					for(int i = 1; i <= 4; i++) px[i] = 340+i*20;
					SlideIn = true;
					ResetSlide = false;
				}
				if(Selection == 5){EndGame = true; break;}				
			}
		}

		//LD3.PutSprite(&WinApp, 120, Arrow, -1, 59+(int)ArrowAni, false);



		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
#if 0
			//TODO(davidgow): Joystick support.
			WinApp.lpdikey->GetDeviceState(256, Keyboard);
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#else
			memset(&JoyState, 0, sizeof(DIJOYSTATE));
#endif

			HoldTimer--;
			PlaySound = false;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				//if(Keyboard[SDL_SCANCODE_ESCAPE]){Selection = 5; SlideOut = true; SlideIn = false;} //{EndGame = true; break;}
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]){
					Selection = Arrow;
					SlideOut = true;
				}
			
				if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
					Col[Arrow] = 15;
					Arrow -= 1;
					HoldTimer = HoldDelay;
					if(Arrow < 0) Arrow = 0; else PlaySound = true;
					Col[Arrow] = 40;					
				}
				if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
					Col[Arrow] = 15;
					Arrow += 1;
					HoldTimer = HoldDelay;
					if(Arrow > 4) Arrow = 4; else PlaySound = true;
					Col[Arrow] = 40;					
				}				
			}

			if(PlaySound){				
				FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
			}
		}
	}
}

void mnuScores()
{
	//- Show the hiscores
	int Arrow = 200;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = false;
	int  yScroll = 240;
	int Col[15]; int px[15];
	bool SlideIn = true;
	bool SlideOut = false;
	bool SlideChange = false;
	bool ResetSlide = false;

	char Initial[40];
	char HiScore[200];
	//int  score;

	Col[0] = 15;
	px[0] = 340;
	for(int i = 1; i <= 10; i++){
		Col[i] = 15;
		px[i] = 340+i*4;
	}

	ifstream ScoreFile;

	//- Load in the scorefile
	ScoreFile.open("save/hiscores.txt", ios::binary);

		//- get the initials
		memset(Initial, 0, 40);
		for(int i = 1; i <= 10; i++)			
			ReadWord(&ScoreFile, &Initial[i*4]);
		//- get the scores
		memset(HiScore, 0, 200);
		for(int i = 1; i <= 10; i++){
			//score = ReadValue(&ScoreFile);			
			//itoa(score, &Score[i*20], 10);
			ReadWord(&ScoreFile, &HiScore[i*20]);
		}
	ScoreFile.close();

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		//WriteBigFont(50, 5+yScroll, "Hi Scores");
		
		/*for(int i = 0; i <= 9; i++){
			WriteBigFont(60,  i*20+30+yScroll, &Initial[i*4]);
			WriteBigFont(180, i*20+30+yScroll, &HiScore[i*20]);
		}		
		WriteBigFont(120, 240, "Back");
		LD3.PutSprite(&WinApp, 100, Arrow, -1, 59+(int)ArrowAni, false);

		yScroll--;
		if(yScroll < 0) yScroll = 0;*/

		WriteText(px[0]+20, 50, "Hi Scores", 15, 0);
		for(int i = 1; i <= 10; i++){
			WriteText(px[i], 64+i*10, &Initial[i<<2], Col[i], 0);
			WriteText(px[i]+64, 64+i*10, &HiScore[i*20], 14, 0);			
		}	

		SlideChange = false;
		if(SlideIn){
			for(int i = 0; i <= 10; i++){
				if(px[i] > 100){
					px[i] -= (14-i)<<1;
					SlideChange = true;
					if(px[i] < 100) px[i] = 100;
				}
			}
		}
		else if(SlideOut){
			for(int i = 0; i <= 10; i++){
				if(px[i] < 360){
					px[i] += (i+4)<<1;
					SlideChange = true;
				}
			}
		}

		if(SlideChange == false){
			if(SlideIn) SlideIn = false;
			if(SlideOut){
				SlideOut = false;
				break;
			}
		}


		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			//TODO(davidgow): Joystick support
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			HoldTimer--;
			PlaySound = false;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				if(Keyboard[SDL_SCANCODE_ESCAPE]) SlideOut = true;
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]) SlideOut = true;				
			}

			if(PlaySound) FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);			
		}
	}
}

void EnterHiScore()
{
	char Initial[100];
	char PlayerInitial[4];
	char cHiScore[200];
	int  HiScore[11];
	int  PlayerRank = -1;
	int  InitialCount = 0;
	char *Letter = &PlayerInitial[InitialCount];
	int  HoldCount = 0;
	int  BlinkTimer = 0;
	bool PlaySound = false;
	char SingleInitial[3];
	HiScore[10] = 0;
	//int  score;
	memcpy(PlayerInitial, "AAA", 3);
	memcpy(SingleInitial, "A ", 3);
	memset(cHiScore, 0, 200);

	LD3.LoadTileSet(&WinApp, "gfx/ld3font.put", 0);
	LD3.LoadMap("maps/blank.ld3");
	
	ifstream ScoreFile;
	ofstream ScoreFile2;

	//- Load in the scorefile
	ScoreFile.open("save/hiscores.txt", ios::binary);

		//- get the initials
		memset(Initial, 0, 100);
		for(int i = 0; i <= 9; i++)			
			ReadWord(&ScoreFile, &Initial[i*10]);
		//- get the scores		
		for(int i = 0; i <= 9; i++)
			HiScore[i] = ReadValue(&ScoreFile);
	ScoreFile.close();

	//- Check if the player's score is within the hiscores

	for(int i = 0; i <= 9; i++){
		if(Score > HiScore[i] && i == 0) PlayerRank = 0;
		if(Score < HiScore[i] && Score > HiScore[i+1] && i < 9) PlayerRank = i+1;
		if(Score == HiScore[i] && Score > HiScore[i+1] && i < 9) PlayerRank = i+1;
		//if(Score < HiScore[i] && Score == HiScore[i+1] && i < 9) PlayerRank = i+2;
	}

	
	while(PlayerRank > -1)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}

		
		LD3.ClearBuffer(&WinApp, 0);
			
		WriteBigFont(70,  15, "New Hi Score");
		WriteBigFont(60, 80, "Enter Initials");

		SingleInitial[0] = PlayerInitial[0];
		if(InitialCount != 0 || (InitialCount == 0 && (BlinkTimer & 15) < 11)) WriteBigFont(130, 110, SingleInitial);
		SingleInitial[0] = PlayerInitial[1];
		if(InitialCount != 1 || (InitialCount == 1 && (BlinkTimer & 15) < 11)) WriteBigFont(150, 110, SingleInitial);
		SingleInitial[0] = PlayerInitial[2];
		if(InitialCount != 2 || (InitialCount == 2 && (BlinkTimer & 15) < 11)) WriteBigFont(170, 110, SingleInitial);

		BlinkTimer++;
		if(BlinkTimer > 1023) BlinkTimer = 0;

		LD3.FlipSurfaces(&WinApp);

		Keyboard = SDL_GetKeyboardState(0);
#if 0
		//TODO(davidgow): Joystick
		if(WinApp.JoyStickEnabled){
			WinApp.lpdijoy[JoyOne]->Poll();
			WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
		}
#endif

		if(Keyboard[SDL_SCANCODE_Q]) break;
		HoldCount--;
		if(HoldCount <= 0){
			HoldCount = 0;
			if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
				*Letter -= 1;
				if(*Letter < 65) *Letter = 90;
				HoldCount = 10;
				PlaySound = true;
			}
			if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
				*Letter += 1;				
				if(*Letter > 90) *Letter = 65;
				HoldCount = 10;
				PlaySound = true;
			}
			if(Keyboard[SDL_SCANCODE_LEFT] || JoyState.lX < -dpad){
				InitialCount -= 1;
				if(InitialCount < 0) InitialCount = 0;
				Letter = &PlayerInitial[InitialCount];
				HoldCount = 10;
				PlaySound = true;
			}
			if(Keyboard[SDL_SCANCODE_RIGHT] || JoyState.lX > dpad){
				InitialCount += 1;
				if(InitialCount > 2) InitialCount = 2;
				Letter = &PlayerInitial[InitialCount];
				HoldCount = 10;
				PlaySound = true;
			}
			if(Keyboard[SDL_SCANCODE_RETURN] || Keyboard[SDL_SCANCODE_SPACE] || JoyState.rgbButtons[0]){
				break;
			}
		}

		if(PlaySound){
			FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
			PlaySound = false;
		}				
	}

	if(PlayerRank > -1){

		//- Insert the player's score within the rankings
		for(int i = 8; i >= PlayerRank; i--){
			memcpy(&Initial[(i+1)*10], &Initial[i*10], 4);
			HiScore[i+1] = HiScore[i];
		}		
		//memset(PlayerInitial, 0, 4);
		memcpy(&Initial[PlayerRank*10], PlayerInitial, 4);
		HiScore[PlayerRank] = Score;
		
		//- save the score data
		ScoreFile2.open("save/hiscores.txt", ios::binary);

			//- write the initials
			//memset(Initial, 0, 40);
			for(int i = 0; i <= 9; i++)				
				PutString(&ScoreFile2, &Initial[i*10]);
			//- write the scores		
			for(int i = 0; i <= 9; i++){
				sprintf(&cHiScore[i*20], "%d", HiScore[i]);
				PutString(&ScoreFile2, &cHiScore[i*20]);
			}

		ScoreFile2.close();
		Score = 0;
		mnuScores();
	}

}

void mnuOptions()
{
	//- Show and process the options
	int Arrow = 0;
	int Selection = 0;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = true;
	int Col[5];	int px[5];
	bool SlideIn = true;
	bool SlideOut = false;
	bool SlideChange = false;
	bool ResetSlide = false;
	
	Col[0] = 40;
	px[0] = 340;
	for(int i = 1; i <= 4; i++){
		Col[i] = 15;
		px[i] = 340+i*20;
	}

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		/*WriteBigFont(140,  60, "Graphics");
		WriteBigFont(140,  80, "Sound");
		WriteBigFont(140, 100, "Controls");
		WriteBigFont(140, 120, "Credits");
		WriteBigFont(140, 160, "Back");

		LD3.PutSprite(&WinApp, 120, Arrow, -1, 59+(int)ArrowAni, false);*/

		WriteText(px[0],  80, "Graphics", Col[0], 0);
		WriteText(px[1],  90, "Sound", Col[1], 0);
		WriteText(px[2], 100, "Controls", Col[2], 0);
		WriteText(px[3], 110, "Credits", Col[3], 0);
		WriteText(px[4], 140, "Back", Col[4], 0);

		SlideChange = false;
		if(SlideIn){
			for(int i = 0; i <= 4; i++){
				if(px[i] > 200){
					px[i] -= (8-i)<<1;
					SlideChange = true;
					if(px[i] < 200) px[i] = 200;
				}
			}
		}
		else if(SlideOut){
			for(int i = 0; i <= 4; i++){
				if(px[i] < 400){
					px[i] += (i+4)<<1;
					SlideChange = true;
				}
			}
		}

		if(SlideChange == false){
			if(SlideIn) SlideIn = false;
			if(SlideOut){
				SlideOut = false;
				if(Selection == 0) break;
				if(Selection == 1){mnuSound(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 2){mnuControls(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 3){mnuCredits(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 4) break;
				//if(Selection == 5) break;
				//if(Arrow == 100) Options();
				//break;
				if(ResetSlide){
					px[0] = 340;
					for(int i = 1; i <= 4; i++) px[i] = 340+i*20;
					SlideIn = true;
					ResetSlide = false;
				}				
			}
		}

		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			PlaySound = false;
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			if(Keyboard[SDL_SCANCODE_ESCAPE]){
				Selection = 4; 
				SlideOut = true;
			}
			
			HoldTimer--;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]){
					Selection = Arrow;
					SlideOut = true;
				}
							
				if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
					Col[Arrow] = 15;
					Arrow -= 1;
					HoldTimer = HoldDelay;
					if(Arrow < 0) Arrow = 0; else PlaySound = true;
					Col[Arrow] = 40;					
				}
				if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
					Col[Arrow] = 15;
					Arrow += 1;
					HoldTimer = HoldDelay;
					if(Arrow > 4) Arrow = 4; else PlaySound = true;
					Col[Arrow] = 40;					
				}
							
				if(Arrow < 0){Arrow = 0; PlaySound = false;}
				if(Arrow > 4){Arrow = 4; PlaySound = false;}
			}

			if(PlaySound){				
				FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
				PlaySound = false;
			}
		}
	}
}

void mnuSound()
{
	//- Show and process the sound options
	int Arrow = 0;
	int Selection = Arrow;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = true;
	char text[20];
	int  temp = 0;
	double product = 100.0f/255.0f;
	int Col[5];	int px[5];
	bool SlideIn = true;
	bool SlideOut = false;
	bool SlideChange = false;
	bool ResetSlide = false;
	
	Col[0] = 40;
	px[0] = 340;
	for(int i = 1; i <= 4; i++){
		Col[i] = 15;
		px[i] = 340+i*20;
	}

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		/*WriteBigFont(40,  60, "Music Volume");
		memset(text, 0, 20);
		temp = (float)maxMusicVolume*product;
		itoa(temp, text, 10);
		WriteBigFont(260, 60, text);
		WriteBigFont(40,  80, "Sound Volume");
		memset(text, 0, 20);
		temp = (float)sfxVolume*product;
		itoa(temp, text, 10);
		WriteBigFont(260,  80, text);
		WriteBigFont(40, 140, "Back");

		LD3.PutSprite(&WinApp, 20, Arrow, -1, 59+(int)ArrowAni, false);*/

		WriteText(px[0], 80, "Music Volume", Col[0], 0);
		WriteText(px[1], 90, "SFX Volume", Col[1], 0);
		WriteText(px[2], 120, "Back", Col[2], 0);

		memset(text, 0, 20);
		temp = (float)maxMusicVolume*product;
		sprintf(text, "%d", temp);
		WriteText(px[0]+100, 80, text, 14, 0);

		memset(text, 0, 20);
		temp = (float)sfxVolume*product;
		sprintf(text, "%d", temp);
		WriteText(px[1]+100, 90, text, 14, 0);
		

		SlideChange = false;
		if(SlideIn){
			for(int i = 0; i <= 4; i++){
				if(px[i] > 200){
					px[i] -= (8-i)<<1;
					SlideChange = true;
					if(px[i] < 200) px[i] = 200;
				}
			}
		}
		else if(SlideOut){
			for(int i = 0; i <= 4; i++){
				if(px[i] < 400){
					px[i] += (i+4)<<1;
					SlideChange = true;
				}
			}
		}

		if(SlideChange == false){
			if(SlideIn) SlideIn = false;
			if(SlideOut){
				SlideOut = false;
				if(Selection == 0) break;
				if(Selection == 1){mnuSound(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 2){mnuControls(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 3){mnuCredits(); HoldTimer = HoldDelay; ResetSlide = true;}
				if(Selection == 4) break;
				//if(Selection == 5) break;
				//if(Arrow == 100) Options();
				//break;
				if(ResetSlide){
					px[0] = 340;
					for(int i = 1; i <= 4; i++) px[i] = 340+i*20;
					SlideIn = true;
					ResetSlide = false;
				}				
			}
		}


		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			PlaySound = false;
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			if(Keyboard[SDL_SCANCODE_ESCAPE]) SlideOut = true;
			
			HoldTimer--;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]) SlideOut = true;
				
				if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
					Col[Arrow] = 15;
					Arrow += 1;
					HoldTimer = HoldDelay;
					if(Arrow > 2) Arrow = 2; else PlaySound = true;
					Col[Arrow] = 40;					
				}
				if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
					Col[Arrow] = 15;
					Arrow -= 1;
					HoldTimer = HoldDelay;
					if(Arrow < 0) Arrow = 0; else PlaySound = true;
					Col[Arrow] = 40;					
				}
				if(Keyboard[SDL_SCANCODE_RIGHT] || JoyState.lX > dpad){
					if(Arrow == 0) maxMusicVolume += 32;
					if(Arrow == 1) sfxVolume += 32;
					if(maxMusicVolume > 255) maxMusicVolume = 255;
					if(sfxVolume > 255) sfxVolume = 255;
					FMUSIC_SetMasterVolume(song1, maxMusicVolume);
					FSOUND_SetSFXMasterVolume(sfxVolume);
					PlaySound = true;
					HoldTimer = HoldDelay;
				}
				if(Keyboard[SDL_SCANCODE_LEFT] || JoyState.lX < -dpad){
					if(Arrow == 0) maxMusicVolume -= 32;
					if(Arrow == 1) sfxVolume -= 32;
					if(maxMusicVolume < 0) maxMusicVolume = 0;
					if(sfxVolume < 0) sfxVolume = 0;
					FMUSIC_SetMasterVolume(song1, maxMusicVolume);
					FSOUND_SetSFXMasterVolume(sfxVolume);
					PlaySound = true;
					HoldTimer = HoldDelay;
				}
							
				if(Arrow < 0){Arrow = 0; PlaySound = false;}
				if(Arrow > 2){Arrow = 2; PlaySound = false;}
			}

			if(PlaySound){				
				FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
				PlaySound = false;
			}
		}
	}
}

void mnuCredits()
{
	//- Show the hiscores
	int Arrow = 200;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = false;
	int  yScroll = 240;
	bool SlideIn = true;
	bool SlideOut = false;
	bool SlideChange = false;
	bool ResetSlide = false;
	int px[20];

	for(int i = 0; i <= 9; i++) px[i] = 340+i*20;

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		if(px[0] < 330){WriteText(px[0],  64, "Programming", 15, 0);	WriteText(px[0]+110, 64, "Joe King", 14, 0);}
		if(px[1] < 330){WriteText(px[1],  74, "Graphics", 15, 0);		WriteText(px[1]+110, 74, "Joe King", 14, 0);}
		if(px[2] < 330){WriteText(px[2],  84, "Music/SFX", 15, 0);		WriteText(px[2]+110, 84, "Joe King", 14, 0);}
		if(px[3] < 330){WriteText(px[3],  94, "Level Design", 15, 0);	WriteText(px[3]+110, 94, "Joe King", 14, 0);}
		if(px[4] < 330) WriteText(px[4], 114, "Testing", 15, 0);
		if(px[4] < 230){
			WriteText(px[4]+110, 114, "Kendall Chapman", 14, 0);
			WriteText(px[5]+110, 124, "Lasse Hassing", 14, 0);
			WriteText(px[6]+110, 134, "Nexinarus", 14, 0);
		}

		if(px[7] < 330) WriteText(px[6], 154, "Special Thanks", 15, 0);
		if(px[8] < 230){
			WriteText(px[8]+110, 154, "Pixel Plus 256", 14, 0);		
			WriteText(px[9]+110, 164, "Modplug Tracker", 14, 0);
		}

		SlideChange = false;
		if(SlideIn){
			for(int i = 0; i <= 9; i++){
				if(px[i] > 60){
					px[i] -= (14-i)<<1;
					SlideChange = true;
					if(px[i] < 60) px[i] = 60;
				}
			}
		}
		else if(SlideOut){
			for(int i = 0; i <= 9; i++){
				if(px[i] < 360){
					px[i] += (i+4)<<1;
					SlideChange = true;
				}
			}
		}

		if(SlideChange == false){
			if(SlideIn) SlideIn = false;
			if(SlideOut){
				SlideOut = false;
				break;
			}
			//WriteText(64, 180, "Copyright(c) 2004 - Delta Code", 15, 0);
			//WriteText(84, 190, "name subject to change", 15, 0);
			//WriteText(40, 210, "materials used in this game cannot", 15, 0);
			//WriteText(20, 220, "be used without verbal or written consent", 15, 0);			
		}


		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			HoldTimer--;
			PlaySound = false;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				if(Keyboard[SDL_SCANCODE_ESCAPE]) SlideOut = true;
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]) SlideOut = true;				
			}

			if(PlaySound) FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);			
		}
	}
}

void mnuCredits2()
{
	//- Show the credits
	float yScroll = 250;
	char Credits[8000];
	int center;
	int list;
	int i = 0;
	char TileSetFilename[80];

	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);

	strcpy(PaletteFilename, "gfx/palettes/grad.pal");
	LD3.LoadMap("maps/blank.ld3");
	strcpy(TileSetFilename, "gfx/ld3font.put");

	LD3.LoadTileSet(&WinApp, "gfx/blank10.put", 240);
	LD3.LoadTileSet(&WinApp, TileSetFilename, 0);
	LD3.LoadPalette(&WinApp, PaletteFilename);

	memcpy(FadePalette, WinApp.palette, sizeof(SDL_Color)*256);
	
	SDL_FreePalette(WinApp.lpddpal);
	WinApp.lpddpal = SDL_AllocPalette(256);
	SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
	SDL_SetSurfacePalette(WinApp.lpddsback, WinApp.lpddpal);
	

	memset(Credits, 0, 8000);

	strcpy(&Credits[i], "Presented by"); i += 80;
	strcpy(&Credits[i], "Delta Code"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Story"); i += 80;
	strcpy(&Credits[i], "Joe King"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Programming"); i += 80;
	strcpy(&Credits[i], "Joe King"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Graphics"); i += 80;
	strcpy(&Credits[i], "Joe King"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Music And Sound"); i += 80;
	strcpy(&Credits[i], "Joe King"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Level Design"); i += 80;
	strcpy(&Credits[i], "Joseph Nephi King"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Testing"); i += 80;
	strcpy(&Credits[i], "Joe King"); i += 80;
	strcpy(&Credits[i], "Kendall Chapman"); i += 80;
	strcpy(&Credits[i], "Lasse Hassing"); i += 80;
	strcpy(&Credits[i], "Nexinarus"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Special Thanks"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "Chris Chadwicks"); i += 80;
	strcpy(&Credits[i], "Pixel Plus 256"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], "ModPlug Tracker"); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;	
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;
	strcpy(&Credits[i], ""); i += 80;	
	strcpy(&Credits[i], "Larry The Dinosaur3"); i += 80;

	
	
	
	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.ClearBuffer(&WinApp, 0);
		//LD3.DrawSky(&WinApp, xShift, 0);
		//LD3.DrawMap(&WinApp, 0, 0, 0);

		for(int i = 0; i <= 98; i++){
			center = strlen(&Credits[i*80])*16;
			center = (xRes-center)/2;
			
			WriteBigFont(center,  i*20+30+(int)yScroll, &Credits[i*80]);			
		}
		yScroll -= 0.5f;

		if(yScroll < -1850) yScroll = -1850;

		xShift += 4;
		if(xShift > 32767) xShift = 0;
		
		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			if(Keyboard[SDL_SCANCODE_ESCAPE]) break;		
		}

		LD3.FlipSurfaces(&WinApp);
	}
}

void mnuControls()
{
	//- Show and process the custom controls
	int HoldTimer = HoldDelay;
	bool PlaySound = true;
	int ArrowX = 120;
	int ArrowY = 40;
	
	char *KeyMsg, *JoyMsg;
	char InputDevice[80];
	char Sensitivity[4];
	UCHAR *KeyPtr[2][20];

	KeyPtr[0][0] = &keyJump;	KeyPtr[1][0] = &keyJump2;
	KeyPtr[0][1] = &keyShoot;	KeyPtr[1][1] = &keyShoot2;
	KeyPtr[0][2] = &keyGrenade;	KeyPtr[1][2] = &keyGrenade2;
	KeyPtr[0][3] = &keyAim;		KeyPtr[1][3] = &keyAim2;
	KeyPtr[0][4] = &keyNoFlip;	KeyPtr[1][4] = &keyNoFlip2;
	KeyPtr[0][5] = &keyNoMove;	KeyPtr[1][5] = &keyNoMove2;

	KeyPtr[0][7]  = &joyJump;	 KeyPtr[1][7]  = &joyJump2;
	KeyPtr[0][8]  = &joyShoot;	 KeyPtr[1][8]  = &joyShoot2;
	KeyPtr[0][9]  = &joyGrenade; KeyPtr[1][9]  = &joyGrenade2;
	KeyPtr[0][10] = &joyAim;	 KeyPtr[1][10] = &joyAim2;
	KeyPtr[0][11] = &joyNoFlip;	 KeyPtr[1][11] = &joyNoFlip2;
	KeyPtr[0][12] = &joyNoMove;	 KeyPtr[1][12] = &joyNoMove2;
	
	KeyMsg = (char*)calloc(4000, sizeof(char));
	JoyMsg = (char*)calloc(200, sizeof(char));

	// TODO(davidgow): SDL has the SDL_GetScancodeName() function which could replace all of this!
	strcpy(&KeyMsg[SDL_SCANCODE_A<<4], "A");	strcpy(&KeyMsg[SDL_SCANCODE_B<<4], "B");	strcpy(&KeyMsg[SDL_SCANCODE_C<<4], "C");	strcpy(&KeyMsg[SDL_SCANCODE_D<<4], "D");
	strcpy(&KeyMsg[SDL_SCANCODE_E<<4], "E");	strcpy(&KeyMsg[SDL_SCANCODE_F<<4], "F");	strcpy(&KeyMsg[SDL_SCANCODE_G<<4], "G");	strcpy(&KeyMsg[SDL_SCANCODE_H<<4], "H");
	strcpy(&KeyMsg[SDL_SCANCODE_I<<4], "I");	strcpy(&KeyMsg[SDL_SCANCODE_J<<4], "J");	strcpy(&KeyMsg[SDL_SCANCODE_K<<4], "K");	strcpy(&KeyMsg[SDL_SCANCODE_L<<4], "L");
	strcpy(&KeyMsg[SDL_SCANCODE_M<<4], "M");	strcpy(&KeyMsg[SDL_SCANCODE_N<<4], "N");	strcpy(&KeyMsg[SDL_SCANCODE_O<<4], "O");	strcpy(&KeyMsg[SDL_SCANCODE_P<<4], "P");
	strcpy(&KeyMsg[SDL_SCANCODE_Q<<4], "Q");	strcpy(&KeyMsg[SDL_SCANCODE_R<<4], "R");	strcpy(&KeyMsg[SDL_SCANCODE_S<<4], "S");	strcpy(&KeyMsg[SDL_SCANCODE_T<<4], "T");
	strcpy(&KeyMsg[SDL_SCANCODE_U<<4], "U");	strcpy(&KeyMsg[SDL_SCANCODE_V<<4], "V");	strcpy(&KeyMsg[SDL_SCANCODE_W<<4], "W");	strcpy(&KeyMsg[SDL_SCANCODE_X<<4], "X");
	strcpy(&KeyMsg[SDL_SCANCODE_Y<<4], "Y");	strcpy(&KeyMsg[SDL_SCANCODE_Z<<4], "Z");
	strcpy(&KeyMsg[SDL_SCANCODE_1<<4], "1");	strcpy(&KeyMsg[SDL_SCANCODE_2<<4], "2");	strcpy(&KeyMsg[SDL_SCANCODE_3<<4], "3");	strcpy(&KeyMsg[SDL_SCANCODE_4<<4], "4");
	strcpy(&KeyMsg[SDL_SCANCODE_5<<4], "5");	strcpy(&KeyMsg[SDL_SCANCODE_6<<4], "6");	strcpy(&KeyMsg[SDL_SCANCODE_7<<4], "7");	strcpy(&KeyMsg[SDL_SCANCODE_8<<4], "8");
	strcpy(&KeyMsg[SDL_SCANCODE_9<<4], "9");	strcpy(&KeyMsg[SDL_SCANCODE_0<<4], "0");	strcpy(&KeyMsg[SDL_SCANCODE_MINUS<<4], "-");strcpy(&KeyMsg[SDL_SCANCODE_EQUALS<<4], "=");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_1<<4], "NUMPAD 1");	strcpy(&KeyMsg[SDL_SCANCODE_KP_2<<4], "NUMPAD 2");	strcpy(&KeyMsg[SDL_SCANCODE_KP_3<<4], "NUMPAD 3");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_4<<4], "NUMPAD 4");	strcpy(&KeyMsg[SDL_SCANCODE_KP_5<<4], "NUMPAD 5");	strcpy(&KeyMsg[SDL_SCANCODE_KP_6<<4], "NUMPAD 6");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_7<<4], "NUMPAD 7");	strcpy(&KeyMsg[SDL_SCANCODE_KP_8<<4], "NUMPAD 8");	strcpy(&KeyMsg[SDL_SCANCODE_KP_9<<4], "NUMPAD 9");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_0<<4], "NUMPAD 0");
	strcpy(&KeyMsg[SDL_SCANCODE_TAB<<4], "TAB");
	strcpy(&KeyMsg[SDL_SCANCODE_BACKSPACE<<4], "BACKSPACE");
	strcpy(&KeyMsg[SDL_SCANCODE_RETURN<<4], "ENTER");
	strcpy(&KeyMsg[SDL_SCANCODE_LEFTBRACKET<<4], "[");				strcpy(&KeyMsg[SDL_SCANCODE_RIGHTBRACKET<<4], "]");	
	strcpy(&KeyMsg[SDL_SCANCODE_LCTRL<<4], "LEFT CONTROL");	strcpy(&KeyMsg[SDL_SCANCODE_RCTRL<<4], "RIGHT CONTROL");
	strcpy(&KeyMsg[SDL_SCANCODE_LALT<<4], "LEFT ALT");			strcpy(&KeyMsg[SDL_SCANCODE_RALT<<4], "RIGHT ALT");
	strcpy(&KeyMsg[SDL_SCANCODE_LSHIFT<<4], "LEFT SHIFT");		strcpy(&KeyMsg[SDL_SCANCODE_RSHIFT<<4], "RIGHT SHIFT");
	strcpy(&KeyMsg[SDL_SCANCODE_SEMICOLON<<4], ";");
	strcpy(&KeyMsg[SDL_SCANCODE_APOSTROPHE<<4], "'");
	strcpy(&KeyMsg[SDL_SCANCODE_GRAVE<<4], "`");
	strcpy(&KeyMsg[SDL_SCANCODE_COMMA<<4], ",");
	strcpy(&KeyMsg[SDL_SCANCODE_PERIOD<<4], ".");
	strcpy(&KeyMsg[SDL_SCANCODE_SLASH<<4], "/");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_MULTIPLY<<4], "NUMPAD *");
	strcpy(&KeyMsg[SDL_SCANCODE_SPACE<<4], "SPACEBAR");
	strcpy(&KeyMsg[SDL_SCANCODE_CAPSLOCK<<4], "CAPS LOCK");
	strcpy(&KeyMsg[SDL_SCANCODE_NUMLOCKCLEAR<<4], "NUMLOCK");
	strcpy(&KeyMsg[SDL_SCANCODE_SCROLLLOCK<<4], "SCROLL LOCK");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_MINUS<<4], "NUMPAD -");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_PLUS<<4], "NUMPAD +");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_DECIMAL<<4], "NUMPAD .");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_ENTER<<4], "NUMPAD ENTER");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_COMMA<<4], "NUMPAD COMMA");
	strcpy(&KeyMsg[SDL_SCANCODE_KP_DIVIDE<<4], "NUMPAD /");
	strcpy(&KeyMsg[SDL_SCANCODE_SYSREQ<<4], "SYSRQ");
	strcpy(&KeyMsg[SDL_SCANCODE_HOME<<4], "HOME");
	strcpy(&KeyMsg[SDL_SCANCODE_END<<4], "END");
	strcpy(&KeyMsg[SDL_SCANCODE_PAGEUP<<4], "PAGE UP");
	strcpy(&KeyMsg[SDL_SCANCODE_PAGEDOWN<<4], "PAGE DOWN");
	strcpy(&KeyMsg[SDL_SCANCODE_UP<<4], "UP ARROW");		strcpy(&KeyMsg[SDL_SCANCODE_DOWN<<4], "DOWN ARROW");
	strcpy(&KeyMsg[SDL_SCANCODE_LEFT<<4], "LEFT ARROW");	strcpy(&KeyMsg[SDL_SCANCODE_RIGHT<<4], "RIGHT ARROW");
	strcpy(&KeyMsg[SDL_SCANCODE_INSERT<<4], "INSERT");
	strcpy(&KeyMsg[SDL_SCANCODE_DELETE<<4], "DELETE");	
	strcpy(&KeyMsg[0], "UNDEFINED");	
	
	strcpy(&JoyMsg[0<<4],  "BUTTON 0");
	strcpy(&JoyMsg[1<<4],  "BUTTON 1");
	strcpy(&JoyMsg[2<<4],  "BUTTON 2");
	strcpy(&JoyMsg[3<<4],  "BUTTON 3");
	strcpy(&JoyMsg[4<<4],  "BUTTON 4");
	strcpy(&JoyMsg[5<<4],  "BUTTON 5");
	strcpy(&JoyMsg[6<<4],  "BUTTON 6");
	strcpy(&JoyMsg[7<<4],  "BUTTON 7");
	strcpy(&JoyMsg[8<<4],  "BUTTON 8");
	strcpy(&JoyMsg[9<<4],  "BUTTON 9");
	strcpy(&JoyMsg[10<<4], "BUTTON 10");
	strcpy(&JoyMsg[11<<4], "BUTTON 11");
	strcpy(&JoyMsg[12<<4], "BUTTON 12");
	


	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		//LD3.DrawMap(&WinApp, 0, 0, 0);

		//WriteBigFont(90,  6, "Keyboard");
		//LD3.DrawBox(&WinApp, 0, 0, xRes-1, yRes-1, 1, true);		
		WriteText(0, 5,  "KEYBOARD", 15, DT_CENTER);
		LD3.DrawBox(&WinApp, 0, 15, xRes-1, 15, 15, false);
		
		WriteText(120, 20,  "PRIMARY", 15, 0);	WriteText(220, 20,  "ALTERNATE", 15, 0);

		if(ArrowY <= 90){
			//LD3.DrawBox(&WinApp, ArrowX-2, ArrowY-2, ArrowX+90, ArrowY+6, 8, true);
			LD3.DrawBox(&WinApp, ArrowX-2, ArrowY-2, ArrowX+90, ArrowY+6, 15, false);
		}		
		else if(ArrowY > 100){
			//LD3.DrawBox(&WinApp, ArrowX-2, ArrowY+38, ArrowX+90, ArrowY+46, 8, true);
			LD3.DrawBox(&WinApp, ArrowX-2, ArrowY+38, ArrowX+90, ArrowY+46, 15, false);
		}
		else{
			//LD3.DrawBox(&WinApp, 5, ArrowY+28, 314, ArrowY+36, 8, true);
			LD3.DrawBox(&WinApp, 5, ArrowY+28, 314, ArrowY+36, 15, false);
		}

		WriteText(20, 40,  "JUMP", 14, 0);
		WriteText(20, 50,  "SHOOT", 14, 0);
		WriteText(20, 60,  "THROW GRENADE", 14, 0);
		WriteText(20, 70,  "AIM GRENADE", 14, 0);
		WriteText(20, 80,  "DON'T TURN", 14, 0);
		WriteText(20, 90,  "HOLD STILL", 14, 0);
		
		WriteText(120, 40,   &KeyMsg[keyJump<<4], 7, 0);	WriteText(220, 40,   &KeyMsg[keyJump2<<4], 7, 0);
		WriteText(120, 50,   &KeyMsg[keyShoot<<4], 7, 0);	WriteText(220, 50,   &KeyMsg[keyShoot2<<4], 7, 0);
		WriteText(120, 60,   &KeyMsg[keyGrenade<<4], 7, 0);	WriteText(220, 60,   &KeyMsg[keyGrenade2<<4], 7, 0);
		WriteText(120, 70,   &KeyMsg[keyAim<<4], 7, 0);		WriteText(220, 70,   &KeyMsg[keyAim2<<4], 7, 0);
		WriteText(120, 80,   &KeyMsg[keyNoFlip<<4], 7, 0);	WriteText(220, 80,   &KeyMsg[keyNoFlip2<<4], 7, 0);
		WriteText(120, 90,   &KeyMsg[keyNoMove<<4], 7, 0);	WriteText(220, 90,   &KeyMsg[keyNoMove2<<4], 7, 0);
				
		
		//WriteBigFont(6,  116, "JoyStick or Gamepad");
		WriteText(0, 110,  "Other Input Devices", 15, DT_CENTER);
		LD3.DrawBox(&WinApp, 0, 120, xRes-1, 120, 15, false);
		
		memset(InputDevice, 0, 80);
#if 0
		//TODO(davidgow): Joystick name (SDL supports getting these for Gamepads, at least)
		strcpy(InputDevice, (char *)&WinApp.joyname[JoyOne*80]);
#else
		strcpy(InputDevice, "[TODO: Joysticks]");
#endif
		WriteText(0, 130, InputDevice, 15, DT_CENTER);

		//WriteText(120, 140,  "PLAYER 1", 15, 0);	WriteText(220, 140,  "PLAYER2", 15, 0);
		if(WinApp.NumJoys > 0){
			WriteText(20, 150, "JUMP", 15, 0);
			WriteText(20, 160, "SHOOT", 15, 0);
			WriteText(20, 170, "THROW GRENADE", 15, 0);
			WriteText(20, 180, "AIM GRENADE", 15, 0);
			WriteText(20, 190, "DON'T TURN", 15, 0);
			WriteText(20, 200, "HOLD STILL", 15, 0);
			WriteText(20, 210, "SENSITIVITY", 15, 0);

			WriteText(120, 150, &JoyMsg[joyJump<<4], 7, 0);	  //WriteText(220, 150, &JoyMsg[joyJump2<<4], 7, 0);
			WriteText(120, 160, &JoyMsg[joyShoot<<4], 7, 0);  //WriteText(220, 160, &JoyMsg[joyShoot2<<4], 7, 0);
			WriteText(120, 170, &JoyMsg[joyGrenade<<4], 7, 0);//WriteText(220, 170, &JoyMsg[joyGrenade2<<4], 7, 0);
			WriteText(120, 180, &JoyMsg[joyAim<<4], 7, 0);	  //WriteText(220, 180, &JoyMsg[joyAim2<<4], 7, 0);
			WriteText(120, 190, &JoyMsg[joyNoFlip<<4], 7, 0); //WriteText(220, 190, &JoyMsg[joyNoFlip2<<4], 7, 0);
			WriteText(120, 200, &JoyMsg[joyNoMove<<4], 7, 0); //WriteText(220, 200, &JoyMsg[joyNoMove2<<4], 7, 0);
			memset(Sensitivity, 0, 4);
			sprintf(Sensitivity, "%d", (int)dpad);
			WriteText(120, 210, Sensitivity, 7, 0);
		}

		if(ArrowY != 100)
			WriteText(0, 230, "Press Esc to go back", 15, DT_CENTER);
		else
			WriteText(0, 230, "Press Left or Right to change input device", 15, DT_CENTER);

		//LD3.PutSprite(&WinApp, 120, Arrow, -1, 59+(int)ArrowAni, false);


		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			if(Keyboard[SDL_SCANCODE_ESCAPE]){

				ofstream sfile;

				sfile.open("controls.dat", ios::binary);

					sfile.put(keyJump);
					sfile.put(keyJump2);
					sfile.put(keyShoot);
					sfile.put(keyShoot2);
					sfile.put(keyGrenade);
					sfile.put(keyGrenade2);
					sfile.put(keyAim);
					sfile.put(keyAim2);
					sfile.put(keyNoFlip);
					sfile.put(keyNoFlip2);
					sfile.put(keyNoMove);
					sfile.put(keyNoMove2);

					sfile.put(joyJump);
					sfile.put(joyShoot);
					sfile.put(joyAim);
					sfile.put(joyGrenade);
					sfile.put(joyNoFlip);
					sfile.put(joyNoMove);
					sfile.put(dpad);

				sfile.close();
				
				break;
			}
			
			HoldTimer--;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				
				if(ArrowY <= 90){
					if(Keyboard[SDL_SCANCODE_UP] == 0 && Keyboard[SDL_SCANCODE_DOWN] == 0 && Keyboard[SDL_SCANCODE_LEFT] == 0 && Keyboard[SDL_SCANCODE_RIGHT] == 0){
						for(int ky = 1; ky <= 255; ky++){						
							if(Keyboard[ky] && strcmp(&KeyMsg[ky<<4], "") != 0){
								*KeyPtr[(ArrowX-120)/100][(ArrowY-40)/10] = ky;
								HoldTimer = HoldDelay;
								PlaySound = true;
							}
						}
						HoldTimer = HoldDelay;
					}
				}
				else if(ArrowY > 100 && ArrowY < 170){
					for(int jy = 0; jy <= 11; jy++){
						if(JoyState.rgbButtons[jy]){
							*KeyPtr[(ArrowX-120)/100][(ArrowY-40)/10] = jy;
							HoldTimer = HoldDelay;
							PlaySound = true;
						}
					}
				}
				
				if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
					ArrowY += 10;
					HoldTimer = HoldDelay;
					if(ArrowY > 170 && WinApp.NumJoys > 0)
						ArrowY = 170;
					else if(ArrowY > 90 && WinApp.NumJoys == 0)
						ArrowY = 90;
					else
						PlaySound = true;
					

					if(ArrowY > 100 && ArrowX > 120) ArrowX = 120;
				}
				if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
					ArrowY -= 10;
					HoldTimer = HoldDelay;
					if(ArrowY < 40)
						ArrowY = 40;
					else
						PlaySound = true;
				}
				if(Keyboard[SDL_SCANCODE_LEFT] || JoyState.lX < -dpad){
					if(ArrowY == 170){
						dpad -= 1;
						if(dpad < 1) dpad = 1;
					}					
					if(ArrowY == 100){
						JoyOne --;
						if(JoyOne < 0)
							JoyOne = 0;
						else
							PlaySound = true;
					}
					else{
						ArrowX -= 100;						
						if(ArrowX < 120)
							ArrowX = 120;
						else
							PlaySound = true;
					}
					HoldTimer = HoldDelay;
				}
				if(Keyboard[SDL_SCANCODE_RIGHT] || JoyState.lX > dpad){
					if(ArrowY == 170){
						dpad += 1;
						if(dpad > 9) dpad = 9;
					}					
					if(ArrowY == 100){
						JoyOne ++;
						if(JoyOne > WinApp.NumJoys-1)
							JoyOne = WinApp.NumJoys-1;
						else
							PlaySound = true;
					}
					else if(ArrowY < 100){
						ArrowX += 100;						
						if(ArrowX > 220)
							ArrowX = 220;
						else
							PlaySound = true;
					}
					HoldTimer = HoldDelay;
				}
								
			}

			if(PlaySound){				
				FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
				PlaySound = false;
			}
		}
	}

//	free(JoyMsg);
//	free(KeyMsg);
}

bool mnuChooseSlot(bool Loading)
{
	//- Show and process the title
	int Arrow = 80;
	int oldArrow = Arrow;
	int HoldTimer = HoldDelay;
	float ArrowAni = 0;
	bool PlaySound = true;
	int SlotStart = 0;
	int top;
	int NumDisplaySlots = yRes/80;
	int BottomSlot = 80+NumDisplaySlots*35;
	bool AskSure = false;
	int  AskResult = 0;
	
	char Filename[80];
	char Desc[40];
	char Word[20];
	
	struct tSave{
		int Level;
		int Lives;
		int Shells;
		int Bullets;
		int Rockets;
		int Grenades;
		int Shotgun;
		int Machinegun;
		int Flamethrower;
		int Bazooka;
		bool Empty;
	} SaveData[10];

	memset(SaveData, 0, sizeof(tSave)*10);

	ifstream sfile;

	//- Load Saved Data
	for(int i = 0; i <= 9; i++){
		
		memset(Filename, 0, 80);
		sprintf(Filename, "save/slot%d.txt", i);

		sfile.open(Filename, ios::binary);
		
			while(NO_ONE_CARES)
			{
				memset(Word, 0, 20);
				
				ReadWord(&sfile, Word); SDL_strupr(Word);

				if(strcmp(Word, "LEVEL") == 0)			SaveData[i].Level	= ReadValue(&sfile);
				if(strcmp(Word, "LIVES") == 0)			SaveData[i].Lives	= ReadValue(&sfile);
				if(strcmp(Word, "SHELLS") == 0)			SaveData[i].Shells	= ReadValue(&sfile);
				if(strcmp(Word, "BULLETS") == 0)		SaveData[i].Bullets	= ReadValue(&sfile);
				if(strcmp(Word, "ROCKETS") == 0)		SaveData[i].Rockets	= ReadValue(&sfile);
				if(strcmp(Word, "GRENADES") == 0)		SaveData[i].Grenades	= ReadValue(&sfile);
				if(strcmp(Word, "SHOTGUN") == 0)		SaveData[i].Shotgun		= ReadValue(&sfile);
				if(strcmp(Word, "MACHINEGUN") == 0)		SaveData[i].Machinegun	= ReadValue(&sfile);
				if(strcmp(Word, "FLAMETHROWER") == 0)	SaveData[i].Flamethrower= ReadValue(&sfile);
				if(strcmp(Word, "BAZOOKA") == 0)		SaveData[i].Bazooka		= ReadValue(&sfile);

				if(strcmp(Word, "END") == 0) break;
				if(strcmp(Word, "EMPTY") == 0){
					SaveData[i].Empty = true;
					break;
				}

			}
		
		sfile.close();
	}

	while(NO_ONE_CARES)
	{
		if(SDL_PollEvent(&msg))
		{
			if(msg.type == SDL_QUIT)
				break;
			WindowProc(&msg);
		}
		
		LD3.DrawSky(&WinApp, xShift, 0);
		LD3.DrawMap(&WinApp, 0, 0, 0, 1);

		//WriteBigFont(10,  40, "Choose Slot");
		if(Loading)
			WriteText(10, 20, "Load Game", 15, 0);
		else
			WriteText(10, 20, "New Game", 15, 0);

		WriteText(10, 40, "Choose Slot", 15, 0);
		
		for(int i = SlotStart; i <= SlotStart+NumDisplaySlots; i++){
			top = 80+(i-SlotStart)*35;
			if(SaveData[i].Empty)
				LD3.LoadMapIcon(&WinApp, "gfx/mapicons/empty.bmp", 39, top-3);
			else{
				LD3.PutSprite(&WinApp, 19, top+4, 0, 0, false);
				LD3.PutSprite(&WinApp, 19, top+4, 0, 6, false);
				LD3.PutSprite(&WinApp, 19, top+4, 0, 9, false);
				
				memset(Word, 0, 20); memset(Desc, 0, 40);
				sprintf(Desc, "LEVEL %d", SaveData[i].Level);
				//LD3.WriteText(&WinApp, 85, top, Desc, false, 15, true);
				WriteText(85, top, Desc, 15, 0);

				memset(Word, 0, 20); memset(Desc, 0, 40);
				sprintf(Desc, "LIVES %d", SaveData[i].Lives);
				//LD3.WriteText(&WinApp, 85, top+10, Desc, false, 15, true);
				WriteText(85, top+10, Desc, 15, 0);

				LD3.PutSprite(&WinApp, 140, top-15, -1, miBoxOfShells, false);
				memset(Word, 0, 20); memset(Desc, 0, 40);
				sprintf(Desc, "X%d", SaveData[i].Shells);
				//LD3.WriteText(&WinApp, 158, top, Desc, false, 15, true);
				WriteText(158, top, Desc, 15, 0);
				
				LD3.PutSprite(&WinApp, 180, top-14, -1, miGrenade, false);
				memset(Word, 0, 20); memset(Desc, 0, 40);
				sprintf(Desc, "X%d", SaveData[i].Grenades);
				//LD3.WriteText(&WinApp, 195, top, Desc, false, 15, true);
				WriteText(195, top, Desc, 15, 0);
				
				if(SaveData[i].Level == 1) LD3.LoadMapIcon(&WinApp, "gfx/mapicons/spikevalley.bmp", 39, top-3);
				if(SaveData[i].Level == 2) LD3.LoadMapIcon(&WinApp, "gfx/mapicons/fallevening.bmp", 39, top-3);
				if(SaveData[i].Level == 3) LD3.LoadMapIcon(&WinApp, "gfx/mapicons/oceandrop.bmp", 39, top-3);
				if(SaveData[i].Level == 4) LD3.LoadMapIcon(&WinApp, "gfx/mapicons/shinyforest.bmp", 39, top-3);
				if(SaveData[i].Level == 6) LD3.LoadMapIcon(&WinApp, "gfx/mapicons/frostbite.bmp", 39, top-3);

				if(SaveData[i].Level == 0) WriteText(220, top, "Home Base", 15, 0);
				if(SaveData[i].Level == 1) WriteText(220, top, "Spike Valley", 15, 0);
				if(SaveData[i].Level == 2) WriteText(220, top, "Fall Evening", 15, 0);
				if(SaveData[i].Level == 3) WriteText(220, top, "Treatment", 15, 0);
				if(SaveData[i].Level == 4) WriteText(220, top, "Shiny Forest", 15, 0);
				if(SaveData[i].Level == 5) WriteText(220, top, "Crossways", 15, 0);
				if(SaveData[i].Level == 6) WriteText(220, top, "Frostbite", 15, 0);
				if(SaveData[i].Level == 7) WriteText(220, top, "Other World", 15, 0);
				if(SaveData[i].Level == 8) WriteText(220, top, "The End", 15, 0);
				/*if(SaveData[i].Shotgun)
					LD3.PutSprite(&WinApp, 240, top+2, ICON_SPRITE_SET, iconShotgun, false);
				else
					LD3.PutSprite(&WinApp, 240, top+2, ICON_SPRITE_SET, iconShotgunMissing, false);
				if(SaveData[i].Machinegun)
					LD3.PutSprite(&WinApp, 240, top+17, ICON_SPRITE_SET, iconMachinegun, false);
				else
					LD3.PutSprite(&WinApp, 240, top+17, ICON_SPRITE_SET, iconMachinegunMissing, false);
				if(SaveData[i].Flamethrower)
					LD3.PutSprite(&WinApp, 260, top+2, ICON_SPRITE_SET, iconFlamethrower, false);
				else
					LD3.PutSprite(&WinApp, 260, top+2, ICON_SPRITE_SET, iconFlamethrowerMissing, false);
				if(SaveData[i].Bazooka)
					LD3.PutSprite(&WinApp, 260, top+17, ICON_SPRITE_SET, iconBazooka, false);
				else
					LD3.PutSprite(&WinApp, 260, top+17, ICON_SPRITE_SET, iconBazookaMissing, false);*/
			}
		}
		if(AskSure){
			LD3.DrawBox(&WinApp, 18, oldArrow-3, 38, oldArrow+28, 15, false);
			LD3.DrawBox(&WinApp, 38, oldArrow-3, 79, oldArrow+28, 15, false);
			LD3.DrawBox(&WinApp, 79, oldArrow-3, 310, oldArrow+28, 15, false);
			LD3.DrawBox(&WinApp, 55, 75, 275, 160, 1, true);
			LD3.DrawBox(&WinApp, 50, 70, 270, 155, 9, true);
			WriteText(60, 75, "Overwrite Slot?", 15, 0);
			WriteBigFont(60,  80, "Are You Sure");
			WriteBigFont(140,  105, "Yes");
			WriteBigFont(140,  125, "No");
			LD3.PutSprite(&WinApp, 120, Arrow+4, -1, 59+(int)ArrowAni, false);
		}
		else{
			LD3.PutSprite(&WinApp, 0, Arrow+4, -1, 59+(int)ArrowAni, false);
			LD3.DrawBox(&WinApp, 18, Arrow-3, 38, Arrow+28, 15, false);
			LD3.DrawBox(&WinApp, 38, Arrow-3, 79, Arrow+28, 15, false);
			LD3.DrawBox(&WinApp, 79, Arrow-3, 310, Arrow+28, 15, false);
		}		
		
		SlotNum = (Arrow-80)/35+SlotStart;

		LD3.FlipSurfaces(&WinApp);

		xShift += 4;
		if(xShift > 32767) xShift = 0;

		ArrowAni += 0.2f;
		if(ArrowAni >= 10) ArrowAni = 0;

		if(MusicFade < MusicVolume){
			MusicFade ++;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}
		if(MusicFade > MusicVolume){
			MusicFade --;
			FMUSIC_SetMasterVolume(song1, MusicFade);
		}		

		if(Active){
		//- Process the input
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif

			HoldTimer--;
			if(HoldTimer <= 0) HoldTimer = 0;
			if(HoldTimer <= 0){
				if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_LCTRL] || Keyboard[SDL_SCANCODE_RETURN] || JoyState.rgbButtons[0]){
					if(AskSure){
						if(Arrow == 100){
							FSOUND_PlaySound(FSOUND_FREE, diescum);
							Level	= 0;
							Lives	= 5;
							Shells	= 30;
							Bullets	= 50;
							Grenades= 5;
							Score	= 0;
							ScoreDisplay = 0;
							SaveGame();
							PlaySound = true;
							SlotNum = (oldArrow-80)/35+SlotStart;
							return(true);
						}
						else{
							AskSure = false;
							HoldTimer = HoldDelay;
							PlaySound = true;
							Arrow = oldArrow;
						}
					}
					else{
						if(Arrow == 200) break;					
						if(Loading){
							if(SaveData[SlotNum].Empty){
								FSOUND_PlaySound(FSOUND_FREE, sndDeath[1]);
								HoldTimer = HoldDelay;
							}
							else{
								FSOUND_PlaySound(FSOUND_FREE, diescum);
								Level    = SaveData[SlotNum].Level;
								Lives    = SaveData[SlotNum].Lives;
								Shells   = SaveData[SlotNum].Shells;
								Bullets  = SaveData[SlotNum].Bullets;
								Grenades = SaveData[SlotNum].Grenades;
								Score    = 0;
								ScoreDisplay = 0;
								return(true);
							}
						}
						else{
							if(SaveData[SlotNum].Empty){
								FSOUND_PlaySound(FSOUND_FREE, diescum);
								Level	= 0;
								Lives	= 5;
								Shells	= 30;
								Bullets	= 50;
								Grenades= 5;
								Score	= 0;
								ScoreDisplay = 0;
								SaveGame();
								return(true);
							}
							else{
								AskSure = true;
								oldArrow = Arrow;
								Arrow = 120;
								PlaySound = true;
								HoldTimer = HoldDelay;
							}
						}
					}
				}

				if(Keyboard[SDL_SCANCODE_ESCAPE] || JoyState.rgbButtons[3]) break;			
							
				if(AskSure){
					if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
						Arrow += 20;
						HoldTimer = HoldDelay;						
						PlaySound = true;
					}
					if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
						Arrow -= 20;
						HoldTimer = HoldDelay;
						PlaySound = true;
					}

					if(Arrow < 100){
						Arrow = 100;
						PlaySound = false;
					}
					if(Arrow > 120){
						Arrow = 120;
						PlaySound = false;
					}
				}
				else{
					if(Keyboard[SDL_SCANCODE_DOWN] || JoyState.lY > dpad){
						Arrow += 35;
						HoldTimer = HoldDelay;
						//if(Arrow == 140) Arrow = 160;
						PlaySound = true;
					}
					if(Keyboard[SDL_SCANCODE_UP] || JoyState.lY < -dpad){
						Arrow -= 35;
						HoldTimer = HoldDelay;
						//if(Arrow == 140) Arrow = 120;
						PlaySound = true;
					}
								
					if(Arrow < 80){
						Arrow = 80;
						SlotStart--;
						if(SlotStart < 0){
							SlotStart = 0;
							PlaySound = false;
						}
					}
					if(Arrow > BottomSlot){
						Arrow = BottomSlot;
						SlotStart++;
						if(SlotStart > 6){
							SlotStart = 6;
							PlaySound = false;
						}
					}
				}
			}

			if(PlaySound){				
				FSOUND_PlaySound(FSOUND_FREE,sndItemPickup);
				PlaySound = false;
			}
		}
	}
	return(false);
}

void SaveGame()
{
	//- Save the game in the selected slot
	char Filename[80];
	char Word[20];
	char Num[20];

	memset(Filename, 0, 80);
	sprintf(Filename, "save/slot%d.txt", SlotNum);

	ofstream sfile;
	sfile.open(Filename, ios::binary);
	
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Level %d", Level);
		PutString(&sfile, Word);

		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Lives %d", Lives);
		PutString(&sfile, Word);

		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Shotgun %d", HasShotgun);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Machinegun %d", HasMachinegun);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Flamethrower %d", HasFlamethrower);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Bazooka %d", HasBazooka);
		PutString(&sfile, Word);

		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Shells %d", Shells);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Bullets %d", Bullets);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Rockets %d", Rockets);
		PutString(&sfile, Word);
		memset(Word, 0, 20); memset(Num, 0, 20);
		sprintf(Word, "Grenades %d", Grenades);
		PutString(&sfile, Word);

		memset(Word, 0, 20); strcpy(Word, "END");
		PutString(&sfile, Word);

	sfile.close();
}

void PutString(ofstream *File, char *Text)
{
	//- Writes a character stream to the given file
	int Length;

	Length = strlen(Text);
	Text[Length+1] = 13;
	Text[Length+2] = 10;
	for(int i = 0; i <= Length+2; i++){
		File->put(Text[i]);
	}
}

void WriteBigFont(int x, int y, const char *text)
{
	int length, fx;
	char Text1[80];

	memset(Text1, 0, 80);
	strcpy(Text1, text);
	SDL_strupr(Text1);

	length = strlen(Text1);
	fx = x;
	
	for(int i = 0; i <= length; i++){
		if(Text1[i] >= 32 && Text1[i] <= 90)
			LD3.PutSprite(&WinApp, fx, y, -1, Text1[i]-32, false);
		
		fx += 17;

		if(Text1[i] == 'I') fx -= 10;
		if(Text1[i] < 60) fx -= 3;
	}	
}

void Intro()
{
	char TileSetFilename[80];

	//song1   = FMUSIC_LoadSong("music/scent.it");
	
	strcpy(PaletteFilename, "gfx/palettes/grad.pal");
	
	LD3.LoadFont("gfx/font1.put");
	LD3.LoadPalette(&WinApp, PaletteFilename);
	WinApp.CreatePalette();
	WinApp.SetPalette();

	memcpy(FadePalette, WinApp.palette, sizeof(SDL_Color)*256);
	
	SDL_FreePalette(WinApp.lpddpal);
	WinApp.lpddpal = SDL_AllocPalette(256);
	SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
	SDL_SetSurfacePalette(WinApp.lpddsback, WinApp.lpddpal);

	Intro1();
	//Intro2();

	/*FadeScreenToBlack();
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
	LD3.WriteText(&WinApp, 0, 80, "LOCATION: EARTH", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "TIME: CIRCA 65 MILLION BC", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(4); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "THE CRETACEOUS PERIOD", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	FMUSIC_PlaySong(song1); Wait(1);
	//LD3.WriteText(&WinApp, 0, 80, "THE MAASTRICHTIAN AGE OF THE SENONIAN EPOCH", true, 15, true); LD3.FlipSurfaces(&WinApp);
	//FadeScreenOutOfBlack();  Wait(5); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "DELTA CODE PRESENTS", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "A GAME BY JOE KING", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "LARRY THE DINOSAUR 3", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(4); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0); Wait(2);	
	LD3.WriteText(&WinApp, 0, 80, "HE MADE IT OUT ALIVE", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "HE THOUGHT IT WAS OVER", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "HE MADE A MISTAKE", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "THE PORTAL WAS LEFT ALONE", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0);
	LD3.WriteText(&WinApp, 0, 80, "DESTINY CALLS HIM AGAIN", true, 15, true); LD3.FlipSurfaces(&WinApp);
	FadeScreenOutOfBlack();  Wait(3); FadeScreenToBlack(); LD3.ClearBuffer(&WinApp, 0); Wait(2);
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);*/
	
	//FMUSIC_StopSong(song1);
}

void Intro1()
{
	int  i, n;
	bool state;
	char Text[800];
	
	memset(Text, 0, 800);

	i = 0; n = 0;
	strcpy(&Text[i], "EARTH"); i += 80;
	strcpy(&Text[i], "CIRCA 65 MILLION BC"); i += 80;
	strcpy(&Text[i], "THE CRETACEOUS PERIOD");

	FadeScreenToBlack();
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
	
	state = false;
	for(n = 0; n <= i; n += 80){
		LD3.WriteText(&WinApp, 0, 80, &Text[n], true, 15, true); LD3.FlipSurfaces(&WinApp);		
		state = FadeScreenOutOfBlack();	if(state) break;
		state = Wait(3);				if(state) break;
		state = FadeScreenToBlack();	if(state) break;
		LD3.ClearBuffer(&WinApp, 0);	
	}

	FadeScreenToBlack();
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
	Wait(1);
}

void Intro2()
{
	int  i, n;
	bool state;
	char Text[800];
	
	memset(Text, 0, 800);
	
	i = 0; n = 0;
	strcpy(&Text[i], "DELTA CODE PRESENTS"); i += 80;
	strcpy(&Text[i], "A GAME BY JOE KING"); i += 80;
	strcpy(&Text[i], "LARRY THE DINOSAUR 3"); i += 80;
	strcpy(&Text[i], "HE MADE IT OUT ALIVE"); i += 80;
	strcpy(&Text[i], "HE THOUGHT IT WAS OVER"); i += 80;
	strcpy(&Text[i], "HE MADE A MISTAKE"); i += 80;
	strcpy(&Text[i], "THE PORTAL WAS LEFT ALONE"); i += 80;
	strcpy(&Text[i], "DESTINY CALLS HIM AGAIN");
	
	FadeScreenToBlack();
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
	FMUSIC_PlaySong(song1); Wait(1);

	state = false;
	for(n = 0; n <= i; n += 80){
		LD3.WriteText(&WinApp, 0, 80, &Text[n], true, 15, true); LD3.FlipSurfaces(&WinApp);		
		state = FadeScreenOutOfBlack();	if(state) break;
		state = Wait(3);				if(state) break;
		state = FadeScreenToBlack();	if(state) break;
		if(n == 160) {state = Wait(2); if(state) break;}
		LD3.ClearBuffer(&WinApp, 0);	
	}

	FadeScreenToBlack();
	LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp); LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
	Wait(2);
	FMUSIC_StopSong(song1);
}

ENTITY *CreateNewMAC()
{
	//- Create a new mini assault chopper and return the pointer to it
	ENTITY *m;
	NumMonsters++;
	
	m = &Monster[NumMonsters];
	m->Init();
	m->id			= CHOPPER;
	m->startx		= Player->x;
	m->starty		= Player->y;
	SetDefaultValue(m);

	MACammo = 50;
	MACfuel = 50.99f;

	return(m);
}

bool FadeScreenToBlack()
{
	int red, grn, blu;
	bool PaletteChange = true;
	bool state = false;
	bool onGamepad = true;

	SDL_Color blackpal[256];
	memset(blackpal, 0, sizeof(SDL_Color)*256);

	while(PaletteChange == true){
		PaletteChange = false;
		LD3.FlipSurfaces(&WinApp);
		for(int n = 0; n <= 255; n++){
			red = (int)FadePalette[n].r;
			grn = (int)FadePalette[n].g;
			blu = (int)FadePalette[n].b;
			if(red > 0) {red -= FADE_SPEED; PaletteChange = true;}
			if(grn > 0) {grn -= FADE_SPEED; PaletteChange = true;}
			if(blu > 0) {blu -= FADE_SPEED; PaletteChange = true;}
			if(red < 0) red = 0;
			if(grn < 0) grn = 0;
			if(blu < 0) blu = 0;
			FadePalette[n].r   = (BYTE)red;
			FadePalette[n].g = (BYTE)grn;
			FadePalette[n].b  = (BYTE)blu;
		}
		SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
		//------------------------------------------------
		//- Read the input from the keyboard and gamepad
		//------------------------------------------------
			Keyboard = SDL_GetKeyboardState(0);
#if 0
			if(WinApp.JoyStickEnabled){
				WinApp.lpdijoy[JoyOne]->Poll();
				WinApp.lpdijoy[JoyOne]->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&JoyState);
			}
#endif
		//------------------------------------------------
		//- End reading input
		//------------------------------------------------
			if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_ESCAPE] || Keyboard[keyJump] || Keyboard[keyShoot]) {state = true; break;}
			if(JoyState.rgbButtons[0] || JoyState.rgbButtons[1] || JoyState.rgbButtons[2] || JoyState.rgbButtons[3]){
				if(onGamepad == false){
					state = true;
					break;
				}
			}
			else{
				onGamepad = false;
			}
	}
	if(state){
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
		SDL_SetPaletteColors(WinApp.lpddpal, blackpal, 0, 256);
	}
	return(state);
}

bool FadeScreenOutOfBlack()
{
	int red, grn, blu;
	bool PaletteChange = true;
	bool state = false;
	
	while(PaletteChange == true){
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			red = (int)FadePalette[n].r;
			grn = (int)FadePalette[n].g;
			blu = (int)FadePalette[n].b;
			if(red < WinApp.palette[n].r)   {red += FADE_SPEED; PaletteChange = true;}
			if(grn < WinApp.palette[n].g) {grn += FADE_SPEED; PaletteChange = true;}
			if(blu < WinApp.palette[n].b)  {blu += FADE_SPEED; PaletteChange = true;}
			if(red > WinApp.palette[n].r)   red = WinApp.palette[n].r;
			if(grn > WinApp.palette[n].g) grn = WinApp.palette[n].g;
			if(blu > WinApp.palette[n].b)  blu = WinApp.palette[n].b;
			FadePalette[n].r   = (BYTE)red;
			FadePalette[n].g = (BYTE)grn;
			FadePalette[n].b  = (BYTE)blu;
		}
		SDL_SetPaletteColors(WinApp.lpddpal, FadePalette, 0, 256);
		LD3.FlipSurfaces(&WinApp);
		//------------------------------------------------
		//- Read the input from the keyboard and gamepad
		//------------------------------------------------
			Keyboard = SDL_GetKeyboardState(0);
			// TODO(davidgow): This (and others) won't matter if we don't poll the events! Also joystick!
		//------------------------------------------------
		//- End reading input
		//------------------------------------------------
			if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_ESCAPE] || Keyboard[keyJump] || Keyboard[keyShoot]) {state = true; break;}
			if(JoyState.rgbButtons[0] || JoyState.rgbButtons[1] || JoyState.rgbButtons[2] || JoyState.rgbButtons[3]) {state = true; break;}
	}
	if(state){
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
		SDL_SetPaletteColors(WinApp.lpddpal, WinApp.palette, 0, 256);
	}
	return(state);
}

bool Wait(int seconds)
{
	bool state = false;
	
	// TODO(davidgow): Make this VSync & Refresh rate independent.
	for(int n = 0; n <= seconds*75; n++){
		LD3.FlipSurfaces(&WinApp);
		//------------------------------------------------
		//- Read the input from the keyboard and gamepad
		//------------------------------------------------
			Keyboard = SDL_GetKeyboardState(0);
		//------------------------------------------------
		//- End reading input
		//------------------------------------------------
			if(Keyboard[SDL_SCANCODE_SPACE] || Keyboard[SDL_SCANCODE_ESCAPE] || Keyboard[keyJump] || Keyboard[keyShoot]) {state = true; break;}
			if(JoyState.rgbButtons[0] || JoyState.rgbButtons[1] || JoyState.rgbButtons[2] || JoyState.rgbButtons[3]) {state = true; break;}
	}
	if(state){
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);
		LD3.ClearBuffer(&WinApp, 0); LD3.FlipSurfaces(&WinApp);	
	}
	return(state);
}
