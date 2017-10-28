#include "hugo.h"


void GameOver::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	PostMessage(hwnd,WM_COMMAND,HHC_ENTERSCORE,0);
}

void GameOver::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	PostMessage(hwnd,WM_COMMAND,HHC_ENTERSCORE,0);
}


void GameOver::Draw()
{
	HDC hDC;
	HFONT hf1=CreateFont(100,25,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"ARIAL");
	HFONT hf;
	//int l;
	//char  buffer[50];

	hDC = offscreen.GetDC();
	PatBlt(hDC,0,0,320,200,BLACKNESS);
	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,PALETTERGB(255,128,0));
	hf=SelectFont(hDC,hf1);
	TextOut(hDC,0,50,"GAME OVER",9);
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	DeleteObject(hf1);
}

