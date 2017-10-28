#include "hugo.h"

static const uint8_t Selection0[32]={64,65,66,67,68,69,141,229,228,252,253,254,255,251,30,142,158,159,165,0};
static const uint8_t Selection1[32]={29,244,246,59,164,227,132,167,176,178,180,182,183,184,235,236,237,238,0};
static const uint8_t Selection2[32]={89,90,209,225,226,166,42,144,145,147,148,154,155,156,157,247,248,249,250,168,169,170,171,172,173,174,175,33,243,0};
static const uint8_t Selection3[32]={70,71,72,73,74,75,76,78,79,77,80,81,82,185,186,187,188,189,190,191,192,193,194,195,196,197,232,149,150,151,152,0};
static const uint8_t Selection4[32]={83,84,85,86,87,88,120,121,128,129,130,131,39,133,135,136,137,220,221,224,241,35,36,230,231,60,239,47,138,139,140,0};
static const uint8_t Selection5[32]={108,109,110,111,112,113,114,115,34,162,106,107,116,117,118,119,0};
static const uint8_t Selection6[32]={91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,0};
static const uint8_t Selection7[32]={198,199,200,201,202,208,203,204,205,206,207,210,211,212,213,214,215,216,217,218,219,222,223,233,37,40,38,0};

static const uint8_t cSelection[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,25,26,27,190,192,193,194,199,203,205,206,251,252,254,
							    18,19,20,21,22,23,24,92,93,94,95,96,97,98,99,100,110,120,130,140,145,150,161,162,163,164,165,166,167,168,169,170,171}; 


static const uint8_t * const Sels[]={
	Selection0,
	Selection1,
	Selection2,
	Selection3,
	Selection4,
	Selection5,
	Selection6,
	Selection7,
	NULL};

Editor::Editor(level * lev,charset * chr)
{
	theLevel=lev;
	theCharset=chr;
	lpbfMenubar=(LPBITMAPFILEHEADER)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_EDIMENU),"BINARY");
	lpbiMenubar=(LPBITMAPINFOHEADER)(lpbfMenubar+1);
	lpbitsMenubar=(uint8_t*)lpbfMenubar+lpbfMenubar->bfOffBits;

	currentSelection=0;
	currentColor=6;
	cSelect=Selection0;


	mou_X=mou_Y=0;
	mou_P=(uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_POINTER),"BINARY");
	lpsprBall= (uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_SPRITE),"BINARY");

	levno=-1;
	changed=FALSE;
	action=0;
	extSelect=0;

}

Editor::~Editor()
{
}


void Editor::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	switch(vk)
	{
		case 'L':
			action=1;
			PostMessage(hwnd,WM_COMMAND,HHC_STARTLEVEL,1);
			break;


		case 'S':
			action=2;
			if (GetKeyState(VK_SHIFT)&0x8000) levno=-1;
			if (levno==-1) PostMessage(hwnd,WM_COMMAND,HHC_STARTLEVEL,2);
			else theLevel->Save(levno);
			break;

		case 'N':
			if (GetKeyState(VK_CONTROL)&0x8000) 
			{
				//theLevel->Save(levno);
				theLevel->Load(++levno);
				theLevel->PrepareEditor();
			}
			else theLevel->CreateEmptyLevel();
			break;

		case 'P':	// Specials
		{
			if ((GetKeyState(VK_CONTROL)&0x8000)) 
			{
				if (levno)
				{
					//theLevel->Save(levno);
					theLevel->Load(--levno);
					theLevel->PrepareEditor();
				}
			}
			else
			{
				int v=theLevel->GetSpecials()+64;
				if (v==192) v=0;
				theLevel->SetSpecial(v);
			}
			break;
		}

		case 'T':	// Zeit
			action=3;
			PostMessage(hwnd,WM_COMMAND,HHC_ENTERTIME,1);
			break;

		case VK_ESCAPE:
			action=0;
			PostMessage(hwnd,WM_COMMAND,HHC_MENU,0);

	}
}

void Editor::OnLButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags)
{

	if (y>183 && (extSelect<7 || extSelect>9))
	{
		int Select=(x-1)/13;
		if (Select<8) 
		{
			cSelect=Sels[Select];
			currentSelection=0;
			extSelect=0;
		}
		else if (Select==8) extSelect=extSelect==1? 2:1;
		else if (Select==9) extSelect=extSelect==3? 4:3;
		else if (Select==10) extSelect=5;
		else if (Select==11) OnKey(hw,VK_ESCAPE,FALSE,0,0);
	}
	else if (y>170 && y<=180)
	{
		int px=(x-1)/9;
		switch(extSelect)
		{
			case 0:
				if (px<31 && cSelect[px]) currentSelection=px;
				break;

			case 1:
			case 2:
				currentColor=cSelection[px+extSelect*32-32];
				extSelect=0;
				break;

			case 3:
			case 4:
				theLevel->SetFrameColor(cSelection[px+extSelect*32-96]); 
				extSelect=0;
				break;

			case 5:
				if (px<7)
				{
					extSelect=6;
					EditSprite=px;
				}
				break;

			case 6:
				extSelect=7;
				EditSpriteMode=px;
				sprP[2].x=sprP[1].x=sprP[0].x=160;
				sprP[2].y=sprP[1].y=sprP[0].y=100;
				break;

		}
	}
	else if (y<152 && !extSelect)
	{
		x>>=3;
		y>>=3;
		if (y==0 || x==0 || x==39) return;
		theLevel->SetCol(x+y*40,cSelect[currentSelection],currentColor);
	}
	else 
	{
		RECT r={0,0,320,200};
		switch(extSelect)
		{
			case 7:
				switch(EditSpriteMode)
				{
					case 4:
					case 0:
						SetRect(&r,-20,sprP[0].y+12,340,sprP[0].y+12);
						break;

					case 1:
						SetRect(&r,sprP[0].x+13,-20,sprP[0].x+13,340);
						break;
					case 2:
					case 3:
						break;
				}
				extSelect=8;
				break;

			case 8:
				if (EditSpriteMode<4) extSelect=0;
				else 
				{
					extSelect=9;
					SetRect(&r,sprP[0].x+13,-20,sprP[0].x+13,340);
				}
				break;

			case 9:
				extSelect=0;
				break;
		}
		ClipCursor(&r);
	}
}

void Editor::OnRButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags)
{
	if (y<152 && !extSelect)
	{
		x>>=3;
		y>>=3;
		if (y==0 || x==0 || x==39) return;
		theLevel->SetCol(x+y*40,32,currentColor);
	}
	extSelect=0;
}

void Editor::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
}

void Editor::OnMouseMove(HWND hwnd,int x,int y,UINT)
{
BOOL map=GetSystemMetrics(SM_SWAPBUTTON);

	if (y<152 && GetAsyncKeyState(map? VK_RBUTTON:VK_LBUTTON)&0x8000) OnLButtonDown(hwnd,FALSE,x,y,0);
	if (y<152 && GetAsyncKeyState(map? VK_LBUTTON:VK_RBUTTON)&0x8000) OnRButtonDown(hwnd,FALSE,x,y,0);

	switch(extSelect)
	{
		case 7:
		case 8:
		case 9:
			sprP[extSelect-7].x=x-13;
			sprP[extSelect-7].y=y-12;
	}
}


void Editor::Activate()
{
	switch(action)
	{
		case 0:
			if (levno==-1) theLevel->CreateEmptyLevel();
			//levno=-1;
			break;

		case 1:		// load level
			levno=value;
			if (theLevel->Load(levno)) theLevel->PrepareEditor();
			else 
			{
				theLevel->CreateEmptyLevel();
				levno=-1;
			}
			break;

		case 2:		// save level
			levno=value;
			theLevel->Save(levno);
			break;

		case 3:		// set time
			theLevel->SetTime(value);
			break;
	}
	Draw();
}

void Editor::Deactivate()
{
}

void Editor::Draw()
{
	DIBSection & ddd = offscreen; 
	static const uint8_t sprsyms[]={45,44,58,62,63};

	const uint8_t * cSel;
	int i;

	ddd.CopyBlock(0,160,320,40,lpbitsMenubar,lpbiMenubar,0,0);
	i=2;
	switch(extSelect)
	{
		case 0:
			cSel=cSelect;
			while (*cSel)
			{
				theCharset->DrawChar(ddd.Bits(),ddd.Width(),i,172,*cSel,currentColor);
				cSel++;
				i+=9;
			}
			ddd.SetBlock(1+currentSelection*9,171,10,1,190);
			ddd.SetBlock(1+currentSelection*9,171,1,10,190);
			ddd.SetBlock(1+currentSelection*9,180,10,1,190);
			ddd.SetBlock(10+currentSelection*9,171,1,10,190);
			break;

		case 1:
		case 2:
		case 3:
		case 4:
			cSel=cSelection+(((extSelect-1)&1)+1)*32-32;
			for(;i<2+9*32;i+=9)
			{
				ddd.SetBlock(i,172,8,8,*cSel);
				cSel++;
			}
			break;

		case 5:
			for(i=0;i<7;i++) theCharset->PutChar(ddd.Bits(),ddd.Width(),2+i*9,172,43,200+i);
			break;

		case 6:
			for(i=0;i<5;i++) theCharset->PutChar(ddd.Bits(),ddd.Width(),2+i*9,172,sprsyms[i],200+EditSprite);
			break;
			

	}
}


void Editor::Loop(int count) 
{
static const uint8_t col[]={0,2,255};
HDC hDC;
POINT p;

	theLevel->Draw(offscreen,FALSE);

	hDC = offscreen.GetDC();
	if (extSelect>=5 && extSelect<=9) 
	{
		if (extSelect>7) CreateSprite();
		theLevel->DrawSpriteTracks(hDC);
	}
	offscreen.ReleaseDC(hDC);

	Draw();


	offscreen.SetBlock(312,192,8,8,col[theLevel->GetSpecials()>>6]);
	GetCursorPos(&p);
	if (extSelect<7 || extSelect>9) offscreen.DisplaySpriteEx(p.x,p.y,0,0,0,0,319,200,mou_P,0);
	else offscreen.DisplaySpriteEx(p.x-12,p.y-10,0,0,0,0,319,200,lpsprBall,EditSprite);
}



void Editor::CreateSprite()
{
sprite spr;

	spr.present=1;
	switch(EditSpriteMode)
	{
		case 0:
			spr.direction=1;
			spr.lorand=min(sprP[0].x,sprP[1].x);
			spr.rurand=max(sprP[0].x,sprP[1].x);
			spr.ypos=sprP[0].y;
			break;

		case 1:
			spr.direction=2;
			spr.lorand=min(sprP[0].y,sprP[1].y);
			spr.rurand=max(sprP[0].y,sprP[1].y);
			spr.xpos=sprP[0].x;
			break;

		case 2:
		case 3:
			spr.direction=1+EditSpriteMode;
			spr.sx=sprP[0].x;
			spr.sy=sprP[0].y;
			spr.ex=sprP[1].x;
			spr.ey=sprP[1].y;
			break;

		case 4:
			spr.direction=6+(sprP[2].y>sprP[0].y);
			spr.mx=sprP[0].x;
			spr.my=sprP[0].y;
			spr.radx=abs(sprP[1].x-sprP[0].x);
			spr.rady=extSelect==8? 10: abs(sprP[2].y-sprP[0].y);
			break;
	}
	theLevel->SetSprite(EditSprite,&spr);
}

