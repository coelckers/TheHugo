#include "hugo.h"



Menu::Menu()
{
	lastobj=status=-1;
	sprptr=(const uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_HUGO),"BINARY");

}


Menu::~Menu()
{
}



void Menu::Draw()
{
	HDC hDC;
	HFONT hf1=CreateFont(-8,7,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"MS SANS SERIF");
	HFONT hf2=CreateFont(-16,14,0,0,FW_EXTRABOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"MS SANS SERIF");
	HFONT hf;
	SIZE sz,sz1;
	int i;
	char  buffer[50];
	static const char * const tx[]={
	" to start the game",
	" to enter the editor",
	" to view high scores",
	" to resume a game",
	" to start a level" };
	static const uint8_t c[]={0,190,6};


	hDC = offscreen.GetDC();
	PatBlt(hDC,0,0,320,200,BLACKNESS);
	SetBkColor(hDC,0);
	SetTextColor(hDC,PALETTERGB(255,0,0));
	hf=SelectFont(hDC,hf2);
	GetTextExtentPoint(hDC,"The Hugo",8,&sz);
	TextOut(hDC,160-(sz.cx>>1),20,"The Hugo",8);
	SelectFont(hDC,hf1);
	SetTextColor(hDC,PALETTERGB(255,255,0));
	GetTextExtentPoint(hDC,"written 1987-1997 by",20,&sz1);
	TextOut(hDC,160-(sz1.cx>>1),42,"written 1987-1997 by",20);
	GetTextExtentPoint(hDC,"Christoph Oelckers",18,&sz1);
	TextOut(hDC,160-(sz1.cx>>1),62,"Christoph Oelckers",18);
	GetTextExtentPoint(hDC,"© 1987-1997 by Hugosoft",23,&sz1);
	SetTextColor(hDC,PALETTERGB(0,0,255));
	TextOut(hDC,160-(sz1.cx>>1),82,"© 1987-1997 by Hugosoft",23);

	for(i=1;i<6;i++) 
	{
		SetTextAlign(hDC,TA_UPDATECP);
		MoveToEx(hDC,75,100+14*i,NULL);
		SetTextColor(hDC,PALETTERGB(255,0,255));
		SetBkColor(hDC,0);
		TextOut(hDC,0,0,"Press ",6);
		SetBkColor(hDC,PALETTERGB(255,0,255));
		SetTextColor(hDC,0);
		TextOut(hDC,0,0,buffer,wsprintf(buffer,"  F%d  ",i));
		SetTextColor(hDC,PALETTERGB(255,0,255));
		SetBkColor(hDC,0);
		TextOut(hDC,0,0,tx[i-1],strlen(tx[i-1]));
		SetTextAlign(hDC,TA_NOUPDATECP);
	}
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	offscreen.TranslateSprite(160-(sz.cx>>1)-10,24,1,0,sprptr,0,c);
	offscreen.TranslateSprite(160+(sz.cx>>1)+10,24,0,0,sprptr,0,c);
	DeleteObject(hf1);
	DeleteObject(hf2);
}



void Menu::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	if (fDown)
	{
		switch(vk)
		{
			case VK_F1:
				PostMessage(hGameWindow,WM_COMMAND,HHC_STARTGAME,0);
				break;
			case VK_F2:
				PostMessage(hGameWindow,WM_COMMAND,HHC_STARTEDITOR,0);
				break;
			case VK_F3:
				PostMessage(hGameWindow,WM_COMMAND,HHC_HIGHSCORES,0);
				break;
			case VK_F4:
				PostMessage(hGameWindow,WM_COMMAND,HHC_STARTGAME,-2);
				break;
			case VK_F5:
				PostMessage(hGameWindow,WM_COMMAND,HHC_STARTLEVEL,0);
				break;
		}
	}
}

void Menu::OnLButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags)
{
}

void Menu::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
}

void Menu::OnMouseMove(HWND,int x,int y,UINT)
{
}
