#include "hugo.h"




Hiscore::Hiscore()
{
hientry he={0,0,"The Hugo"};

	for(int i=0;i<10;i++) hlist[i]=he;
	HANDLE h=OpenFile("Hiscores.dat");
	ReadFile(h,hlist,sizeof(hlist));
	CloseFile(h);
}


Hiscore::~Hiscore()
{
}



void Hiscore::Draw()
{
	HDC hDC;
	HFONT hf1=CreateFont(-8,8,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"MS SANS SERIF");
	HFONT hf2=CreateFont(16,8,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,0,0,0,0,"COURIER NEW");
	HFONT hf;
	int i,l;
	char  buffer[50];

	hDC = offscreen.GetDC();
	PatBlt(hDC,0,0,320,200,BLACKNESS);
	SetBkMode(hDC,TRANSPARENT);
	SetTextColor(hDC,PALETTERGB(255,128,0));
	hf=SelectFont(hDC,hf1);
	TextOut(hDC,4,4,"The ten best players:",21);
	SetTextColor(hDC,PALETTERGB(255,0,0));
	TextOut(hDC,8,180,"Press a key to return to the menu",33);
	SelectFont(hDC,hf2);
	SetTextColor(hDC,PALETTERGB(255,255,0)); 
	for(i=0;i<10;i++)
	{
		l=wsprintf(buffer,"%2d. %8d %3d %s",i+1,hlist[i].score,hlist[i].level,hlist[i].name);
		TextOut(hDC,0,33+i*14,buffer,l);
	}
	SelectFont(hDC,hf);
	offscreen.ReleaseDC(hDC);
	DeleteObject(hf1);
	DeleteObject(hf2);
}



void Hiscore::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	PostMessage(hGameWindow,WM_COMMAND,HHC_MENU,0);
}

void Hiscore::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	PostMessage(hGameWindow,WM_COMMAND,HHC_MENU,0);
}



BOOL Hiscore::CheckScore(int score,int level)
{
	return (level>hlist[9].level || (level==hlist[9].level && score>hlist[9].score));
}


void Hiscore::InsertScore(int score,int level,char * name)
{
hientry he;
int i=9;

	hlist[9].score=score;
	hlist[9].level=level;
	lstrcpy(hlist[9].name,name);
	while (i && (hlist[i].level>hlist[i-1].level || 
				(hlist[i].level==hlist[i-1].level && hlist[i].score>hlist[i-1].score)))
	{
		he=hlist[i];
		hlist[i]=hlist[i-1];
		hlist[i-1]=he;
		i--;
	}
	Save();
}

void Hiscore::Save()
{
	HANDLE h=CreateFile("Hiscores.dat");
	WriteFile(h,hlist,sizeof(hlist));
	CloseFile(h);
}