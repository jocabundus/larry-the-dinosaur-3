//- Window Application Class soure file for Larry The Dinosaur III
//- November 22, 2002 - Created by Joe King
//================================================================

#include <iostream>
#include "SDL2/SDL.h"
#include "slx_win32types.h"
#include "winapp.h"

WINAPP::WINAPP()
{
	//- When the class initialized
	IsRunning = true;
	DirectDrawWasInitialized	= false;
	SurfacesWereCreated			= false;
	DirectInputWasInitialized	= false;
	KeyboardWasInitialized		= false;
	PaletteWasCreated			= false;
	MouseWasInitialized			= false;
	JoyWasInitialized			= false;
	WindowMode					= false;
	NumJoys						= 0;

	//strcpy((CHAR*)joyname, "No Other Input Devices Detected");
}

WINAPP::~WINAPP()
{
	//- When the class is destroyed
	KillDirectDraw();
}

void WINAPP::InitWindow(int xr, int yr, DWORD style, void *WindowProc)
{
	//- Initialize a window
	//---------------------
	// xr  = xRes
	// yr  = yRes	
	//--------------------------------------

	xRes = xr;
	yRes = yr;
	int wndScale = 3;
	
	
	if(!(hwnd = SDL_CreateWindow("Larry The Dinosaur III", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, xRes * wndScale, yRes * wndScale, 0)))
	{
		//- Error! The window was not created
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error (InitWindow())", "CreateWindow() failed!", NULL);
		IsRunning = false;
	}

}

void WINAPP::InitDirectDraw(int xr, int yr, int bts)
{
	//- Initialize DirectDraw7
	//------------------------
	// xr  = xRes
	// yr  = yRes
	// bts = bits(color depth of the screen)
	//--------------------------------------
	
	xRes = xr;
	yRes = yr;
	bits = bts;

	lpRenderer = SDL_CreateRenderer(hwnd, -1, SDL_RENDERER_PRESENTVSYNC);

	if(!lpRenderer)
	{
		//- Error! DirectDrawCreate failed!
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error (InitDirectDraw())", "SDL_CreateRenderer() failed!", NULL);
		IsRunning = false;
	}

	SDL_RenderSetLogicalSize(lpRenderer, xr, yr);
	
	DirectDrawWasInitialized = true;
}

void WINAPP::InitJoy()
{
	//- Initialize the joystick/gamepad
	//---------------------------------
	bool error = false;

	printf("STUB: InitJoy()\n");

}

void WINAPP::KillDirectDraw()
{
	//- Shutdown DirectX
	//----------------------

	if(PaletteWasCreated) {SDL_FreePalette(lpddpal);}

	if(SurfacesWereCreated)
	{
		SDL_FreeSurface(lpddsSprites[4]);		lpddsSprites[4] = NULL;
		SDL_FreeSurface(lpddsSprites[3]);		lpddsSprites[3] = NULL;
		SDL_FreeSurface(lpddsSprites[2]);		lpddsSprites[2] = NULL;
		SDL_FreeSurface(lpddsSprites[1]);		lpddsSprites[1] = NULL;
		SDL_FreeSurface(lpddsSprites[0]);		lpddsSprites[0] = NULL;		
		SDL_FreeSurface(lpddsTiles[4]);		lpddsTiles[4] = NULL;
		SDL_FreeSurface(lpddsTiles[3]);		lpddsTiles[3] = NULL;
		SDL_FreeSurface(lpddsTiles[2]);		lpddsTiles[2] = NULL;
		SDL_FreeSurface(lpddsTiles[1]);		lpddsTiles[1] = NULL;
		SDL_FreeSurface(lpddsTiles[0]);		lpddsTiles[0] = NULL;
		SDL_FreeSurface(lpddsback);			lpddsback = NULL;
		SDL_DestroyTexture(lpddsprimary);		lpddsprimary = NULL;
	}
}

void WINAPP::CreateSurfaces()
{
	//- Create the primary and secondary surfaces
	//-------------------------------------------

	if(DirectDrawWasInitialized)
	{

		lpddsprimary = SDL_CreateTexture(lpRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, xRes, yRes);
		if(!lpddsprimary)
		{
			//- Error! CreateSurface failed!
			SDL_ShowSimpleMessageBox(0, "Error! (CreateSurfaces()) - lpddsprimary", "SDL_CreateTexture failure", hwnd);
			IsRunning = false;
		}
		
		lpddsback = SDL_CreateRGBSurface(0, xRes, yRes, 8, 0, 0, 0, 0);
		// TODO(davidgow): What was colkey2 for?
		
		
		//- Create the tiles surface
		for(int i = 0; i <= 4; i++){
			lpddsTiles[i] = SDL_CreateRGBSurface(0, 320, 240, 8, 0, 0, 0, 0);
			SDL_SetColorKey(lpddsTiles[i], SDL_TRUE, 0);
			SDL_LockSurface(lpddsTiles[i]);
				UCHAR *TileBuffer = (UCHAR *)lpddsTiles[i]->pixels;
				for(int y = 0; y < 240; y++){
					for(int x = 0; x < 320; x++){
						TileBuffer[x+y*lpddsTiles[i]->pitch] = 0;
					}
				}
			SDL_UnlockSurface(lpddsTiles[i]);
		}
		//- Create the sprites surface
		for(int i = 0; i <= 4; i++){
			lpddsSprites[i] = SDL_CreateRGBSurface(0, 320, 240, 8, 0, 0, 0, 0);
			SDL_SetColorKey(lpddsSprites[i], SDL_TRUE, 0);
		}
		
		
		SurfacesWereCreated = true;
	}
	else
	{
			//- Error! DirectDraw wasn't initialized first!
			//MessageBox(hwnd, "DirectDraw wasn't initialized before calling CreateSurfaces()", "Error! (CreateSurfaces())", MB_OK);
			IsRunning = false;
	}	
}

void WINAPP::InitDirectInput()
{
	//- Initialize DirectInput8
	//-------------------------

	DirectInputWasInitialized = true;
}

void WINAPP::InitKeyboard()
{
	//- Initialize Keyboard
	//---------------------
}

void WINAPP::CreatePalette()
{
	//- Create the palette(for 8bit modes)
	//------------------------------------
	lpddpal = SDL_AllocPalette(256);
	SDL_SetPaletteColors(lpddpal, palette, 0, 256);
	PaletteWasCreated = true;
}

void WINAPP::SetPalette()
{
	//- Set the palette
	//-----------------
	if(PaletteWasCreated)
	{		
		SDL_SetSurfacePalette(lpddsback, lpddpal);
	}
	else
	{
		//- Error! The palette wasn't created first!
		SDL_ShowSimpleMessageBox(0, "Error! (SetPalette())", "SetPalette() was called before CreatePalette()", hwnd);
		IsRunning = false;
	}
}

void WINAPP::InitMouse()
{
}

