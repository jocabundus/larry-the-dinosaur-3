//- Window Application Class for Larry The Dinosaur III
//- November, 2002 - Created by Joe King
//=====================================================

#ifndef WINAPP_H
#define WINAPP_H

//#include "main.h"

class WINAPP
{
public:

										WINAPP();
										~WINAPP();

				void					InitWindow(int xr, int yr, DWORD style, WNDPROC WindowProc);
				void					InitDirectDraw(int xr, int yr, int bts);
				void					KillDirectDraw();
				void					CreateSurfaces();
				void					InitDirectInput();
				void					InitKeyboard();
				void					CreatePalette();
				void					SetPalette();
				void					InitMouse();
				void					InitJoy();
				

				HWND					hwnd;
				bool					IsRunning;

				LPDIRECTDRAW7			lpdd;
				DDSURFACEDESC2			ddsd;
				LPDIRECTDRAWSURFACE7	lpddsprimary, lpddsback;
				LPDIRECTDRAWSURFACE7	lpddsTiles[5], lpddsSprites[5];				
				DDSCAPS2				ddscaps;
				DDBLTFX					ddbltfx;
				DDCOLORKEY				colkey, colkey2;
				LPDIRECTDRAWCLIPPER		lpddclipper;
				RGNDATA					regiondata;
								
				LPDIRECTINPUT8			lpdi;
				LPDIRECTINPUTDEVICE8	lpdikey;
				LPDIRECTINPUTDEVICE8	lpdimouse;
				LPDIRECTINPUTDEVICE8 	lpdijoy[10];
				DIPROPRANGE				joyaxisrange[10];
				UCHAR					joyname[1000];
				GUID					joystickGUID[10];
				bool					JoyStickEnabled;
				
				
				LPDIRECTDRAWPALETTE		lpddpal;
				PALETTEENTRY			palette[256];
				HINSTANCE				hinstance;		

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
				
				static BOOL CALLBACK	DI_Enum_Joysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr);
				static WINAPP			*LastInstance;
};


#endif