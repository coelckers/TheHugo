#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dsound.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

const void *GetResourcePointer(HINSTANCE hInst, char *rname, char * rtype);

inline HANDLE CreateFile(LPCSTR filename)
{
	return CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

inline int WriteFile(HANDLE h, LPCVOID buffer, int len)
{
	DWORD dwRead;

	WriteFile(h, buffer, len, &dwRead, NULL);
	return dwRead;
}

inline void CloseFile(HANDLE h) { CloseHandle(h); }

inline HANDLE OpenFile(LPCSTR filename, DWORD access = GENERIC_READ, DWORD share = FILE_SHARE_READ)
{
	return CreateFile(filename, access, share, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

inline int ReadFile(HANDLE h, LPVOID buffer, int len)
{
	DWORD dwRead;

	ReadFile(h, buffer, len, &dwRead, NULL);
	return dwRead;
}

inline DWORD SeekFile(HANDLE h, int pos, int from)
{
	return SetFilePointer(h, pos, NULL, from);
}

inline BOOL Line(HDC hDC, int x1, int y1, int x2, int y2)
{
	POINT pt[2];

	pt[0].x = x1;
	pt[0].y = y1;
	pt[1].x = x2;
	pt[1].y = y2;
	return Polyline(hDC, pt, 2);
}

inline BOOL Line(HDC hDC, POINT p1, POINT p2)
{
	POINT pt[2];

	pt[0] = p1;
	pt[1] = p2;
	return Polyline(hDC, pt, 2);
}



#pragma warning(disable:4305)
#pragma warning(disable:4309)

extern BOOL fActive;
extern HWND hGameWindow;
extern LPDIRECTSOUND lpds;
/*
extern LPDIRECTDRAW lpdd;
extern LPDIRECTDRAWSURFACE lpdds, lpddsbb;
extern LPDIRECTDRAWPALETTE lpddp, lpddpb;
*/
extern LPBITMAPFILEHEADER lpbfPal;

#include "sprites.h"
#include "sound.h"

#include "resource.h"
#include "dibsect.h"
#include "charset.h"
#include "level.h"

#include "context.h"
#include "menu.h"
#include "hiscore.h"
#include "enterlevno.h"
#include "game.h"
#include "editor.h"
#include "gameover.h"


extern DIBSection offscreen;

extern Context * CurrentContext;


#define HHM_GAMEOVER WM_USER+0x100

#define HHC_STARTGAME 10000
#define HHC_STARTEDITOR 10001
#define HHC_HIGHSCORES 10002
#define HHC_RANDOMGAME 10003
#define HHC_STARTLEVEL 10004
#define HHC_MENU 10005
#define HHC_ENTERSCORE 10006
#define HHC_ENTERTIME 10007

