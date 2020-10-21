//- Larry The Dinosaur 3 Engine Source File
//- November, 2002 - Created by Joe King
//===========================================

//#include "ld3.h"
//#include "main.h"
using namespace std;
#include <fstream>
#include "SDL2/SDL.h"
#include "defines.h"
#include "ld3.h"
#include "winapp.h"

LD3ENGINE::LD3ENGINE()
{
	//- Initialize
	//------------
	MaxTiles	  = 0;
	MapWidth	  = MAPWIDTH;
	MapHeight	  = MAPHEIGHT;
	ScreenshotNum = 0;
	AutoXShift    = 0;

	SkyScrollVertical		= false;	
	SkyVerticalScrollShift	= 2;
	SkyMirrorX				= false;
	SkyMirrorY				= false;

	AniSpeed	= 0.1f;
	
	Map = (int*)calloc(220000, sizeof(int));
	//ZeroMemory(&Map, sizeof(Map));
}

LD3ENGINE::~LD3ENGINE()
{
	free(Map);
}

void LD3ENGINE::LoadPalette(WINAPP *WinApp, const char* Filename)
{
	//- Load the palette
	//------------------

	ifstream PalFile(Filename, ios::binary);
	SDL_Color	*palette;
	char			ch;	
	
	palette = &WinApp->palette[0];
	
	for(int i = 0; i <= 255; i++)
	{
		PalFile.get(ch); palette[i].r	= ch*4;
		PalFile.get(ch); palette[i].g	= ch*4;
		PalFile.get(ch); palette[i].b	= ch*4;
		palette[i].a = 255;
		
	}
	
	PalFile.close();	
}

void LD3ENGINE::LoadTileSet(WINAPP *WinApp, const char* Filename, int start)
{
	//- Load the tile sprites
	//-----------------------

	SDL_Surface		*surf;
	UCHAR			*TileBuffer;
	long			lp;
	int		n = start;
	int		tl = start % 160;
	char	ch;

	ifstream SpriteFile(Filename, ios::binary);

	if (!SpriteFile.is_open())
	{
		SDL_ShowSimpleMessageBox(0, "Error", "Unable to open file.", WinApp->hwnd);
		return;
	}
	
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

	surf = WinApp->lpddsTiles[start/160];
	SDL_LockSurface(surf);
	TileBuffer = (UCHAR *)surf->pixels;


	while(!SpriteFile.eof())
	{
		SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

		for(int y = 0; y <= 19; y++)
		{
			for(int x = 0; x <= 19; x++)
			{
				SpriteFile.get(ch);
				//sTiles[x+y*20][n] = ch;
				//TileBuffer[n*20+x+y*ddsd->lPitch] = ch;
				//if(start >= 160)
				//	tl = n-160;
				//else
				//	tl = n;
				//tl = n % 160;
				TileBuffer[((tl&15)*20)+x+(y+((tl>>4)*20))*surf->pitch] = ch;
			}
		}
		//n++;
		tl++;
		if(tl >= 160){
			SDL_UnlockSurface(surf);
			start += 160;
			surf = WinApp->lpddsTiles[start/160];
			SDL_LockSurface(surf);
			TileBuffer = (UCHAR *)surf->pixels;
			tl = 0;
		}
	}
	SDL_UnlockSurface(surf);

	SpriteFile.close();
	//MaxTiles = n-2;
}

void LD3ENGINE::LoadMap(WINAPP *WinApp)
{
	//- Load the map
	//--------------
/*
	OPENFILENAME	ofn;
	char			ch;
	char			Filename[256];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ShowCursor(true);
	ofn.hwndOwner		= WinApp->hwnd;
	ofn.lpstrFilter		= "All Files (*.*)\0*.*\0";
	ofn.nFilterIndex	= 1;	
	ofn.lpstrFile		= Filename;
	ofn.nMaxFile		= 256;
	ofn.lStructSize		= sizeof(OPENFILENAME);
			
	//WinApp->lpdd->FlipToGDISurface();
	while(GetOpenFileName(&ofn) != IDOK);
	ShowCursor(false);

	ifstream MapFile(Filename, ios::binary);
	
	for(int my = 0; my <= MapHeight-1; my++)
	{
		for(int mx = 0; mx <= MapWidth-1; mx++)
		{
			MapFile.get(ch);
			Map[mx+my*MapWidth] = ch;			
		}
	}
		
	MapFile.close();*/
}

void LD3ENGINE::LoadMap(const char* Filename)
{
	//- Load the map with dialog box selection
	//----------------------------------------
	
	char			ch1, ch2;
	int				m;
	
	ifstream MapFile(Filename, ios::binary);
	
	for(int ml = 0; ml <= 3; ml++)
	{
		for(int my = 0; my <= MapHeight-1; my++)
		{
			for(int mx = 0; mx <= MapWidth-1; mx++)
			{
				if(ml == 3){
					Map[mx+my*MapWidth+165000] = 1;
				}
				else
				{
					//MapFile.get(ch);
					//Map[mx+my*MapWidth+(ml*55000)] = ch;
					MapFile.get(ch1);
					MapFile.get(ch2);
					m = ((UCHAR)(ch1))<<8;
					m += (UCHAR)ch2;
					Map[mx+my*MapWidth+(ml*55000)] = m;
				}

				/*if(mx > 499){
					Map[mx+my*MapWidth] = 0;
					Map[mx+my*MapWidth+55000] = 0;
					Map[mx+my*MapWidth+(2*55000)] = 1;
					Map[mx+my*MapWidth+165000] = 1;
				}*/
			}
		}
	}
		
	MapFile.close();
}

void LD3ENGINE::LoadSprite(WINAPP *WinApp, const char* Filename, int Num, int start)
{
	//- Load a character sprite file
	//------------------------------

	SDL_Surface *surf;
	UCHAR			*SpriteBuffer;
	char			ch;
	int				n = start;
	long			lp;

	ifstream SpriteFile(Filename, ios::binary);
	
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

	surf = WinApp->lpddsSprites[Num];
	SDL_LockSurface(surf);
		SpriteBuffer = (UCHAR *)surf->pixels;
	
		while(!SpriteFile.eof())
		{
			SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

			for(int y = 0; y <= 19; y++)
			{
				for(int x = 0; x <= 19; x++)
				{
					SpriteFile.get(ch);
					//Sprite[x+y*20][n][Num] = ch;
					//SpriteBuffer[Num*1600+n*20+x+y*ddsd->lPitch] = ch;
					SpriteBuffer[((n&15)*20)+x+(y+((n>>4)*20))*surf->pitch] = ch;					
				}
			}
			n++;
		}
	SDL_UnlockSurface(surf);
			
	SpriteFile.close();	
}

void LD3ENGINE::LoadFont(const char* Filename)
{
	//- Load a character sprite file
	//------------------------------

	char			ch;
	int				n = 0;

	ifstream SpriteFile(Filename, ios::binary);
	
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
		
	while(!SpriteFile.eof())
	{
		SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
		//SpriteFile.get(ch); SpriteFile.get(ch);

		for(int y = 0; y <= 4; y++)
		{
			for(int x = 0; x <= 5; x++)
			{
				SpriteFile.get(ch);
				Font[x][y][n] = ch;
			}
		}
		n++;
	}
			
	SpriteFile.close();
}

void LD3ENGINE::ClearBuffer(WINAPP *WinApp, int col)
{
	//- color the entire surface with the given color

	SDL_FillRect(WinApp->lpddsback, NULL, col);
}

void LD3ENGINE::DrawSky(WINAPP *WinApp, int xShift, int yShift)
{
	//- Draw the sky
	UCHAR			*VideoBuffer;
	long			lp;
	static int		CloudCount = 0;
	int				ss = 0;
	int				flip = 0;
	
	SDL_LockSurface(WinApp->lpddsback);
		VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;
	

		lp = 0;
		CloudCount += AutoXShift;
		if(CloudCount > 32767) CloudCount -= 32767;

		if(SkyScrollVertical){
			if(SkyMirrorX){
				for(int y = 0; y < yRes; y++){
					flip = abs(255-abs(255 - ((y+(yShift>>SkyVerticalScrollShift)) & 511)));
					ss = SkyScroll[flip];					
					for(int x = 0; x < xRes; x++){
						VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>ss)) & 511][flip];
					}
					lp += WinApp->lpddsback->pitch;
				}
			}
			else{
				for(int y = 0; y < yRes; y++){
					ss = SkyScroll[y+(yShift>>SkyVerticalScrollShift) & 255];
					for(int x = 0; x < xRes; x++){
						VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>ss)) & 511][y+(yShift>>SkyVerticalScrollShift) & 255];
					}
					lp += WinApp->lpddsback->pitch;
				}
			}
		}
		else{
			if(SkyMirrorY){
				for(int y = 0; y < yRes; y++){					
					ss = SkyScroll[y];
					for(int x = 0; x < xRes; x++){
						flip = abs(511-abs(511 - ((x+((xShift+CloudCount)>>ss)) & 1023)));
						VideoBuffer[lp+x] = Sky[flip][y];
					}
					lp += WinApp->lpddsback->pitch;
				}
			}
			else{
				for(int y = 0; y < yRes; y++){
					ss = SkyScroll[y];
					for(int x = 0; x < xRes; x++){
						VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>ss)) & 511][y];
					}
					lp += WinApp->lpddsback->pitch;
				}
			}
		}
		

		
	SDL_UnlockSurface(WinApp->lpddsback);
	
}

void LD3ENGINE::DrawMap(WINAPP *WinApp, int xShift, int yShift, int layer, float diff)
{
	//- Draw the map onto the screen
	//------------------------------

	UCHAR			*VideoBuffer;
	int				mx, my, ml;
	int				cx, cy;
	int				tile, num;

	int				cystart, cyend;
	int             cxstart, cxend;
	int				xadd, yadd;
	int				xmin, ymin;
	int				xs, ys;
	UCHAR			col;
	long			mlt;
	int				ani;
	int				mxmax, mymax;
	static float	Animation = 0;
	SDL_Rect			SrcArea, DestArea;
	int				top, left;
	
	xs = xShift / 20;
	ys = yShift / 20;
	mxmax = xRes/20;
	mymax = yRes/20;
	
	ml = layer;
	long myys;
	
		
		mlt = ml*55000; top = 0;
		for(my = 0; my <= mymax; my++)
		{
			left = 0;
			for(mx = 0; mx <= mxmax; mx++)
			{
				DestArea.y	= top-(yShift%20);
				DestArea.w 	= 20;
				DestArea.x	= left-(xShift%20);
				DestArea.h	= 20;
								
				myys = mx+xs+(my+ys)*MapWidth;
				
				if(ml == 0)
					ani = Map[myys+110000];
				else
					ani = Map[myys+165000];
				
				tile = Map[myys+mlt] + ((ani != 0) ? ((long)Animation % ani) : 0);

				if(tile < 160){
					
					SrcArea.y	= (tile>>4)*20;
					SrcArea.w	= 20;
					SrcArea.x	= (tile&15)*20;
					SrcArea.h	= 20;

					SDL_SetSurfacePalette(WinApp->lpddsTiles[0], WinApp->lpddpal);
					SDL_BlitSurface(WinApp->lpddsTiles[0], &SrcArea, WinApp->lpddsback, &DestArea);
				
				}
				else{

					num = tile / 160;
					tile = tile % 160;
					SrcArea.y	= (tile>>4)*20;
					SrcArea.h	= 20;
					SrcArea.x	= (tile&15)*20;
					SrcArea.w	= 20;
				
					SDL_SetSurfacePalette(WinApp->lpddsTiles[num], WinApp->lpddpal);
					SDL_BlitSurface(WinApp->lpddsTiles[num], &SrcArea, WinApp->lpddsback, &DestArea);
				}
				left += 20;
			}
			top += 20;
		}		
		
	Animation += AniSpeed*diff;
	if(Animation > 362880) Animation = 0;	
}

void LD3ENGINE::FlipSurfaces(WINAPP *WinApp)
{
	//- Flip the primary and secondary surfaces
	//-----------------------------------------
	
	SDL_Surface *palResolveSurf = SDL_ConvertSurfaceFormat(WinApp->lpddsback, SDL_PIXELFORMAT_RGBA8888, 0);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(WinApp->lpRenderer, palResolveSurf);
	SDL_RenderClear(WinApp->lpRenderer);
	SDL_UpdateTexture(WinApp->lpddsprimary, 0, palResolveSurf->pixels, palResolveSurf->pitch);
	SDL_RenderCopy(WinApp->lpRenderer, tex, NULL, NULL);
	SDL_RenderPresent(WinApp->lpRenderer);
	SDL_FreeSurface(palResolveSurf);
	SDL_DestroyTexture(tex);
}

void SLX_BlitFlipSurface(SDL_Surface *src, SDL_Rect *srcRect, SDL_Surface *dst, SDL_Rect *dstRect, bool flipH, bool flipV)
{
	if (dstRect->x + srcRect->w > dst->w)
		srcRect->w = dst->w - dstRect->x;
	if (dstRect->x < 0)
	{
		srcRect->w += dstRect->x;
		srcRect->x -= dstRect->x;
		dstRect->x = 0;
	}	
	if (dstRect->y + srcRect->h > dst->h)
		srcRect->h = dst->h - dstRect->y;
	if (dstRect->y < 0)
	{
		srcRect->h += dstRect->y;
		srcRect->y -= dstRect->y;
		dstRect->y = 0;
	}
	if (srcRect->w <= 0 || srcRect->h <= 0)
		return;
	SDL_LockSurface(src);
	SDL_LockSurface(dst);
	uint8_t *srcptr = (uint8_t*)src->pixels + (srcRect->y * src->pitch) + srcRect->x;
	uint8_t *dstptr = (uint8_t*)dst->pixels + (dstRect->y * dst->pitch) + dstRect->x;
	uint32_t colourKey;
	SDL_GetColorKey(src, &colourKey);
	int srcNextPx = flipH?-1:1;
	int srcNextLine = flipV?((flipH?-srcRect->w:srcRect->w)-src->pitch):(src->pitch-(flipH?-srcRect->w:srcRect->w));
	if (flipH) srcptr += srcRect->w-1;
	if (flipV) srcptr += src->pitch * srcRect->h;

	for (int y = 0; y < srcRect->h; ++y)
	{
		for (int x = 0; x < srcRect->w; ++x)
		{
			if (*srcptr != colourKey)
				*dstptr = *srcptr;
			dstptr++;
			srcptr += srcNextPx;
		}
		dstptr += dst->pitch - srcRect->w;
		srcptr += srcNextLine;
	}
	SDL_UnlockSurface(dst);
	SDL_UnlockSurface(src);
}

void LD3ENGINE::PutSprite(WINAPP *WinApp, int x, int y, int Num, int Tile, bool xFlip)
{
	//- Draw the given sprite onto the secondary surface
	//--------------------------------------------------

	UCHAR			*VideoBuffer;
	int				col;
	int				cystart, cyend;
	int             cxstart, cxend;
	int				xadd, yadd;
	int				TileNum = 0;
	SDL_Rect			SrcArea, DestArea;

	/*cxstart = x;    cystart = y;
	cxend   = x+19; cyend   = x+19;
	xadd    = 0;    yadd    = 0;*/
	
	//WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	//	VideoBuffer = (UCHAR *)ddsd->lpSurface;
	
		/*if(x < 0)        {cxstart = 0; xadd = fabs(x);}
		if((x+19) > 319) cxend = 319;
		if(y < 0)        {cystart = 0; yadd = fabs(y);}
		if((y+19) > 239) cyend = 239;*/

		DestArea.y	= y;
		DestArea.h 	= 20;
		DestArea.x	= x;
		DestArea.w	= 20;

		if(Num == -1){

			TileNum = Tile / 160;
			Tile = Tile % 160;

			SrcArea.y	= (Tile>>4)*20;
			SrcArea.h	= 20;
			SrcArea.x	= (Tile & 15)*20;
			SrcArea.w	= 20;		

			//if(xFlip)
			{
				SLX_BlitFlipSurface(WinApp->lpddsTiles[TileNum], &SrcArea, WinApp->lpddsback, &DestArea, xFlip, false);
			}
			/*else
			{	
				SDL_SetSurfacePalette(WinApp->lpddsTiles[TileNum], WinApp->lpddpal);
				SDL_BlitSurface(WinApp->lpddsTiles[TileNum], &SrcArea, WinApp->lpddsback, &DestArea);
			}*/
			
			/*if(xFlip)
			{
				if(Tile < 160)
					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[0], &SrcArea,  DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);
				else{
					Tile -= 160;
					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[1], &SrcArea,  DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);
				}
			
				/*for(int cy = 0; cy <= 19-yadd; cy++)
				{
					for(int cx = 0; cx <= 19-xadd; cx++)
					{					
						col = sTiles[19-cx-xadd+(cy+yadd)*20][Tile];
						if(col > 0) VideoBuffer[cxstart+cx+ (cystart+cy)*ddsd->lPitch] = col;
					}
				}
			}
			else
			{
				if(Tile < 160)
					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[0], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
				else{
					Tile -= 160;
					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[1], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
				}
			
				/*for(int cy = 0; cy <= 19-yadd; cy++)
				{
					for(int cx = 0; cx <= 19-xadd; cx++)
					{					
						col = sTiles[cx+xadd+(cy+yadd)*20][Tile];
						if(col > 0) VideoBuffer[cxstart+cx+ (cystart+cy)*ddsd->lPitch] = col;
					}
				}
			}*/
		}
		else
		{
			SrcArea.y	= (Tile>>4)*20;
			SrcArea.h	= 20;
			SrcArea.x	= (Tile & 15)*20;
			SrcArea.w	= 20;			
		
			SLX_BlitFlipSurface(WinApp->lpddsSprites[Num], &SrcArea, WinApp->lpddsback, &DestArea, xFlip, false);
		
			/*if(xFlip)
			{
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsSprites[Num], &SrcArea,  DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);
			
				/*for(int cy = 0; cy <= 19-yadd; cy++)
				{
					for(int cx = 0; cx <= 19-xadd; cx++)
					{					
						col = Sprite[19-cx-xadd+(cy+yadd)*20][Tile][Num];
						if(col > 0) VideoBuffer[cxstart+cx+ (cystart+cy)*ddsd->lPitch] = col;
					}
				}
			}
			else
			{
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsSprites[Num], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
			
				/*for(int cy = 0; cy <= 19-yadd; cy++)
				{
					for(int cx = 0; cx <= 19-xadd; cx++)
					{					
						col = Sprite[cx+xadd+(cy+yadd)*20][Tile][Num];
						if(col > 0) VideoBuffer[cxstart+cx+ (cystart+cy)*ddsd->lPitch] = col;
					}
				}
			}*/
		}
	//WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::LoadMapIcon(WINAPP *WinApp, const char *Filename, int x, int y)
{
	//- Load a map icon

	SDL_Surface *surf;
	UCHAR			*VideoBuffer;
	long			lp;
	char  ch;

	ifstream BitmapFile(Filename, ios::binary);	

	BitmapFile.seekg(1078);

	surf = WinApp->lpddsback;
	SDL_LockSurface(surf);
		VideoBuffer = (UCHAR *)surf->pixels;

		lp = surf->pitch*(y+30);
		for(int cy = 29; cy >= 0; cy--)
		{
			for(int cx = 0; cx <= 39; cx++)
			{
				BitmapFile.get(ch);
				VideoBuffer[lp+cx+x] = ch;					
			}
			lp -= surf->pitch;
		}

	SDL_UnlockSurface(surf);

	BitmapFile.close();
}

void LD3ENGINE::LoadSplashScreen(WINAPP *WinApp, const char* Filename, int delay)
{
	//- Load a map icon

	SDL_Surface *surf;
	UCHAR			*VideoBuffer;
	long			lp;
	char			ch;
	SDL_Color	SplashPalette[256];
	SDL_Color	FadePalette[256];
	SDL_Color	ShiftPalette[256];
	DWORD			TickCount;
	bool			PaletteChange;
	int				xadd;

	ifstream BitmapFile(Filename, ios::binary);	

	BitmapFile.seekg(54);

	for(int i = 0; i <= 255; i++)
	{
		BitmapFile.get(ch); SplashPalette[i].b		= ch;
		BitmapFile.get(ch); SplashPalette[i].g	= ch;
		BitmapFile.get(ch); SplashPalette[i].r		= ch;
		BitmapFile.get(ch); SplashPalette[i].a	= 255;
	}
	
	memset(FadePalette, 0, sizeof(SDL_Color)*256);
	//memcpy(FadePalette, SplashPalette, sizeof(SDL_Color)*256);
	SDL_FreePalette(WinApp->lpddpal);
	WinApp->lpddpal = SDL_AllocPalette(256);
	SDL_SetSurfacePalette(WinApp->lpddsback, WinApp->lpddpal);
	
	xadd = (WinApp->xRes-320)/2;

	SDL_LockSurface(WinApp->lpddsback);
		VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;

		lp = WinApp->lpddsback->pitch*(WinApp->yRes-((WinApp->yRes-240)/2)-1);//239;
		for(int y = 239; y >= 0; y--)
		{
			for(int x = 0; x <= 319; x++)
			{
				BitmapFile.get(ch);
				if(ch == 255) ch = 0;
				VideoBuffer[lp+x+xadd] = ch;					
			}
			lp -= WinApp->lpddsback->pitch;
		}

	SDL_UnlockSurface(WinApp->lpddsback);

	BitmapFile.close();

	FlipSurfaces(WinApp);

	// TODO(davidgow): If this is timing, we should do a better job.
	for(int i = 0; i <= 390; i++) FlipSurfaces(WinApp);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = SDL_GetTicks();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].r < SplashPalette[n].r){FadePalette[n].r += 2; PaletteChange = true;}
			if(FadePalette[n].g < SplashPalette[n].g){FadePalette[n].g += 2; PaletteChange = true;}
			if(FadePalette[n].b < SplashPalette[n].b){FadePalette[n].b += 2; PaletteChange = true;}
			if(FadePalette[n].r > SplashPalette[n].r) FadePalette[n].r = SplashPalette[n].r;
			if(FadePalette[n].g > SplashPalette[n].g) FadePalette[n].g = SplashPalette[n].g;
			if(FadePalette[n].b > SplashPalette[n].b) FadePalette[n].b = SplashPalette[n].b;
		}
		// TODO(davidgow): We'll just flip again here?
		//WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		FlipSurfaces(WinApp);

		SDL_SetPaletteColors(WinApp->lpddpal, &FadePalette[0], 0, 255);

		//while(GetTickCount()-TickCount < 1.3f);		
		//WinApp->lpddpal->GetEntries(0, 1, 254, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 2, 253, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 1, 1, &FadePalette[253]);
		
	}

	TickCount = SDL_GetTicks();
	while(SDL_GetTicks()-TickCount < delay){
		
		// TODO(davidgow): Again, WaitForVerticalBlank -> FlipSurfaces
		//WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		FlipSurfaces(WinApp);
		
		/*for(int n = 0; n <= 2; n++){
			WinApp->lpddpal->GetEntries(0, 1, 254, FadePalette);
			WinApp->lpddpal->SetEntries(0, 2, 253, FadePalette);
			WinApp->lpddpal->SetEntries(0, 1, 1, &FadePalette[253]);
		}*/
		
		//WinApp.lpddpal->GetEntries(0, 240, 15, ShiftPal);
		//WinApp.lpddpal->SetEntries(0, 241, 14, ShiftPal);
		//WinApp.lpddpal->SetEntries(0, 240, 1, &ShiftPal[14]);
	};

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = SDL_GetTicks();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].r < 255){FadePalette[n].r += 1; PaletteChange = true;}
			if(FadePalette[n].g < 255){FadePalette[n].g += 1; PaletteChange = true;}
			if(FadePalette[n].b < 255){FadePalette[n].b += 1; PaletteChange = true;}
			//if(FadePalette[n].r < 0) FadePalette[n].r = 0;
			//if(FadePalette[n].g < 0) FadePalette[n].g = 0;
			//if(FadePalette[n].b < 0) FadePalette[n].b = 0;
		}
		// TODO(davidgow): We'll just flip again here?
		//WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		FlipSurfaces(WinApp);

		SDL_SetPaletteColors(WinApp->lpddpal, &FadePalette[0], 0, 255);

		//while(GetTickCount()-TickCount < 1.3f);
		//WinApp->lpddpal->GetEntries(0, 1, 254, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 2, 253, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 1, 1, &FadePalette[253]);
		
	}

	for(int i = 0; i <= 150; i++) FlipSurfaces(WinApp);
	
	ClearBuffer(WinApp, 0);	FlipSurfaces(WinApp);
	// TODO(davidgow): We're not really flipping, so this second one is redundant.
	ClearBuffer(WinApp, 0);	FlipSurfaces(WinApp);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = SDL_GetTicks();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].r > 0){FadePalette[n].r -= 5; PaletteChange = true;}
			if(FadePalette[n].g > 0){FadePalette[n].g -= 5; PaletteChange = true;}
			if(FadePalette[n].b > 0){FadePalette[n].b -= 5; PaletteChange = true;}			
		}
		FlipSurfaces(WinApp);
		SDL_SetPaletteColors(WinApp->lpddpal, &FadePalette[0], 0, 255);

	}

	SDL_FreePalette(WinApp->lpddpal);
	WinApp->lpddpal = SDL_AllocPalette(256);
	SDL_SetSurfacePalette(WinApp->lpddsback, WinApp->lpddpal);
}

/*void LD3ENGINE::LoadSky(WINAPP *WinApp, char* Filename, int pos, int Convert)
{
	//- Load the sky

	byte  ch;
	char  ConvertTable[256];
	byte  red, grn, blu;
	int   diff;
	int   rdif, gdif, bdif;
	int   newcol;
	int   av;


	ifstream BitmapFile(Filename, ios::binary);

	
	if(Convert == 0)
	{
		BitmapFile.seekg(54);

		for(int i = 0; i <= 255; i++)
		{
			BitmapFile.get(ch); WinApp->palette[i].b	= ch;
			BitmapFile.get(ch); WinApp->palette[i].g	= ch;
			BitmapFile.get(ch); WinApp->palette[i].r	= ch;
			BitmapFile.get(ch); WinApp->palette[i].a 	= 255;
		}
	}
	else
	{
		BitmapFile.seekg(54);

		for(int i = 0; i <= 255; i++)
		{
			BitmapFile.get(blu);
			BitmapFile.get(grn);
			BitmapFile.get(red);
			BitmapFile.get(ch);
			diff = 256; newcol = i;
			for(int n = 0; n <= 255; n++)
			{
				rdif = abs(WinApp->palette[n].r   - red);
				gdif = abs(WinApp->palette[n].g - grn);
				bdif = abs(WinApp->palette[n].b  - blu);
				av = (rdif + gdif + bdif) / 3;
				if(av < diff)
				{
					diff = av;
					newcol = n;
				}
			}
			ConvertTable[i] = newcol;
		}
	}

	for(int y = 63; y >= 0; y--)
	{
		for(int x = 0; x <= 127; x++)
		{
			BitmapFile.get(ch);
			if(Convert == 0){
				Sky[x][y] = ch;}
			else{
				Sky[x][y] = ConvertTable[ch];}			
		}
	}

	if(Convert == 0){
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, WinApp->palette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);}
	
	BitmapFile.close();
}*/

void LD3ENGINE::LoadSky(WINAPP *WinApp, const char* Filename, int pos, int Convert)
{
	//- Load the sky

	char  ch;
	char  ConvertTable[256];
	byte  red, grn, blu;
	int   diff;
	int   rdif, gdif, bdif;
	int   newcol;
	int   av;


	ifstream BitmapFile(Filename, ios::binary);

	
	if(Convert == 0)
	{
		BitmapFile.seekg(54);

		for(int i = 0; i <= 255; i++)
		{
			BitmapFile.get(ch); WinApp->palette[i].b	= ch;
			BitmapFile.get(ch); WinApp->palette[i].g	= ch;
			BitmapFile.get(ch); WinApp->palette[i].r	= ch;
			BitmapFile.get(ch); WinApp->palette[i].a 	= 255;
		}
	}
	else if(Convert == 1)
	{
		BitmapFile.seekg(54);

		for(int i = 0; i <= 255; i++)
		{
			BitmapFile.get(ch); blu = ch;
			BitmapFile.get(ch); grn = ch;
			BitmapFile.get(ch); red = ch;
			BitmapFile.get(ch);
			diff = 256; newcol = i;
			for(int n = 0; n <= 255; n++)
			{
				rdif = abs(WinApp->palette[n].r   - red);
				gdif = abs(WinApp->palette[n].g - grn);
				bdif = abs(WinApp->palette[n].b  - blu);
				av = (rdif + gdif + bdif) / 3;
				if(av < diff)
				{
					diff = av;
					newcol = n;
				}
			}
			ConvertTable[i] = newcol;
		}
	}
	else
		BitmapFile.seekg(1078);

	for(int y = 255; y >= 0; y--)
	{
		for(int x = 0; x <= 511; x++)
		{
			BitmapFile.get(ch);
			if(Convert == 1)
				Sky[x][y] = ConvertTable[ch];
			else
				Sky[x][y] = ch;
		}
		SkyScroll[y] = 2;
	}

	if(Convert == 0){
		WinApp->lpddpal = SDL_AllocPalette(256);
		SDL_SetPaletteColors(WinApp->lpddpal, WinApp->palette, 0, 256);
		SDL_SetSurfacePalette(WinApp->lpddsback, WinApp->lpddpal);}
	
	BitmapFile.close();
}

void LD3ENGINE::SetSkyScroll(int yStart, int yEnd, int ShiftNum)
{
	for(int y = yStart; y <= yEnd; y++) SkyScroll[y] = ShiftNum;
}

void LD3ENGINE::WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, bool trans)
{
	SDL_Surface *surf;
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn, ovn;
	int				tx, ty, spchar, length;
	UCHAR			col;

	
	surf = WinApp->lpddsback;
	SDL_LockSurface(surf);
		VideoBuffer = (UCHAR *)surf->pixels;

		lp = surf->pitch;	
		tx = x;
		length = strlen(Text);
		if(center)
			tx = (WinApp->xRes-length*7)>>1;

		ty = y;
		ovn = ty*lp;
		
		if(trans)
		{
			for(int i = 0; i < length; i++)
			{
				vn = ovn;
				spchar = Text[i]-32;
				for(int cy = 0; cy <= 4; cy++){
					for(int cx = 0; cx <= 5; cx++){
						col = Font[cx][cy][spchar];
						if(col) VideoBuffer[tx+cx+vn] = col;
					}
					vn += lp;
				}
				tx += 7;
			}		
		}
		else
		{
			for(int i = 0; i < length; i++)
			{
				vn = ovn;
				spchar = Text[i]-32;
				for(int cy = 0; cy <= 4; cy++){
					for(int cx = 0; cx <= 5; cx++){
						col = Font[cx][cy][spchar];
						VideoBuffer[tx+cx+vn] = col;
					}
					vn += lp;
				}
				tx += 7;
			}
		}

	SDL_UnlockSurface(surf);
}

void LD3ENGINE::WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, DWORD fcol, bool trans)
{
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn, ovn;
	int				tx, ty, spchar, length;
	UCHAR			col;

	
	SDL_LockSurface(WinApp->lpddsback);
		VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;

		lp = WinApp->lpddsback->pitch;	
		tx = x;
		length = strlen(Text);
		if(center)
			tx = (WinApp->xRes-length*7)>>1;

		ty = y;
		ovn = ty*lp;
		
		if(trans)
		{
			for(int i = 0; i < length; i++)
			{
				vn = ovn;
				spchar = Text[i]-32;
				for(int cy = 0; cy <= 4; cy++){
					for(int cx = 0; cx <= 5; cx++){
						col = Font[cx][cy][spchar];
						if(col) VideoBuffer[tx+cx+vn] = fcol;
					}
					vn += lp;
				}
				tx += 7;
			}		
		}
		else
		{
			for(int i = 0; i < length; i++)
			{
				vn = ovn;
				spchar = Text[i]-32;
				for(int cy = 0; cy <= 4; cy++){
					for(int cx = 0; cx <= 5; cx++){
						col = Font[cx][cy][spchar];
						VideoBuffer[tx+cx+vn] = fcol;
					}
					vn += lp;
				}
				tx += 7;
			}
		}

	SDL_UnlockSurface(WinApp->lpddsback);
}

void LD3ENGINE::DrawTalkBox(WINAPP *WinApp, int sy)
{
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn;
	long			syXlp;

	
	SDL_LockSurface(WinApp->lpddsback);
		VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;

		lp = WinApp->lpddsback->pitch;
		syXlp = sy*lp;
		vn = syXlp;

		for(int y = 0; y <= 18; y++){
			for(int x = 19; x <= WinApp->xRes-2; x++){
				VideoBuffer[x+vn] = 68+(((x>>1)&1)^((y>>1)&1));
			}
			vn += lp;
		}

		vn = syXlp;
		for(int x = 0; x <= WinApp->xRes-1; x++) VideoBuffer[x+vn] = 28;
		vn = lp*(19+sy);
		for(int x = 0; x <= WinApp->xRes-1; x++) VideoBuffer[x+vn] = 28;
		vn = syXlp;
		for(int y = 0; y <= 19; y++){
			VideoBuffer[0+vn]  = 28;
			VideoBuffer[WinApp->xRes-1+vn] = 28;
			vn += lp;
		}

	SDL_UnlockSurface(WinApp->lpddsback);
}

void LD3ENGINE::DrawLetterBox(WINAPP *WinApp)
{
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn;
	UCHAR			col;

	
	SDL_LockSurface(WinApp->lpddsback);
		VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;

		lp = WinApp->lpddsback->pitch;
		vn = 0;

		for(int y = 0; y <= 39; y++){
			for(int x = 0; x <= WinApp->xRes; x++){
				VideoBuffer[x+vn] = 0;
			}
			vn += lp;
		}
		
		vn = (WinApp->yRes-40)*lp;
		for(int y = 0; y <= 39; y++){
			for(int x = 0; x <= WinApp->xRes; x++){
				VideoBuffer[x+vn] = 0;
			}
			vn += lp;
		}

	SDL_UnlockSurface(WinApp->lpddsback);
}

void LD3ENGINE::PutPixel(WINAPP *WinApp, int x, int y, int col)
{
	//- Place a pixel on the surface
	SDL_Rect area;

	area.x = x;
	area.y = y;
	area.w = 1;
	area.h = 1;

	SDL_FillRect(WinApp->lpddsback, &area, col);
}

void LD3ENGINE::DrawBox(WINAPP *WinApp, int x1, int y1, int x2, int y2, int col, bool fill)
{
	//- Draw a filled box with the given color
	SDL_Surface *surf = WinApp->lpddsback;
	UCHAR			*VideoBuffer;
	long			lp, xs, ys;

	SDL_LockSurface(surf);
		VideoBuffer = (UCHAR *)surf->pixels;

		if(fill){
			lp = x1+y1*surf->pitch;
			xs = abs(x2-x1)+1;		
			for(int y = y1; y <= y2; y++){
				for(int x = x1; x <= x2; x++){
					VideoBuffer[lp] = col;
					lp++;
				}
				lp += surf->pitch;
				lp -= xs;
			}
		}
		else{
			lp = y1*surf->pitch;
			xs = abs(x2-x1)+1;
			ys = abs(y2-y1)*surf->pitch;
			for(int y = y1; y <= y2; y++){
				VideoBuffer[lp+x1] = col;
				VideoBuffer[lp+x2] = col;
				lp += surf->pitch;				
			}
			lp = y1*surf->pitch;
			for(int x = x1+1; x <= x2-1; x++){
				VideoBuffer[lp+x] = col;
				VideoBuffer[lp+ys+x] = col;
			}
		}
	
	SDL_UnlockSurface(surf);
}

void LD3ENGINE::TakeScreenshot(WINAPP *WinApp)
{
	//- Take a screenshot

	UCHAR *VideoBuffer;
	char Filename[80];
	char SSNo[8];
	ofstream bfile;

	while(NO_ONE_CARES){
		ScreenshotNum++;		
		sprintf(Filename, "screenshots/shot%d.bmp", ScreenshotNum);
		
		//TODO(davidgow): Use stat() or similar to check if the file exists.
		break;	
	}

	//strcpy(Filename, "screenshots/shot.bmp");

	bfile.open(Filename, ios::binary);
	//bfile.open(Filename, ios::binary);
	
	//ofstream bfile(Filename);
	
	UCHAR		ch;
	int			r1;
	int			r2;
	int			r3;
	int			size;
	
	bfile.put('B');
	bfile.put('M');
	
	size = xRes*yRes+1078;
	r1 = size; r2 = 0; r3 = 0;
	while(r1 >= 256)
	{
		r1 -= 256;
		r2++;
		if(r2 >= 256){
			r2 = 0;
			r3++;}
	}
		
	ch = r1;	bfile.put(ch);
	ch = r2;	bfile.put(ch);
	ch = r3;	bfile.put(ch);
		
	bfile.seekp(10);
	ch = 54;	bfile.put(ch);
	ch = 4;		bfile.put(ch);
		
	bfile.seekp(14);
	ch = 40;	bfile.put(ch);
		
	bfile.seekp(26);
	ch = 1;		bfile.put(ch);
	ch = 0;		bfile.put(ch);
	ch = 8;		bfile.put(ch);
	
	bfile.seekp(18);
	
	r1 = xRes;
	r2 = 0;
	while(r1 >= 256)
	{
		r1 -= 256;
		r2++;
	}
	
	ch = r1;	bfile.put(ch);
	ch = r2;	bfile.put(ch);
	
	bfile.seekp(22);
	
	r1 = yRes;
	r2 = 0;
	while(r1 >= 256)
	{
		r1 -= 256;
		r2++;
	}
	
	ch = r1; bfile.put(ch);
	ch = r2; bfile.put(ch);
		
	bfile.seekp(54);
	long fcount = 54;
	// save palette data
	for(int i = 0; i <= 255; i++)
	{
		ch = WinApp->palette[i].b;		bfile.put(ch);
		ch = WinApp->palette[i].g;	bfile.put(ch);
		ch = WinApp->palette[i].r;		bfile.put(ch);
		ch = 0;	bfile.put(ch);
		fcount += 4;
	}

	// save image data
	SDL_LockSurface(WinApp->lpddsback);
	VideoBuffer = (UCHAR *)WinApp->lpddsback->pixels;
	for(int y = yRes-1; y >= 0; y--)
	{
		for(int x = 0; x <= xRes-1; x++)
		{
			ch = VideoBuffer[x + y*WinApp->lpddsback->pitch];
			bfile.seekp(fcount);
			bfile.put(ch);
			fcount++;
		}		
	}
	SDL_UnlockSurface(WinApp->lpddsback);
	
	bfile.close();	
}

void LD3ENGINE::FadeIn(WINAPP *WinApp)
{
/*	bool PaletteChange;
	long TickCount;
	SDL_Color FadePalette[256];
	memset(FadePalette, 0, sizeof(SDL_Color)*256);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		WinApp->lpddpal->Release();
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].r < WinApp->palette[n].r){FadePalette[n].r += 1; PaletteChange = true;}
			if(FadePalette[n].g < WinApp->palette[n].g){FadePalette[n].g += 1; PaletteChange = true;}
			if(FadePalette[n].b < WinApp->palette[n].b){FadePalette[n].b += 1; PaletteChange = true;}
		}

		while(GetTickCount()-TickCount < 1.3f);
		//LD3.DrawMap(WinApp, 0, 0, 0);
		//LD3.DrawMap(WinApp, 0, 0, 1);	

		FlipSurfaces(WinApp);
	}
*/
	//TickCount = GetTickCount();
	//while(GetTickCount()-TickCount < 1000);
}

void LD3ENGINE::FadeOut(WINAPP *WinApp)
{
	/*bool PaletteChange;
	long TickCount;
	SDL_Color FadePalette[256];
	memcpy(&FadePalette, WinApp->palette, sizeof(SDL_Color)*256);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		WinApp->lpddpal->Release();
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].r > 0){FadePalette[n].r -= 1; PaletteChange = true;}
			if(FadePalette[n].g > 0){FadePalette[n].g -= 1; PaletteChange = true;}
			if(FadePalette[n].b > 0){FadePalette[n].b -= 1; PaletteChange = true;}
		}

		while(GetTickCount()-TickCount < 1.3f);
	}

	TickCount = GetTickCount();
	while(GetTickCount()-TickCount < 1000);*/
}

void LD3ENGINE::WaitForRetrace(WINAPP *WinApp)
{
	//WinApp->lpddsprimary->WaitForVerticalBlank(DDWAITVB_BLOBKBEGIN, NULL);
	//WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	//TODO(davidgow): implement
}
