//- Larry The Dinosaur III WinMain Source File
//- November, 2002 - Created by Joe King
//======================================================

#include "main.h"

void LD3Main(HINSTANCE hinstance);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	MSG		msg;
	//
	//while(WinApp.IsRunning)
	//{
	//	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	//	{
	//		if(msg.message == WM_QUIT)
	//			break;
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	LD3Main();
	//}
	
	LD3Main(hinstance);

	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	return(msg.wParam);
}