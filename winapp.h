//- Window Application Class for Larry The Dinosaur III
//- November, 2002 - Created by Joe King
//=====================================================

#ifndef WINAPP_H
#define WINAPP_H

#include "SDL2/SDL.h"
#include "slx_win32types.h"

class WINAPP
{
public:

										WINAPP();
										~WINAPP();

				void					InitWindow(int xr, int yr, DWORD style, void *WindowProc);
				void					InitDirectDraw(int xr, int yr, int bts);
				void					KillDirectDraw();
				void					CreateSurfaces();
				void					InitDirectInput();
				void					InitKeyboard();
				void					CreatePalette();
				void					SetPalette();
				void					InitMouse();
				void					InitJoy();
				

				SDL_Window				*hwnd;
				SDL_Renderer				*lpRenderer;
				bool					IsRunning;

				SDL_Texture				*lpddsprimary;
			       	SDL_Surface				*lpddsback;
				SDL_Surface				*lpddsTiles[5], *lpddsSprites[5];				
				//DDCOLORKEY				colkey, colkey2;
								
				bool					JoyStickEnabled;
				
				
				SDL_Palette				*lpddpal;
				SDL_Color				palette[256];

				int						xRes, yRes, bits;
				int						NumJoys;
				bool					WindowMode;

private:

				bool					DirectDrawWasInitialized;
				bool					DirectInputWasInitialized;
				bool					SurfacesWereCreated;				
				bool					KeyboardWasInitialized;
				bool					PaletteWasCreated;
				bool					MouseWasInitialized;
				bool					JoyWasInitialized;
				
};


#endif
