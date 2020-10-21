//- Larry The Dinosaur 3 Engine Header File
//- November, 2002 - Created by Joe King
//===========================================

#ifndef LD3_H
#define LD3_H

//#include "main.h"

class LD3ENGINE
{
public:
							LD3ENGINE();
							~LD3ENGINE();

			void			LoadPalette(WINAPP *WinApp, const char* Filename);
			void			LoadTileSet(WINAPP *WinApp, const char* Filename, int start);
			void			LoadFont(const char* Filename);
			void			LoadMap(WINAPP *WinApp);
			void			LoadMap(const char* Filename);
			void			LoadSprite(WINAPP *WinApp, const char* Filename, int Num, int start);
			void			DrawMap(WINAPP *WinApp, int xShift, int yShift, int layer, float diff);
			void			FlipSurfaces(WINAPP *WinApp);
			void			PutSprite(WINAPP *WinApp, int x, int y, int Num, int Tile, bool xFlip);			
			void			LoadSky(WINAPP *WinApp, const char* Filename, int pos, int Convert);
			void			DrawSky(WINAPP *WinApp, int xShift, int yShift);
			void			SetSkyScroll(int yStart, int yEnd, int ShiftNum);
			void			WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, bool trans);
			void			WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, DWORD fcol, bool trans);
			void			DrawTalkBox(WINAPP *WinApp, int sy);
			void			PutPixel(WINAPP *WinApp, int x, int y, int col);
			void			DrawBox(WINAPP *WinApp, int x1, int y1, int x2, int y2, int col, bool fill);
			void			TakeScreenshot(WINAPP *WinApp);
			void			DrawLetterBox(WINAPP *WinApp);
			void			LoadMapIcon(WINAPP *WinApp, const char *Filename, int x, int y);
			void			LoadSplashScreen(WINAPP *WinApp, const char *Filename, int delay);
			void			ClearBuffer(WINAPP *WinApp, int col);
			void			FadeIn(WINAPP *WinApp);
			void			FadeOut(WINAPP *WinApp);
			void			WaitForRetrace(WINAPP *WinApp);
									

			int				MaxTiles;
			int				MapWidth, MapHeight;
			int				xRes, yRes;

			//UCHAR			sTiles[400][256];
			int				*Map;		
			//UCHAR			Sprite[400][80][5];
			UCHAR			Sky[512][256];
			int				SkyScroll[256];
			int				SkyVerticalScrollShift;
			UCHAR			Font[6][5][128];
			int				ScreenshotNum;
			int				AutoXShift;
			float			AniSpeed;

			bool			SkyScrollVertical;
			bool			SkyMirrorX, SkyMirrorY;

			bool WindowResized;
			RECT ClipArea;
private:
};

#endif
