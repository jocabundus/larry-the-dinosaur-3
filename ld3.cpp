//- Larry The Dinosaur 3 Engine Source File
//- November, 2002 - Created by Joe King
//===========================================

//#include "ld3.h"
#include "main.h"

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

void LD3ENGINE::LoadPalette(WINAPP *WinApp, char* Filename)
{
	//- Load the palette
	//------------------

	ifstream PalFile(Filename, ios::binary);
	PALETTEENTRY	*palette;
	char			ch;	
	
	palette = &WinApp->palette[0];
	
	for(int i = 0; i <= 255; i++)
	{
		PalFile.get(ch); palette[i].peRed	= ch*4;
		PalFile.get(ch); palette[i].peGreen	= ch*4;
		PalFile.get(ch); palette[i].peBlue	= ch*4;
		
		palette[i].peFlags = PC_NOCOLLAPSE;		
	}
	
	PalFile.close();	
}

void LD3ENGINE::LoadTileSet(WINAPP *WinApp, char* Filename, int start)
{
	//- Load the tile sprites
	//-----------------------

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*TileBuffer;
	long			lp;
	int		n = start;
	int		tl = start % 160;
	char	ch;

	ifstream SpriteFile(Filename, ios::binary);
	
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

	WinApp->lpddsTiles[start/160]->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		TileBuffer = (UCHAR *)ddsd->lpSurface;
	
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
					TileBuffer[((tl&15)*20)+x+(y+((tl>>4)*20))*ddsd->lPitch] = ch;
				}
			}
			//n++;
			tl++;
			if(tl >= 160){
				WinApp->lpddsTiles[start/160]->Unlock(NULL);
				start += 160;
				WinApp->lpddsTiles[start/160]->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
				TileBuffer = (UCHAR *)ddsd->lpSurface;
				tl = 0;
			}
		}
	WinApp->lpddsTiles[start/160]->Unlock(NULL);

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

void LD3ENGINE::LoadMap(char* Filename)
{
	//- Load the map with dialog box selection
	//----------------------------------------
	
	UCHAR			ch1, ch2;
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
					m = ch1<<8;
					m += ch2;
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

void LD3ENGINE::LoadSprite(WINAPP *WinApp, char* Filename, int Num, int start)
{
	//- Load a character sprite file
	//------------------------------

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*SpriteBuffer;
	char			ch;
	int				n = start;
	long			lp;

	ifstream SpriteFile(Filename, ios::binary);
	
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);
	SpriteFile.get(ch); SpriteFile.get(ch); SpriteFile.get(ch);

	WinApp->lpddsSprites[Num]->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		SpriteBuffer = (UCHAR *)ddsd->lpSurface;
	
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
					SpriteBuffer[((n&15)*20)+x+(y+((n>>4)*20))*ddsd->lPitch] = ch;					
				}
			}
			n++;
		}
	WinApp->lpddsSprites[Num]->Unlock(NULL);
			
	SpriteFile.close();	
}

void LD3ENGINE::LoadFont(char* Filename)
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

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	long			lp;

	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;
	
		lp = 0;
		for(int y = 0; y < yRes; y++){
			for(int x = 0; x < xRes; x++){
				VideoBuffer[lp+x] = col;
			}
			lp += ddsd->lPitch;
		}		
		
	WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::DrawSky(WINAPP *WinApp, int xShift, int yShift)
{
	//- Draw the sky
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	long			lp;
	static			CloudCount = 0;
	RECT			SrcArea, DestArea;
	bool			FillBlank = false;
	int				ss = 0;
	int				flip = 0;
	

	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;
	
		/*lp = 0;
		for(int y = 0; y < 64; y++){
			for(int x = 0; x < xRes; x++){
				VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>4)) & 127][y][0];
			}
			lp += ddsd->lPitch;
		}
		for(y = 0; y < 64; y++){
			for(int x = 0; x < xRes; x++){
				VideoBuffer[lp+x] = Sky[(x+(xShift>>3)) & 127][y][1];
			}
			lp += ddsd->lPitch;
		}
		for(y = 0; y < 64; y++){
			for(int x = 0; x < xRes; x++){
				VideoBuffer[lp+x] = Sky[(x+(xShift>>2)) & 127][y][2];
			}
			lp += ddsd->lPitch;
		}
		if(WinApp->yRes <= 256){
			for(y = 0; y < WinApp->yRes-192; y++){
				for(int x = 0; x < xRes; x++){
					VideoBuffer[lp+x] = Sky[(x+(xShift>>1)) & 127][y][3];
				}
				lp += ddsd->lPitch;
			}
		}
		else
		{
			for(y = 0; y < 64; y++){
				for(int x = 0; x < xRes; x++){
					VideoBuffer[lp+x] = Sky[(x+(xShift>>1)) & 127][y][3];
				}
				lp += ddsd->lPitch;
			}
			DestArea.top = 256;
			DestArea.bottom = WinApp->yRes;
			DestArea.left = 0;
			DestArea.right = WinApp->xRes;
			WinApp->ddbltfx.dwFillColor = Sky[1][63][3];
			FillBlank = true;			
		}*/

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
					lp += ddsd->lPitch;
				}
			}
			else{
				for(int y = 0; y < yRes; y++){
					ss = SkyScroll[y+(yShift>>SkyVerticalScrollShift) & 255];
					for(int x = 0; x < xRes; x++){
						VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>ss)) & 511][y+(yShift>>SkyVerticalScrollShift) & 255];
					}
					lp += ddsd->lPitch;
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
					lp += ddsd->lPitch;
				}
			}
			else{
				for(int y = 0; y < yRes; y++){
					ss = SkyScroll[y];
					for(int x = 0; x < xRes; x++){
						VideoBuffer[lp+x] = Sky[(x+((xShift+CloudCount)>>ss)) & 511][y];
					}
					lp += ddsd->lPitch;
				}
			}
		}
		

		//CloudCount += 4;
		//if(CloudCount > 2047) CloudCount -= 2048;
		//for(int y = 0; y < yRes-(yShift>>4); y++){
		//	for(int x = 0; x < xRes; x++){
		//		VideoBuffer[x+y*ddsd->lPitch] = Sky[(x+(xShift>>2))%1280][(y+(yShift>>4))%240];
		//	}			
		//}
		//for(y = yRes-(yShift>>4); y < yRes; y++){
		//	for(int x = 0; x < xRes; x++){
		//		VideoBuffer[x+y*ddsd->lPitch] = Sky[(x+(xShift>>2))%1280][239];
		//	}			
		//}
		
	WinApp->lpddsback->Unlock(NULL);
	
	if(FillBlank) WinApp->lpddsback->Blt(&DestArea, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &WinApp->ddbltfx);
}

void LD3ENGINE::DrawMap(WINAPP *WinApp, int xShift, int yShift, int layer, float diff)
{
	//- Draw the map onto the screen
	//------------------------------

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
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
	RECT			SrcArea, DestArea;
	int				top, left;
	
	xs = xShift / 20;
	ys = yShift / 20;
	mxmax = xRes/20;
	mymax = yRes/20;
	
	ml = layer;
	long myys;
	
	//WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	//	VideoBuffer = (UCHAR *)ddsd->lpSurface;
		
		mlt = ml*55000; top = 0;
		for(my = 0; my <= mymax; my++)
		{
			left = 0;
			for(mx = 0; mx <= mxmax; mx++)
			{
				/*cxstart = (mx+1)*20-20-(xShift%20);	cystart = (my+1)*20-20-(yShift%20);
				cxend	= cxstart+19;				cyend	= cystart+19;
				xadd    = 0;						yadd    = 0;
				xmin	= 0;						ymin	= 0;

				if(mx == 0 || my == 0 || mx == 16 || my == 12)
				{
					if(cxstart < 0)			{xadd = abs(cxstart); cxstart = 0;}
					if(cxend > 319)			{cxend = 319; xmin = 19-abs(cxend-cxstart);}
					if(cystart < 0)			{yadd = abs(cystart); cystart = 0;}
					if(cyend > 239)			{cyend = 239; ymin = 19-abs(cyend-cystart);}
				}*/
				DestArea.top	= top-(yShift%20);
				DestArea.bottom = DestArea.top+20;
				DestArea.left	= left-(xShift%20);
				DestArea.right	= DestArea.left+20;
								
				myys = mx+xs+(my+ys)*MapWidth;
				
				if(ml == 0)
					ani = Map[myys+110000];
				else
					ani = Map[myys+165000];
				
				tile = Map[myys+mlt] + ((long)Animation % ani);

				if(tile < 160){
					
					SrcArea.top		= (tile>>4)*20;
					SrcArea.bottom	= SrcArea.top+20;
					SrcArea.left	= (tile&15)*20;
					SrcArea.right	= SrcArea.left+20;

					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[0], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
				
					/*for(int cy = 0; cy <= 19-yadd-ymin; cy++)
					{
						for(int cx = 0; cx <= 19-xadd-xmin; cx++)
						{					
							col = sTiles[cx+xadd+(cy+yadd)*20][tile];
							if(col > 0) VideoBuffer[cxstart+cx+ (cystart+cy)*ddsd->lPitch] = col;							
						}
					}*/
				}
				else{

					num = tile / 160;
					tile = tile % 160;
					SrcArea.top		= (tile>>4)*20;
					SrcArea.bottom	= SrcArea.top+20;
					SrcArea.left	= (tile&15)*20;
					SrcArea.right	= SrcArea.left+20;
				
					WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[num], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
				}
				left += 20;
			}
			top += 20;
		}		
		
	//WinApp->lpddsback->Unlock(NULL);
	Animation += AniSpeed*diff;
	if(Animation > 362880) Animation = 0;	
}

void LD3ENGINE::FlipSurfaces(WINAPP *WinApp)
{
	//- Flip the primary and secondary surfaces
	//-----------------------------------------
	 
	//while(WinApp->lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK);
	if(WinApp->WindowMode == false)
		while(WinApp->lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK);
	else{

		RECT DestArea, SrcArea;
		RECT WindowArea, ClientArea;
		
		GetWindowRect(WinApp->hwnd, &WindowArea);
		GetClientRect(WinApp->hwnd, &ClientArea);	

		DestArea.top	= WindowArea.top;				SrcArea.top	= 0;
		DestArea.bottom	= WindowArea.top+WinApp->yRes;	SrcArea.bottom	= WinApp->yRes;
		DestArea.left	= WindowArea.left;				SrcArea.left	= 0;
		DestArea.right	= WindowArea.left+WinApp->xRes;	SrcArea.right	= WinApp->xRes;
		
		WinApp->lpddsprimary->Blt(&DestArea, WinApp->lpddsback, &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);

		if(WindowResized){
			/*RGNDATA regiondata;

			regiondata.rdh.dwSize			= sizeof(RGNDATAHEADER);
			regiondata.rdh.iType			= RDH_RECTANGLES;
			regiondata.rdh.nCount			= 1;
			regiondata.rdh.nRgnSize			= sizeof(RECT);
			regiondata.rdh.rcBound.top		= 0;
			regiondata.rdh.rcBound.bottom	= yRes-1;
			regiondata.rdh.rcBound.left		= 0;
			regiondata.rdh.rcBound.right	= xRes-1;

			ClipArea.top = 0;
			ClipArea.bottom = yRes;
			ClipArea.left = 0;
			ClipArea.right = xRes;
			memcpy(regiondata.Buffer, ClipArea, sizeof(RECT));
			//WinApp->lpddclipper->Release();
			WinApp->lpdd->CreateClipper(0, &WinApp->lpddclipper, NULL);
			WinApp->lpddclipper->SetClipList(&regiondata, 0);
			WinApp->lpddsback->SetClipper(WinApp->lpddclipper);*/
			WindowResized = false;
		}	
	}
}

void LD3ENGINE::PutSprite(WINAPP *WinApp, int x, int y, int Num, int Tile, bool xFlip)
{
	//- Draw the given sprite onto the secondary surface
	//--------------------------------------------------

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	int				col;
	int				cystart, cyend;
	int             cxstart, cxend;
	int				xadd, yadd;
	int				TileNum = 0;
	RECT			SrcArea, DestArea;

	/*cxstart = x;    cystart = y;
	cxend   = x+19; cyend   = x+19;
	xadd    = 0;    yadd    = 0;*/
	
	//WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	//	VideoBuffer = (UCHAR *)ddsd->lpSurface;
	
		/*if(x < 0)        {cxstart = 0; xadd = fabs(x);}
		if((x+19) > 319) cxend = 319;
		if(y < 0)        {cystart = 0; yadd = fabs(y);}
		if((y+19) > 239) cyend = 239;*/

		DestArea.top	= y;
		DestArea.bottom = y+20;
		DestArea.left	= x;
		DestArea.right	= x+20;

		if(Num == -1){

			TileNum = Tile / 160;
			Tile = Tile % 160;

			SrcArea.top		= (Tile>>4)*20;
			SrcArea.bottom	= SrcArea.top+20;
			SrcArea.left	= (Tile & 15)*20;
			SrcArea.right	= SrcArea.left+20;		

			if(xFlip)
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[TileNum], &SrcArea,  DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);
			else
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsTiles[TileNum], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
			
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
			SrcArea.top		= (Tile>>4)*20;
			SrcArea.bottom	= SrcArea.top+20;
			SrcArea.left	= (Tile & 15)*20;
			SrcArea.right	= SrcArea.left+20;			
		
			if(xFlip)
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsSprites[Num], &SrcArea,  DDBLT_KEYSRC | DDBLT_WAIT | DDBLT_DDFX, &WinApp->ddbltfx);
			else
				WinApp->lpddsback->Blt(&DestArea, WinApp->lpddsSprites[Num], &SrcArea, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
		
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

void LD3ENGINE::LoadMapIcon(WINAPP *WinApp, char *Filename, int x, int y)
{
	//- Load a map icon

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	long			lp;
	byte  ch;

	ifstream BitmapFile(Filename, ios::binary);	

	BitmapFile.seekg(1078);

	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch*(y+30);
		for(int cy = 29; cy >= 0; cy--)
		{
			for(int cx = 0; cx <= 39; cx++)
			{
				BitmapFile.get(ch);
				VideoBuffer[lp+cx+x] = ch;					
			}
			lp -= ddsd->lPitch;
		}

	WinApp->lpddsback->Unlock(NULL);

	BitmapFile.close();
}

void LD3ENGINE::LoadSplashScreen(WINAPP *WinApp, char* Filename, int delay)
{
	//- Load a map icon

	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	long			lp;
	byte			ch;
	PALETTEENTRY	SplashPalette[256];
	PALETTEENTRY	FadePalette[256];
	PALETTEENTRY	ShiftPalette[256];
	DWORD			TickCount;
	bool			PaletteChange;
	int				xadd;

	ifstream BitmapFile(Filename, ios::binary);	

	BitmapFile.seekg(54);

	for(int i = 0; i <= 255; i++)
	{
		BitmapFile.get(ch); SplashPalette[i].peBlue		= ch;
		BitmapFile.get(ch); SplashPalette[i].peGreen	= ch;
		BitmapFile.get(ch); SplashPalette[i].peRed		= ch;
		BitmapFile.get(ch); SplashPalette[i].peFlags	= NULL;
	}
	
	memset(FadePalette, 0, sizeof(PALETTEENTRY)*256);
	//memcpy(FadePalette, SplashPalette, sizeof(PALETTEENTRY)*256);
	WinApp->lpddpal->Release();
	WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
	WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
	
	xadd = (WinApp->xRes-320)/2;

	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch*(WinApp->yRes-((WinApp->yRes-240)/2)-1);//239;
		for(int y = 239; y >= 0; y--)
		{
			for(int x = 0; x <= 319; x++)
			{
				BitmapFile.get(ch);
				if(ch == 255) ch = 0;
				VideoBuffer[lp+x+xadd] = ch;					
			}
			lp -= ddsd->lPitch;
		}

	WinApp->lpddsback->Unlock(NULL);

	BitmapFile.close();

	FlipSurfaces(WinApp);

	for(i = 0; i <= 390; i++) WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].peRed < SplashPalette[n].peRed){FadePalette[n].peRed += 2; PaletteChange = true;}
			if(FadePalette[n].peGreen < SplashPalette[n].peGreen){FadePalette[n].peGreen += 2; PaletteChange = true;}
			if(FadePalette[n].peBlue < SplashPalette[n].peBlue){FadePalette[n].peBlue += 2; PaletteChange = true;}
			if(FadePalette[n].peRed > SplashPalette[n].peRed) FadePalette[n].peRed = SplashPalette[n].peRed;
			if(FadePalette[n].peGreen > SplashPalette[n].peGreen) FadePalette[n].peGreen = SplashPalette[n].peGreen;
			if(FadePalette[n].peBlue > SplashPalette[n].peBlue) FadePalette[n].peBlue = SplashPalette[n].peBlue;
		}
		WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		WinApp->lpddpal->SetEntries(0, 0, 255, &FadePalette[0]);

		//while(GetTickCount()-TickCount < 1.3f);		
		//WinApp->lpddpal->GetEntries(0, 1, 254, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 2, 253, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 1, 1, &FadePalette[253]);
		
	}

	TickCount = GetTickCount();
	while(GetTickCount()-TickCount < delay){
		
		WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		
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
		TickCount = GetTickCount();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].peRed < 255){FadePalette[n].peRed += 1; PaletteChange = true;}
			if(FadePalette[n].peGreen < 255){FadePalette[n].peGreen += 1; PaletteChange = true;}
			if(FadePalette[n].peBlue < 255){FadePalette[n].peBlue += 1; PaletteChange = true;}
			//if(FadePalette[n].peRed < 0) FadePalette[n].peRed = 0;
			//if(FadePalette[n].peGreen < 0) FadePalette[n].peGreen = 0;
			//if(FadePalette[n].peBlue < 0) FadePalette[n].peBlue = 0;
		}
		WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		WinApp->lpddpal->SetEntries(0, 0, 255, &FadePalette[0]);

		//while(GetTickCount()-TickCount < 1.3f);
		//WinApp->lpddpal->GetEntries(0, 1, 254, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 2, 253, FadePalette);
		//WinApp->lpddpal->SetEntries(0, 1, 1, &FadePalette[253]);
		
	}

	for(i = 0; i <= 150; i++) WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	
	ClearBuffer(WinApp, 0);	FlipSurfaces(WinApp);
	ClearBuffer(WinApp, 0);	FlipSurfaces(WinApp);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		//WinApp->lpddpal->Release();
		//WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		//WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].peRed > 0){FadePalette[n].peRed -= 5; PaletteChange = true;}
			if(FadePalette[n].peGreen > 0){FadePalette[n].peGreen -= 5; PaletteChange = true;}
			if(FadePalette[n].peBlue > 0){FadePalette[n].peBlue -= 5; PaletteChange = true;}			
		}
		WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
		WinApp->lpddpal->SetEntries(0, 0, 255, &FadePalette[0]);

	}

	WinApp->lpddpal->Release();
	WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, WinApp->palette, &WinApp->lpddpal, NULL);
	WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);	
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
			BitmapFile.get(ch); WinApp->palette[i].peBlue	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peGreen	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peRed	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peFlags = NULL;
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
				rdif = abs(WinApp->palette[n].peRed   - red);
				gdif = abs(WinApp->palette[n].peGreen - grn);
				bdif = abs(WinApp->palette[n].peBlue  - blu);
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

void LD3ENGINE::LoadSky(WINAPP *WinApp, char* Filename, int pos, int Convert)
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
			BitmapFile.get(ch); WinApp->palette[i].peBlue	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peGreen	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peRed	= ch;
			BitmapFile.get(ch); WinApp->palette[i].peFlags = NULL;
		}
	}
	else if(Convert == 1)
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
				rdif = abs(WinApp->palette[n].peRed   - red);
				gdif = abs(WinApp->palette[n].peGreen - grn);
				bdif = abs(WinApp->palette[n].peBlue  - blu);
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
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, WinApp->palette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);}
	
	BitmapFile.close();
}

void LD3ENGINE::SetSkyScroll(int yStart, int yEnd, int ShiftNum)
{
	for(int y = yStart; y <= yEnd; y++) SkyScroll[y] = ShiftNum;
}

void LD3ENGINE::WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, bool trans)
{
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn, ovn;
	int				tx, ty, spchar, length;
	UCHAR			col;

	
	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch;	
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

	WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::WriteText(WINAPP *WinApp, int x, int y, char* Text, bool center, DWORD fcol, bool trans)
{
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn, ovn;
	int				tx, ty, spchar, length;
	UCHAR			col;

	
	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch;	
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

	WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::DrawTalkBox(WINAPP *WinApp, int sy)
{
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn;
	long			syXlp;

	
	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch;
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
		for(x = 0; x <= WinApp->xRes-1; x++) VideoBuffer[x+vn] = 28;
		vn = syXlp;
		for(y = 0; y <= 19; y++){
			VideoBuffer[0+vn]  = 28;
			VideoBuffer[WinApp->xRes-1+vn] = 28;
			vn += lp;
		}

	WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::DrawLetterBox(WINAPP *WinApp)
{
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	int				lp;
	int				vn;
	UCHAR			col;

	
	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		lp = ddsd->lPitch;
		vn = 0;

		for(int y = 0; y <= 39; y++){
			for(int x = 0; x <= WinApp->xRes; x++){
				VideoBuffer[x+vn] = 0;
			}
			vn += lp;
		}
		
		vn = (WinApp->yRes-40)*lp;
		for(y = 0; y <= 39; y++){
			for(int x = 0; x <= WinApp->xRes; x++){
				VideoBuffer[x+vn] = 0;
			}
			vn += lp;
		}

	WinApp->lpddsback->Unlock(NULL);
}

void LD3ENGINE::PutPixel(WINAPP *WinApp, int x, int y, int col)
{
	//- Place a pixel on the surface
	//DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	//UCHAR			*VideoBuffer;

	DDBLTFX ddbltfx;
	RECT area;

	area.left	= x;
	area.right	= x+1;
	area.top	= y;
	area.bottom	= y+1;

	memset(&ddbltfx, 0, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = col;
	
	WinApp->lpddsback->Blt(&area, NULL, NULL,  DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	/*WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		VideoBuffer[x+y*ddsd->lPitch] = col;
	
	WinApp->lpddsback->Unlock(NULL);*/
}

void LD3ENGINE::DrawBox(WINAPP *WinApp, int x1, int y1, int x2, int y2, int col, bool fill)
{
	//- Draw a filled box with the given color
	DDSURFACEDESC2	*ddsd = &WinApp->ddsd;
	UCHAR			*VideoBuffer;
	long			lp, xs, ys;

	WinApp->lpddsback->Lock(NULL, ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		VideoBuffer = (UCHAR *)ddsd->lpSurface;

		if(fill){
			lp = x1+y1*ddsd->lPitch;
			xs = abs(x2-x1)+1;		
			for(int y = y1; y <= y2; y++){
				for(int x = x1; x <= x2; x++){
					VideoBuffer[lp] = col;
					lp++;
				}
				lp += ddsd->lPitch;
				lp -= xs;
			}
		}
		else{
			lp = y1*ddsd->lPitch;
			xs = abs(x2-x1)+1;
			ys = abs(y2-y1)*ddsd->lPitch;
			for(int y = y1; y <= y2; y++){
				VideoBuffer[lp+x1] = col;
				VideoBuffer[lp+x2] = col;
				lp += ddsd->lPitch;				
			}
			lp = y1*ddsd->lPitch;
			for(int x = x1+1; x <= x2-1; x++){
				VideoBuffer[lp+x] = col;
				VideoBuffer[lp+ys+x] = col;
			}
		}
	
	WinApp->lpddsback->Unlock(NULL);
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
		memset(Filename, 0, 80);
		strcpy(Filename, "screenshots/shot");

		memset(SSNo, 0, 8);
		itoa(ScreenshotNum, SSNo, 10);
		strcat(SSNo, ".bmp");
		strcat(Filename, SSNo);
		
		bfile.open(Filename, ios::nocreate);
		if(bfile.fail)
			break;
		else
			bfile.close();
			
	}

	//strcpy(Filename, "screenshots/shot.bmp");

	bfile.close();
	bfile.clear();
	bfile.open(Filename, ios::binary);
	//bfile.open(Filename, ios::binary);
	
	//ofstream bfile(Filename);
	
	UCHAR		ch;
	int			r1;
	int			r2;
	int			r3;
	int			size;
	DDSURFACEDESC2		ddsd;			// Holds the DirectDraw surface description
	
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
		ch = WinApp->palette[i].peBlue;		bfile.put(ch);
		ch = WinApp->palette[i].peGreen;	bfile.put(ch);
		ch = WinApp->palette[i].peRed;		bfile.put(ch);
		ch = NULL;	bfile.put(ch);
		fcount += 4;
	}

	// save image data
	memset((void *)&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize				= sizeof(ddsd);
	WinApp->lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	VideoBuffer = (UCHAR *)ddsd.lpSurface;
	WinApp->lpddsback->Unlock(NULL);
	for(int y = yRes-1; y >= 0; y--)
	{
		for(int x = 0; x <= xRes-1; x++)
		{
			ch = VideoBuffer[x + y*ddsd.lPitch];
			bfile.seekp(fcount);
			bfile.put(ch);
			fcount++;
		}		
	}
	
	bfile.close();	
}

void LD3ENGINE::FadeIn(WINAPP *WinApp)
{
/*	bool PaletteChange;
	long TickCount;
	PALETTEENTRY FadePalette[256];
	memset(FadePalette, 0, sizeof(PALETTEENTRY)*256);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		WinApp->lpddpal->Release();
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].peRed < WinApp->palette[n].peRed){FadePalette[n].peRed += 1; PaletteChange = true;}
			if(FadePalette[n].peGreen < WinApp->palette[n].peGreen){FadePalette[n].peGreen += 1; PaletteChange = true;}
			if(FadePalette[n].peBlue < WinApp->palette[n].peBlue){FadePalette[n].peBlue += 1; PaletteChange = true;}
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
	PALETTEENTRY FadePalette[256];
	memcpy(&FadePalette, WinApp->palette, sizeof(PALETTEENTRY)*256);

	PaletteChange = true;
	while(PaletteChange)
	{
		TickCount = GetTickCount();
	
		WinApp->lpddpal->Release();
		WinApp->lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, FadePalette, &WinApp->lpddpal, NULL);
		WinApp->lpddsprimary->SetPalette(WinApp->lpddpal);
		
		PaletteChange = false;
		for(int n = 0; n <= 255; n++){
			if(FadePalette[n].peRed > 0){FadePalette[n].peRed -= 1; PaletteChange = true;}
			if(FadePalette[n].peGreen > 0){FadePalette[n].peGreen -= 1; PaletteChange = true;}
			if(FadePalette[n].peBlue > 0){FadePalette[n].peBlue -= 1; PaletteChange = true;}
		}

		while(GetTickCount()-TickCount < 1.3f);
	}

	TickCount = GetTickCount();
	while(GetTickCount()-TickCount < 1000);*/
}

void LD3ENGINE::WaitForRetrace(WINAPP *WinApp)
{
	//WinApp->lpddsprimary->WaitForVerticalBlank(DDWAITVB_BLOBKBEGIN, NULL);
	WinApp->lpdd->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
}
