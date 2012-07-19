//- Window Procedrue Source File for Larry The Dinosaur III
//- November 22, 2002 - Created by Joe King
//======================================================================

#include "main.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	PAINTSTRUCT ps;
	HDC			hdc;
	//float		active;
	
	switch(msg)
	{
	case WM_CREATE:
		{
			//- When the window is created
			return(0);			
		} break;
	case WM_DESTROY:
		{
			//- When the window is destroyed
			PostQuitMessage(0);
			return(0);
		} break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return(0);			
		} break;
	case WM_SIZE:
		{
			//GetWindowRect(hwnd, &LD3.ClipArea);
			//LD3.WindowResized = true;
			return(0);
		} break;
	case WM_ACTIVATE:
		{
			/*active = LOWORD(wparam);
			if(active == WA_INACTIVE)
				Active = false;
			else
				Active = true;

			active = (BOOL) HIWORD(wparam);
			if(active == 0)
				Active = true;
			else
				Active = false;*/
			//Active = false;
						
			return(0);
		} break;
	case WM_KEYDOWN:
		{
			return(0);			
		} break;

	}
	//Active = false;

	return(DefWindowProc(hwnd, msg, wparam, lparam));
}
