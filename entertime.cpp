#include "hugo.h"

EnterTime::EnterTime() :EnterLevNo(999)
{
	action=1;
}

EnterTime::~EnterTime()
{
}


void EnterTime::Draw()
{
	HDC hDC;
	HFONT hf1=CreateFont(-8,8,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"MS SANS SERIF");
	HFONT hf2=CreateFont(16,8,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"COURIER NEW");
	HFONT hf;
	int l;
	char  buffer[50];

	hDC = offscreen.GetDC();
	PatBlt(hDC,0,0,320,200,BLACKNESS);
	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,PALETTERGB(0,255,0));
	hf=SelectFont(hDC,hf1);
	TextOut(hDC,4,4,"Enter the time for this level:",30);
	SelectFont(hDC,hf2);
	SetTextColor(hDC,PALETTERGB(0,255,255));
	l=wsprintf(buffer,">> %s_",entername);
	TextOut(hDC,4,50,buffer,l);
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	DeleteObject(hf1);
	DeleteObject(hf2);
}
