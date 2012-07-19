//- Window Application Class soure file for Larry The Dinosaur III
//- November 22, 2002 - Created by Joe King
//================================================================

//#include "winapp.h"
#include "main.h"

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

	strcpy((CHAR*)joyname, "No Other Input Devices Detected");
}

WINAPP::~WINAPP()
{
	//- When the class is destroyed
	KillDirectDraw();
}

void WINAPP::InitWindow(int xr, int yr, DWORD style, WNDPROC WindowProc)
{
	//- Initialize a window
	//---------------------
	// xr  = xRes
	// yr  = yRes	
	//--------------------------------------

	xRes = xr;
	yRes = yr;
	
	WNDCLASS	WinClass;

	WinClass.cbClsExtra				= 0;
	WinClass.cbWndExtra				= 0;
	WinClass.hbrBackground			= (HBRUSH)GetStockObject(BLACK_BRUSH);
	WinClass.hCursor				= LoadCursor(NULL, IDC_ARROW);
	WinClass.hIcon					= LoadIcon(NULL, IDI_APPLICATION);
	WinClass.hInstance				= hinstance;
	WinClass.lpfnWndProc			= WindowProc;
	WinClass.lpszClassName			= "LD3WINCLASS";
	WinClass.lpszMenuName			= NULL;
	WinClass.style					= CS_HREDRAW | CS_VREDRAW;

	if(!RegisterClass(&WinClass))
	{
		//- Error! The WinClass did not register
		MessageBox(NULL, "RegisterClass() failed!", "Error (InitWindow())", MB_OK);
		IsRunning = false;
	}
	
	if(!(hwnd =
		CreateWindow(	"LD3WINCLASS",
						"Larry The Dinosaur III",
						style | WS_VISIBLE,
						0, 0,
						xRes, yRes,
						NULL,
						NULL,
						hinstance,
						NULL)))
	{
		//- Error! The window was not created
		MessageBox(NULL, "CreateWindow() failed!", "Error (InitWindow())", MB_OK);
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

	if(DirectDrawCreateEx(NULL, (void**)&lpdd, IID_IDirectDraw7, NULL) != DD_OK)
	{
		//- Error! DirectDrawCreate failed!
		MessageBox(hwnd, "DirectDrawCreate() failed!", "Error! (InitDirectDraw())", MB_OK);
		IsRunning = false;
	}

	if(WindowMode){
		if(lpdd->SetCooperativeLevel(hwnd, DDSCL_NORMAL) != DD_OK)
		{
			//- Error! SetCooperativeLevel failed!
			MessageBox(hwnd, "SetCooperativeLevel() failed!", "Error! (InitDirectDraw())", MB_OK);
			IsRunning = false;
			ShowCursor(true);
		}
	}
	else{
		if(lpdd->SetCooperativeLevel(	hwnd, DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | 
										DDSCL_ALLOWREBOOT) != DD_OK)
		{
			//- Error! SetCooperativeLevel failed!
			MessageBox(hwnd, "SetCooperativeLevel() failed!", "Error! (InitDirectDraw())", MB_OK);
			IsRunning = false;
		}

		if(lpdd->SetDisplayMode(xRes, yRes, bits, 0, 0) != DD_OK)
		{
			//- Error! SetDisplayMode failed!
			MessageBox(hwnd, "SetDisplayMode() failed!", "Error! (InitDirectDraw())", MB_OK);
			IsRunning = false;
		}
		ShowCursor(false);
	}
	
	regiondata.rdh.dwSize			= sizeof(RGNDATAHEADER);
	regiondata.rdh.iType			= RDH_RECTANGLES;
	regiondata.rdh.nCount			= 1;
	regiondata.rdh.nRgnSize			= sizeof(RECT);
	regiondata.rdh.rcBound.top		= 0;
	regiondata.rdh.rcBound.bottom	= yRes-1;
	regiondata.rdh.rcBound.left		= 0;
	regiondata.rdh.rcBound.right	= xRes-1;

	RECT cliprect;

	cliprect.top	= 0;
	cliprect.bottom	= yRes;
	cliprect.left	= 0;
	cliprect.right	= xRes;
	memcpy(regiondata.Buffer, &cliprect, sizeof(RECT));
	
	DirectDrawWasInitialized = true;
}

WINAPP * WINAPP::LastInstance = NULL;

BOOL CALLBACK WINAPP::DI_Enum_Joysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID guid_ptr)
{
	//*(GUID*)guid_ptr = lpddi->guidInstance;
	LastInstance->joystickGUID[LastInstance->NumJoys] = lpddi->guidInstance;
	if(LastInstance != NULL){
		strcpy((char *)&LastInstance->joyname[LastInstance->NumJoys*80], (char *)lpddi->tszProductName);
		LastInstance->NumJoys++;
	}
	return(DIENUM_CONTINUE);
}

void WINAPP::InitJoy()
{
	//- Initialize the joystick/gamepad
	//---------------------------------
	bool error = false;

	if(DirectInputWasInitialized)
	{
		LastInstance = this;
		if(lpdi->EnumDevices(DI8DEVCLASS_GAMECTRL, DI_Enum_Joysticks, &joystickGUID[NumJoys], DIEDFL_ATTACHEDONLY) != DI_OK)
		{
			//- Error! EnumDevices failed!!
			MessageBox(hwnd, "EnumDevices() failed!", "Error! (InitJoy())", MB_OK);
			error = true;
			//IsRunning = false;				
		}
		if(NumJoys > 0) JoyStickEnabled = true;		 
		for(int nj = 0; nj < NumJoys; nj++){
			if(lpdi->CreateDevice(joystickGUID[nj], &lpdijoy[nj], NULL) != DI_OK)
			{
				//- Error! CreateDevice failed!
				
				//IsRunning = false;
				//if(NumJoys == 0){
				JoyStickEnabled = false;
				MessageBox(hwnd, "CreateDevice() failed!", "Error! (InitJoy())", MB_OK);
				//}
				error = true;
				//JoyStickEnabled = false;
			}
		}
		if(JoyStickEnabled && error == false){			
			for(int nj = 0; nj < NumJoys; nj++){
				if(lpdijoy[nj]->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
				{
					//- Error! SetCooperativeLevel failed!
					MessageBox(hwnd, "SetCooperativeLevel() failed!", "Error! (InitJoy())", MB_OK);
					//IsRunning = false;					
				}
				if(lpdijoy[nj]->SetDataFormat(&c_dfDIJoystick) != DI_OK)
				{
					//- Error! SetDataFormat failed!
					MessageBox(hwnd, "SetDataFormat() failed!", "Error! (InitJoy())", MB_OK);
					//IsRunning = false;					
				}
				joyaxisrange[nj].lMin = -10;
				joyaxisrange[nj].lMax = 10;
				joyaxisrange[nj].diph.dwSize = sizeof(DIPROPRANGE);
				joyaxisrange[nj].diph.dwHeaderSize = sizeof(DIPROPHEADER);
				joyaxisrange[nj].diph.dwObj = DIJOFS_X;
				joyaxisrange[nj].diph.dwHow = DIPH_BYOFFSET;
				lpdijoy[nj]->SetProperty(DIPROP_RANGE, &joyaxisrange[nj].diph);
				joyaxisrange[nj].lMin = -10;
				joyaxisrange[nj].lMax = 10;
				joyaxisrange[nj].diph.dwSize = sizeof(DIPROPRANGE);
				joyaxisrange[nj].diph.dwHeaderSize = sizeof(DIPROPHEADER);
				joyaxisrange[nj].diph.dwObj = DIJOFS_Y;
				joyaxisrange[nj].diph.dwHow = DIPH_BYOFFSET;
				lpdijoy[nj]->SetProperty(DIPROP_RANGE, &joyaxisrange[nj].diph);
				if(lpdijoy[nj]->Acquire() != DI_OK)
				{
					//- Error! Acquire failed!
					MessageBox(hwnd, "Acquire() failed!", "Error! (InitJoy())", MB_OK);
					//IsRunning = false;					
				}
				JoyWasInitialized = true;
			}
		}
	}
	else
	{
		//- Error! DirectInput wasn't initialized first!
		MessageBox(hwnd, "DirectInput wasn't initialized before calling InitJoy()", "Error! (InitJoy())", MB_OK);
		IsRunning = false;
	}	
}

void WINAPP::KillDirectDraw()
{
	//- Shutdown DirectX
	//----------------------

	if(JoyWasInitialized)
	{
		for(int j = NumJoys-1; j >= 0; j--){
			lpdijoy[j]->Unacquire();
			lpdijoy[j]->Release();
			lpdijoy[j] = NULL;	
		}
	}
	if(MouseWasInitialized)
	{
		lpdimouse->Unacquire();		
		lpdimouse->Release();
		lpdimouse = NULL;
	}
	if(KeyboardWasInitialized)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
		lpdikey = NULL;
	}
	if(DirectInputWasInitialized) {lpdi->Release(); lpdi = NULL;}
	
	if(PaletteWasCreated) {lpddsprimary->SetPalette(NULL); lpddpal->Release(); lpddpal = NULL;}

	if(SurfacesWereCreated)
	{
		lpddsSprites[4]->Release();		lpddsSprites[4] = NULL;
		lpddsSprites[3]->Release();		lpddsSprites[3] = NULL;
		lpddsSprites[2]->Release();		lpddsSprites[2] = NULL;
		lpddsSprites[1]->Release();		lpddsSprites[1] = NULL;
		lpddsSprites[0]->Release();		lpddsSprites[0] = NULL;		
		lpddsTiles[4]->Release();		lpddsTiles[4] = NULL;
		lpddsTiles[3]->Release();		lpddsTiles[3] = NULL;
		lpddsTiles[2]->Release();		lpddsTiles[2] = NULL;
		lpddsTiles[1]->Release();		lpddsTiles[1] = NULL;
		lpddsTiles[0]->Release();		lpddsTiles[0] = NULL;
		lpddsback->Release();			lpddsback = NULL;
		lpddsprimary->Release();		lpddsprimary = NULL;
		lpddclipper->Release();
	}
	if(DirectDrawWasInitialized)	{lpdd->Release(); lpdd = NULL;}
}

void WINAPP::CreateSurfaces()
{
	//- Create the primary and secondary surfaces
	//-------------------------------------------

	if(DirectDrawWasInitialized)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));		
		ddsd.dwSize	= sizeof(ddsd);
		
		if(WindowMode == false){
			ddsd.dwFlags			= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps		= DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
			ddsd.dwBackBufferCount	= 1;
		}
		else{
			ddsd.dwFlags			= DDSD_CAPS;
			ddsd.ddsCaps.dwCaps		= DDSCAPS_PRIMARYSURFACE;
		}

		if(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL) != DD_OK)
		{
			//- Error! CreateSurface failed!
			MessageBox(hwnd, "CreateSurface() failed!", "Error! (CreateSurfaces()) - lpddsprimary", MB_OK);
			IsRunning = false;
		}
		
		if(WindowMode == false){
			ZeroMemory(&ddscaps, sizeof(ddscaps));
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;		
			if(lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback) != DD_OK)
			{
				//- Error! GetAttachedSurface failed!
				MessageBox(hwnd, "GetAttachedSurface() failed!", "Error! (CreateSurfaces())", MB_OK);
				IsRunning = false;
			}
		}
		else{			
			colkey2.dwColorSpaceLowValue	= 255;
			colkey2.dwColorSpaceHighValue	= 255;		
			ZeroMemory(&ddscaps, sizeof(ddscaps));
			ddsd.dwSize			= sizeof(ddsd);
			ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			ddsd.dwWidth		= xRes;
			ddsd.dwHeight		= yRes;
			ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN;
			if(lpdd->CreateSurface(&ddsd, &lpddsback, NULL) != DD_OK){
				//- Error! CreateSurface failed!
				MessageBox(hwnd, "CreateSurface() failed!", "Error! (CreateSurfaces()) - lpddsback", MB_OK);
				IsRunning = false;
			}
			if(lpddsback->SetColorKey(DDCKEY_SRCBLT, &colkey2) != DD_OK){
				//- Error! SetColorKey failed!
				MessageBox(hwnd, "SetColorKey() failed!", "Error! (CreateSurfaces()) - lpddsback", MB_OK);
				IsRunning = false;
			}
		}
		
		//- Set the color key
		colkey.dwColorSpaceLowValue		= 0;
		colkey.dwColorSpaceHighValue	= 0;		
		
		//- Create the tiles surface
		for(int i = 0; i <= 4; i++){
			ZeroMemory(&ddscaps, sizeof(ddscaps));
			ddsd.dwSize			= sizeof(ddsd);
			ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			ddsd.dwWidth		= 320;
			ddsd.dwHeight		= 240;
			ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN;
			if(lpdd->CreateSurface(&ddsd, &lpddsTiles[i], NULL) != DD_OK){
				//- Error! CreateSurface failed!
				MessageBox(hwnd, "CreateSurface() failed!", "Error! (CreateSurfaces()) - lpddsTiles", MB_OK);
				IsRunning = false;
			}
			if(lpddsTiles[i]->SetColorKey(DDCKEY_SRCBLT, &colkey) != DD_OK){
				//- Error! SetColorKey failed!
				MessageBox(hwnd, "SetColorKey() failed!", "Error! (CreateSurfaces()) - lpddsTiles", MB_OK);
				IsRunning = false;
			}
			lpddsTiles[i]->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
				UCHAR *TileBuffer = (UCHAR *)ddsd.lpSurface;
				for(int y = 0; y <= 240; y++){
					for(int x = 0; x <= 320; x++){
						TileBuffer[x+y*ddsd.lPitch] = 0;
					}
				}
			lpddsTiles[i]->Unlock(NULL);
		}
		//- Create the sprites surface
		for(i = 0; i <= 4; i++){
			ZeroMemory(&ddscaps, sizeof(ddscaps));
			ddsd.dwSize			= sizeof(ddsd);
			ddsd.dwFlags		= DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
			ddsd.dwWidth		= 320;
			ddsd.dwHeight		= 240;
			ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN;
			if(lpdd->CreateSurface(&ddsd, &lpddsSprites[i], NULL) != DD_OK){
				//- Error! CreateSurface failed!
				MessageBox(hwnd, "CreateSurface() failed!", "Error! (CreateSurfaces()) - lpddsSprites", MB_OK);
				IsRunning = false;
			}
			if(lpddsSprites[i]->SetColorKey(DDCKEY_SRCBLT, &colkey) != DD_OK){
				//- Error! SetColorKey failed!
				MessageBox(hwnd, "SetColorKey() failed!", "Error! (CreateSurfaces()) - lpddsSprites", MB_OK);
				IsRunning = false;
			}
		}
		
		
		//- Set the direct draw clipper
		if(lpdd->CreateClipper(0, &lpddclipper, NULL) != DD_OK)
		{
			//- Error! CreateClipper failed!
			MessageBox(hwnd, "CreateClipper() failed!", "Error! (CreateSurfaces())", MB_OK);
			IsRunning = false;
		}

		if(lpddclipper->SetClipList(&regiondata, 0) != DD_OK)
		{
			//- Error! CreateClipper failed!
			MessageBox(hwnd, "SetClipList() failed!", "Error! (CreateSurfaces())", MB_OK);
			IsRunning = false;
		}

		if(lpddsback->SetClipper(lpddclipper) != DD_OK)
		{
			//- Error! CreateClipper failed!
			MessageBox(hwnd, "SetClipper() failed!", "Error! (CreateSurfaces())", MB_OK);
			IsRunning = false;
		}

		//- Set the DDBLTFX structure
		memset(&ddbltfx, 0, sizeof(DDBLTFX));
		ddbltfx.dwSize = sizeof(DDBLTFX);		
		ddbltfx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;
		//ddbltfx.
		
		SurfacesWereCreated = true;
	}
	else
	{
			//- Error! DirectDraw wasn't initialized first!
			MessageBox(hwnd, "DirectDraw wasn't initialized before calling CreateSurfaces()", "Error! (CreateSurfaces())", MB_OK);
			IsRunning = false;
	}	
}

void WINAPP::InitDirectInput()
{
	//- Initialize DirectInput8
	//-------------------------

	if(	DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void **)&lpdi, NULL) != DI_OK)
	{
		//- Error! DirectInput8Create failed!
		MessageBox(hwnd, "DirectInput8Create() failed!", "Error! (InitDirectInput())", MB_OK);
		IsRunning = false;
	}

	DirectInputWasInitialized = true;
}

void WINAPP::InitKeyboard()
{
	//- Initialize Keyboard
	//---------------------

	if(DirectInputWasInitialized)
	{
		if(lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL) != DI_OK)
		{
			//- Error! CreateDevice failed!
			MessageBox(hwnd, "CreateDevice() failed!", "Error! (InitKeyboard())", MB_OK);
			IsRunning = false;
		}

		if(lpdikey->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			//- Error! SetCooperativeLevel failed!
			MessageBox(hwnd, "SetCooperativeLevel() failed!", "Error! (InitKeyboard())", MB_OK);
			IsRunning = false;
		}

		if(lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		{
			//- Error! SetDataFormat failed!
			MessageBox(hwnd, "SetDataFormat() failed!", "Error! (InitKeyboard())", MB_OK);
			IsRunning = false;
		}

		if(lpdikey->Acquire() != DI_OK)
		{
			//- Error! Acquire failed!
			MessageBox(hwnd, "Acquire() failed!", "Error! (InitKeyboard())", MB_OK);
			IsRunning = false;
		}

		KeyboardWasInitialized = true;
	}
	else
	{
		//- Error! DirectInput wasn't initialized first!
		MessageBox(hwnd, "DirectInput wasn't initialized before calling InitKeyboard()", "Error! (InitKeyboard())", MB_OK);
		IsRunning = false;
	}
}

void WINAPP::CreatePalette()
{
	//- Create the palette(for 8bit modes)
	//------------------------------------
	ZeroMemory(&lpddpal, sizeof(lpddpal));	
	if(lpdd->CreatePalette(	DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette,
							&lpddpal, NULL) != DD_OK)
	{
		//- Error! CreatePalette failed!
		MessageBox(hwnd, "CreatePalette() failed!", "Error! (CreatePalette())", MB_OK);
		IsRunning = false;
	}
	PaletteWasCreated = true;
}

void WINAPP::SetPalette()
{
	//- Set the palette
	//-----------------
	if(PaletteWasCreated)
	{		
		if(lpddsprimary->SetPalette(lpddpal) != DD_OK)			
		{
			//- Error! SetPalette failed!
			MessageBox(hwnd, "SetPalette() failed!", "Error! (SetPalette())", MB_OK);
			IsRunning = false;
		}
	}
	else
	{
		//- Error! The palette wasn't created first!
		MessageBox(hwnd, "SetPalette() was called before CreatePalette()", "Error! (SetPalette())", MB_OK);
		IsRunning = false;
	}
}

void WINAPP::InitMouse()
{
	//- Initialize the mouse
	//----------------------

	if(DirectInputWasInitialized)
	{
		if(lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
		{
			//- Error! CreateDevice failed!
			MessageBox(hwnd, "CreateDevice() failed!", "Error! (InitMouse())", MB_OK);
			IsRunning = false;
		}
		if(lpdimouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		{
			//- Error! SetCooperativeLevel failed!
			MessageBox(hwnd, "SetCooperativeLevel() failed!", "Error! (InitMouse())", MB_OK);
			IsRunning = false;
		}
		if(lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		{
			//- Error! SetDataFormat failed!
			MessageBox(hwnd, "SetDataFormat() failed!", "Error! (InitMouse())", MB_OK);
			IsRunning = false;
		}
		if(lpdimouse->Acquire() != DI_OK)
		{
			//- Error! Acquire failed!
			MessageBox(hwnd, "Acquire() failed!", "Error! (InitMouse())", MB_OK);
			IsRunning = false;
		}
		MouseWasInitialized = true;
	}
	else
	{
		//- Error! DirectInput wasn't initialized first!
		MessageBox(hwnd, "DirectInput wasn't initialized before calling InitMouse()", "Error! (InitMouse())", MB_OK);
		IsRunning = false;
	}
}

