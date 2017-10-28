#include "hugo.h"


HiscoreIn::HiscoreIn(Hiscore* hi)
{
	enterpos=-1;
	hitab=hi;
}


HiscoreIn::~HiscoreIn()
{
}


void HiscoreIn::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	switch(vk)
	{
		case VK_DELETE:
		case VK_LEFT:
			if (enterpos) entername[--enterpos]=0;
			Draw();
			break;
	}
}


void HiscoreIn::OnChar(HWND,UINT chr,int rpt)
{
	if (chr<32)
	{
		switch(chr)
		{
			case 8:
				if (enterpos) entername[--enterpos]=0;
				break;

			case 27:
				hitab->InsertScore(score,level,"The Hugo");
				PostMessage(hGameWindow,WM_COMMAND,HHC_HIGHSCORES,0);
				return;

			case 13:
				hitab->InsertScore(score,level,entername);
				PostMessage(hGameWindow,WM_COMMAND,HHC_HIGHSCORES,0);
				return;
		}
				
	}
	else if (enterpos<23) 
	{
		entername[enterpos++]=chr;
		entername[enterpos]=0;
	}
	Draw();
}

void HiscoreIn::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
}


void HiscoreIn::Activate()
{
	if (hitab->CheckScore(score,level))
	{
		enterpos=0;
		*entername=0;
		Draw();
	}
	else PostMessage(hGameWindow,WM_COMMAND,HHC_HIGHSCORES,0);
}

void HiscoreIn::Draw()
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
	SetTextColor(hDC,PALETTERGB(255,128,0));
	hf=SelectFont(hDC,hf1);
	TextOut(hDC,4,4,"Congratulations!",16);
	TextOut(hDC,4,20,"You are one of the ten best players",35);
	SelectFont(hDC,hf2);
	SetTextColor(hDC,PALETTERGB(255,255,0)); 
	l=wsprintf(buffer,">> %s_",entername);
	TextOut(hDC,4,50,buffer,l);
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	DeleteObject(hf1);
	DeleteObject(hf2);
}
