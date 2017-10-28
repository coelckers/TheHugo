#include "hugo.h"


EnterLevNo::EnterLevNo(int ln)
{
	enterpos=0;
	maxlevel=ln;
}

EnterLevNo::~EnterLevNo()
{
}


void EnterLevNo::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
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


void EnterLevNo::OnChar(HWND,UINT chr,int rpt)
{
WPARAM wp=!action? HHC_STARTGAME:HHC_STARTEDITOR;

	if (chr<32)
	{
		switch(chr)
		{
			case 8:
				if (enterpos) entername[--enterpos]=0;
				break;

			case 27:
				PostMessage(hGameWindow,WM_COMMAND,wp,0);
				return;

			case 13:
				PostMessage(hGameWindow,WM_COMMAND,wp,strtol(entername,NULL, 10));
				return;
		}
				
	}
	else if (enterpos<3 && chr>='0' && chr<='9') 
	{
		entername[enterpos++]=chr;
		entername[enterpos]=0;
		if (strtol(entername,NULL, 10)>maxlevel) entername[--enterpos]=0;
	}
	Draw();
}

void EnterLevNo::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
}

void EnterLevNo::Activate()
{
	enterpos=0;
	*entername=0;
	Draw();
}

void EnterLevNo::Draw()
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
	TextOut(hDC,4,4,"Enter the number of the level",29);
	if (action==0) TextOut(hDC,4,20,"you want to start with:",23);
	else if (action==1) TextOut(hDC,4,20,"you want to load:",17);
	else
	{}
	SelectFont(hDC,hf2);
	SetTextColor(hDC,PALETTERGB(0,255,255));
	l=wsprintf(buffer,">> %s_",entername);
	TextOut(hDC,4,50,buffer,l);
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	DeleteObject(hf1);
	DeleteObject(hf2);
}
