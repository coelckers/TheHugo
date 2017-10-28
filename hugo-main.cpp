#include "hugo.h"


DIBSection offscreen;
BOOL fActive=TRUE;
HWND hGameWindow;
LPDIRECTSOUND lpds;
int drawx, drawy, drawscale;
HANDLE hEvent;
LPBITMAPFILEHEADER lpbfPal;
int cdNumTracks;

Context * CurrentContext=NULL;

static charset * theCharSet;
static level * theLevel;

static Game * theGame;
static Menu * theMenu;
static Hiscore * theHighScore;
static HiscoreIn * theHighScoreIn;
static EnterLevNo * theEnterLevNo;
static EnterTime * theEnterTime;
static Editor * theEditor;
static GameOver * theGameOver;

volatile int up=0,down=0,left=0,right=0,fire=0,space=0;
int gEscalFlag=0;

#ifdef _DEBUG
#define CX 1024
#define CY 768
#else
#define CX 320
#define CY 200
#endif


void PlayCDTrack(int levno)
{
	if (cdNumTracks==0) return;
	int track=levno%cdNumTracks+2;
}

/*****************************************************/
/*****************************************************/
void SetTheObject(Context * newO)
{
	if (CurrentContext) CurrentContext->Deactivate();
	CurrentContext=newO;
	CurrentContext->Activate();
}

/*****************************************************/
/*****************************************************/
void CreateObjects()
{
	theCharSet=new charset(MAKEINTRESOURCE(IDR_CHARSET));
	theLevel=new level(theCharSet);

	theMenu=new Menu;
	theHighScore=new Hiscore;
	theHighScoreIn=new HiscoreIn(theHighScore);
	theEnterLevNo=new EnterLevNo(200); //theLevel->GetLevelCount());
	theEnterTime=new EnterTime;
	theGame=new Game(theLevel,theCharSet);
	theEditor=new Editor(theLevel,theCharSet);
	theGameOver=new GameOver;
}


/*****************************************************/
/*****************************************************/
void DeleteObjects()
{
	delete theGameOver;
	delete theEditor;
	delete theGame;
	delete theEnterTime;
	delete theEnterLevNo;
	delete theHighScoreIn;
	delete theHighScore;
	delete theMenu;

	delete theLevel;
	delete theCharSet;
}

int MX(int x)
{
	return (x - drawx) / drawscale;
}

int MY(int y)
{
	return (y - drawy) / drawscale;
}
/*****************************************************/
/*****************************************************/
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch(msg)
	{
		case WM_ACTIVATEAPP:
			fActive=wparam;
			if (wparam)
			{
				SetFocus(hGameWindow);
				ShowWindow(hGameWindow,SW_SHOWMAXIMIZED);
				ShowCursor(FALSE);
				if (CurrentContext) 
				{
					CurrentContext->Draw();
				}
			}
			else
			{
				ShowWindow(hGameWindow,SW_SHOWMINIMIZED);
				ShowCursor(TRUE);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
				case HHC_STARTGAME:
					theGame->StartGame(lparam);
					SetTheObject(theGame);
					break;

				case HHC_ENTERSCORE:
					SetTheObject(theHighScoreIn);
					break;

				case HHC_MENU:
					SetTheObject(theMenu);
					break;

				case HHC_HIGHSCORES:
					SetTheObject(theHighScore);
					break;

				case HHC_STARTLEVEL:
					theEnterLevNo->SetAction(lparam);
					SetTheObject(theEnterLevNo);
					break;

				case HHC_STARTEDITOR:
					theEditor->SetLevel(lparam);
					SetTheObject(theEditor);
					break;

				case HHC_ENTERTIME:
					SetTheObject(theEnterTime);
					break;

			}
			break;

		case HHM_GAMEOVER:
			theHighScoreIn->SetScore(wparam,lparam);
			SetTheObject(theGameOver);
			break;

		case WM_NCHITTEST:
			if (fActive) return HTCLIENT;
			else return DefWindowProc(hwnd,msg,wparam,lparam);

		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
			if (CurrentContext) HANDLE_WM_LBUTTONDOWN(hwnd,MX(wparam),MY(lparam),CurrentContext->OnLButtonDown);
			break;

		case WM_LBUTTONUP:
			if (CurrentContext) HANDLE_WM_LBUTTONUP(hwnd, MX(wparam), MY(lparam),CurrentContext->OnLButtonUp);
			break;

		case WM_RBUTTONDOWN:
			if (CurrentContext) HANDLE_WM_RBUTTONDOWN(hwnd, MX(wparam), MY(lparam),CurrentContext->OnRButtonDown);
			break;

		case WM_RBUTTONUP:
			if (CurrentContext) HANDLE_WM_RBUTTONUP(hwnd, MX(wparam), MY(lparam),CurrentContext->OnRButtonUp);
			break;

		case WM_MOUSEMOVE:
			if (CurrentContext) HANDLE_WM_MOUSEMOVE(hwnd, MX(wparam), MY(lparam),CurrentContext->OnMouseMove);
			break;

		case WM_KEYDOWN:
			if (CurrentContext) HANDLE_WM_KEYDOWN(hwnd,wparam,lparam,CurrentContext->OnKey);
			break;

		case WM_CHAR:
			if (CurrentContext) HANDLE_WM_CHAR(hwnd,wparam,lparam,CurrentContext->OnChar);
			break;

		case WM_PAINT:
		{
			RECT r;
			GetClientRect(hwnd, &r);
			drawscale = min(r.right / 320, r.bottom / 200);
			drawx = (r.right - 320 * drawscale) / 2;
			drawy = (r.bottom - 200 * drawscale) / 2;

			HDC dc = GetDC(hwnd);
			HDC dcb = offscreen.GetDC();
			StretchBlt(dc, drawx, drawy, 320*drawscale, 200*drawscale, dcb, 0, 0, 320, 200, SRCCOPY);
			offscreen.ReleaseDC(dcb);
			ReleaseDC(hwnd, dc);
			break;
		}

		default:
			return DefWindowProc(hwnd,msg,wparam,lparam);
	}
	return 0;
}


/*****************************************************/
/*****************************************************/
void CreateOffScreenSurface()
{           
	offscreen.Create(320, 200);
}                                                                             

/*****************************************************/
/*****************************************************/
int WINAPI WinMain(HINSTANCE,HINSTANCE,char * ,int)
{
	WNDCLASS wClass;
	HINSTANCE hInst=GetModuleHandle(NULL);
	MSG msg;
	__int64 pff,pfc,nextpfc;
	int c;

	//hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	lpbfPal=(LPBITMAPFILEHEADER)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_PALETTE),"BINARY");
	wClass.style=CS_VREDRAW|CS_HREDRAW|CS_BYTEALIGNCLIENT;
	wClass.lpfnWndProc=MainWndProc;
	wClass.cbClsExtra=wClass.cbWndExtra=0;
	wClass.hInstance=hInst;
	wClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wClass.hCursor=NULL;
	wClass.hbrBackground=GetStockBrush(BLACK_BRUSH);
	wClass.lpszMenuName=NULL;
	wClass.lpszClassName="HugoClass";
	RegisterClass(&wClass);
	hGameWindow=CreateWindow("HugoClass","The Hugo",WS_POPUP,0,0,500,500,NULL,NULL,hInst,NULL);
	ShowWindow(hGameWindow,SW_SHOWMAXIMIZED);
	SetFocus(hGameWindow);
	UpdateWindow(hGameWindow);
	CreateOffScreenSurface();
			
	DirectSoundCreate(NULL,&lpds,NULL);
	if (lpds) lpds->SetCooperativeLevel(hGameWindow,DSSCL_NORMAL);
	InitSounds();
	CreateObjects();
			
	SetTheObject(theMenu);
	QueryPerformanceFrequency((LARGE_INTEGER*)&pff);
	pff=(pff+28)/56;
	QueryPerformanceCounter((LARGE_INTEGER*)&nextpfc);
	nextpfc+=pff;
	while(1)
	{
		while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT) goto brk2;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_PAINT) break;
		}
		if (fActive) 
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&pfc);
			if (pfc>=nextpfc)
			{
				nextpfc+=pff;
				if (pfc>nextpfc)
				{
					c=(int)((pfc-nextpfc)/pff);
					nextpfc+=pff*c;
					c++;
					if (c>6) c=6;
				}
				else c=1;
				theCharSet->Animate(1);							
				CurrentContext->Loop(1);
				InvalidateRect(hGameWindow, NULL, false);
			}
		}
		else WaitMessage();
	}
brk2:
	DeleteSounds();
	DeleteObjects();
	ClipCursor(NULL);
	return 0;
}


const void * GetResourcePointer(HINSTANCE hInst, char *rname, char * rtype)
{
	HRSRC hRsrc;
	HGLOBAL hGlobal;

	if (!(hRsrc = FindResource(hInst, rname, rtype))) return NULL;
	hGlobal = LoadResource(hInst, hRsrc);
	return LockResource(hGlobal);
}
