#include "hugo.h"

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
Game::Game(level * pLevel,charset * pCS) : CollChk(12,14)
{
	levno=lives=score=0;
	theLevel=pLevel;
	theCharSet=pCS;
	lpsprHugo=(const uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_HUGO),"BINARY");
	lpsprText= (const uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_TEXT),"BINARY");
	//lpsprBall=GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_SPRITE),"BINARY");
	lpsprBlend= (const uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_BLEND),"BINARY");
}

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
Game::~Game()
{
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::ReadControls()
{
	if (((GetAsyncKeyState(VK_SPACE)&0x8000)) && bombbs &&  theLevel->SB(bspos)==32)
	{
		if (theLevel->SetBomb(bspos)) bombsused++,bombbs--;
	}
	if ((GetAsyncKeyState(VK_CONTROL)&0x8000))
	{
		feuer=1;
		leftflag=rightflag=upflag=downflag=0;
	}
	else if ((GetAsyncKeyState(VK_LEFT)&0x8000))
	{
		rightflag=upflag=downflag=0;
	}
	else if ((GetAsyncKeyState(VK_RIGHT)&0x8000))
	{
		leftflag=upflag=downflag=0;
	}
	else if ((GetAsyncKeyState(VK_UP)&0x8000))
	{
		leftflag=rightflag=downflag=0;
	}
	else if ((GetAsyncKeyState(VK_DOWN)&0x8000))
	{
		leftflag=rightflag=upflag=0;
	}
	else leftflag=rightflag=upflag=downflag=0;
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
BOOL Game::Left()
{
	return !!(GetAsyncKeyState(VK_LEFT)&0x8000);
}

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
BOOL Game::Right()
{
	return !!(GetAsyncKeyState(VK_RIGHT)&0x8000);
}
////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::StartGame(int ll)
{

//	saveflag=esc=0;
	levno=ll;
	lives=8;
	score=0;
	if (ll==-2)
	{
		startat0=TRUE;
		levno=0;
		HANDLE l=OpenFile("HUGO.SAV");
		if (l!=INVALID_HANDLE_VALUE)
		{
			ReadFile(l,&score,4);
			ReadFile(l,&levno,4);
			ReadFile(l,&lives,4);
			CloseFile(l);
		}
	}
	else startat0=levno==0;
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
BOOL Game::StartLevel()
{
int lbak=0;
int l;

	l=theLevel->Load(levno);
	if (!l)
	{
		score+=1000*lives;
		if (!startat0) score=levno=0;
		PostMessage(hGameWindow,HHM_GAMEOVER,score,levno);
		DeleteFile("HUGO.SAV");
		return FALSE;
	}
	theLevel->PrepareGame();
	theCharSet->ResetMFlash();
	GameObjectStatus=GOS_WAITING;
	scormult=1;
	bombbs=bombsused=exm=hsl=torschl=tank=telep=0;
	time=theLevel->GetTime();
	timer=70;
	return TRUE;
}

void Game::Dead()
{
	dead = 0;
	GameObjectStatus = GOS_DEAD;
	PlaySound(SND_DEAD, false);
}

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::StatusField(BOOL create)
{
static short xpos[]={224,232,240,72,80,72,80,72,80,88,224,232,240,248,256,264,272};
static uint8_t ypos[]={192,192,192,162,162,177,177,192,192,192,162,162,162,162,162,162,162};
int i;
DIBSection &ddd = offscreen;
char stbuf[300];


	//if (create)
	{
		ddd.SetBlock(0,160,320,40,0);
		ddd.MaskSprite(24,162,0,0,lpsprText,1,6);
		ddd.MaskSprite(24,177,0,0,lpsprText,2,6);
		ddd.MaskSprite(24,192,0,0,lpsprText,4,6);
		ddd.MaskSprite(176,162,0,0,lpsprText,0,6);
		ddd.MaskSprite(184,192,0,0,lpsprText,3,6);
	}
	wsprintf(stbuf, "%03d%02i%02i%03i%07d",time,lives,bombbs,levno,score);
	int c=time? 6:2;
	for(i=0;i<3;i++) theCharSet->PutChar(ddd.Bits(),ddd.Width(),xpos[i],ypos[i],stbuf[i],c);
	for(;i<17;i++) theCharSet->PutChar(ddd.Bits(),ddd.Width(),xpos[i],ypos[i],stbuf[i],6);
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::EndLevel(BOOL saveflag)
{
	levno++;
	if (saveflag)
	{
		HANDLE l=CreateFile("HUGO.SAV");
		WriteFile(l,&score,4);
		WriteFile(l,&levno,4);
		WriteFile(l,&lives,4);
		CloseFile(l);
	}
}




////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::Activate()
{
	StartLevel();
	//Draw();
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags)
{
	if (GameObjectStatus==GOS_WAITING)
	{
		// level neu starten
		if (vk==VK_CONTROL || vk==VK_SHIFT)
		{
			if (vk==VK_SHIFT) theLevel->Rebuild();
			Home();
			FadeIn(GOS_PLAYING);
			tank=telep=0;
			theLevel->ClearHints();
			theLevel->KillBomb();
			theLevel->RstMin();
			push=20;
			GetAsyncKeyState(VK_F2);
		}
		else if (vk==VK_ESCAPE) PostMessage(hGameWindow,WM_COMMAND,HHC_MENU,0);

	}
	else if (GameObjectStatus==GOS_FINISHED)
	{
	}
	else
	{
	}
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::Draw()
{
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::Loop(int count)
{
int j;

	if (GameObjectStatus==GOS_WAITING)
	{
		hugodir=-1;
	}
	else if (GameObjectStatus==GOS_FINISHED)
	{
		EndLevel(GetAsyncKeyState(VK_F2));
		StartLevel();
	}
	else if (GameObjectStatus==GOS_DEAD)
	{
	static uint8_t blendtab[]={15,14,13,12,11,10,9,8,7,8,9,10,11,12,11,10,
							9,8,7,6,5,4,3,4,5,6,7,8,7,6,5,4,3,2,1,0};

		hugodir=hugodir%3+3;
		if (dead<36) blend=blendtab[dead++];
		else if (--lives)
		{
			GameObjectStatus=GOS_WAITING;
			hugodir=-1;
		}
		else 
		{
			if (!startat0) score=levno=0;
			PostMessage(hGameWindow,HHM_GAMEOVER,score,levno);
		}
	}
	else
	{
		theLevel->Animate(1);
		if (!--timer)
		{
			if (time) time--;
			timer=70;
		}
		switch(GameObjectStatus)
		{
			case GOS_FADEIN:
				if (blend) hugodir=hugodir%3+3;
				else hugodir=hugodir%3+303;
				if (++blend==12) 
				{
					GameObjectStatus=NextGOS;
					hugodir%=3;
				}
				break;

			case GOS_FADEOUT:
				hugodir=hugodir%3+3;
				if (--blend<0) 
				{
					GameObjectStatus=NextGOS;
					hugodir%=3;
				}
				break;

			case GOS_ENTERPIPEUP:
				hugodir=hugodir%3+6;
				if (++blend<16) bsp_y--;
				else TrackPipe(trackpipe);
				break;
					
			case GOS_ENTERPIPEDOWN:
				hugodir=hugodir%3+6;
				if (++blend<16) bsp_y++;
				else TrackPipe(trackpipe);
				break;

			case GOS_EXITPIPEUP:
				hugodir=hugodir%3+6;
				if (++blend<17) bsp_y--;
				else 
				{
					hugodir%=3;
					GameObjectStatus=GOS_PLAYING;	// Jump
					Move(0,-1,7);
				}
				break;
					
			case GOS_EXITPIPEDOWN:
				hugodir=hugodir%3+6;
				if (++blend<17) bsp_y++;
				else 
				{
					hugodir%=3;
					GameObjectStatus=GOS_PLAYING;	// MvDown
					if (ChkUnJp()) Move(0,1,0);
				}
				break;

			case GOS_TELEPORT:
				if (telepos==-1) Home();
				else if (telepos>200)
				{
					j=bspos+41;
					while (theLevel->SB(j)!=telepos)
					{
						j++;
						if (j==0x2ff) j=81;
					}
					j-=40;
					bspos=j;
					bsp_y=8*(j/40);
					bsp_x=8*(j%40);
				}
				else
				{
					j=bspos;
					bspos=robobs;
					robobs=j;
					j=bsp_x;
					bsp_x=rob_x;
					rob_x=j;
					j=bsp_y;
					bsp_y=rob_y;
					rob_y=j;
					if (robi2=~robi2) hugocol[2]=2;
					else hugocol[2]=6;
				}
				FadeIn(GOS_PLAYING);
				break;

			case GOS_OPENDOOR:
				OpenDoor();			
				break;

			/*
			case GOS_KEIL:

			theLevel->SB(bspos+80]=130;
			theLevel->SB(bspos+80+0x300]=j=theLevel->SB(bspos+40+0x300];
			for(i=0;i<8;i++)
			{
				bsp_y++;
				put1char(130,bsp_x,bsp_y+8,j,FP_SEG(gt));
				hline(bsp_x,bsp_y+7,8,0,FP_SEG(gt));
				putback_hot(bsp_x,bsp_y+7,FP_SEG(gt),FP_SEG(vid));
				putback_hot(bsp_x,bsp_y+8,FP_SEG(gt),FP_SEG(vid));
				copyquick(bsp_x,bsp_y+8,FP_SEG(vid),0xa000);
				intloop();
			}
			bspos+=40;
			theLevel->SB(bspos)=32;
			return 0;

			*/


			case GOS_PLAYING:
				if (!isMoving) 
				{
					Check();
					if (GameObjectStatus!=GOS_PLAYING) break;
				}
				if (isMoving)
				{
					bsp_x+=mv_x;
					bsp_y+=mv_y;
					isMoving--;
					if (dofahr)
					{
						if (mv_y<0)
						{
							if (isMoving==7)
							{
								theLevel->Set(bspos+40,60);
								theLevel->Set(bspos,27);
							}
							else if (isMoving) theLevel->Set(bspos,20+isMoving);
							else theLevel->Set(bspos,239);
						}
						else
						{
							if (isMoving) theLevel->Set(bspos+40,28-isMoving);
							else
							{
								theLevel->Set(bspos+40,60);
								theLevel->Set(bspos+80,239);
							}
						}
					}
					if (!isMoving) 
					{
						bspos+=mv_x+40*mv_y;
						switch(plMove)
						{
							case 1:		// senk. oder diag. Förderband
								fall=0;
								Check(8);
								break;
							case 2:		// waag Förderband
								fall=0;
								Check(11);
								break;
							case 3:		// Fall
								fall++;
								if (fall<=3)	
								{
									Check(3);
									if (GameObjectStatus!=GOS_PLAYING) break;
								}
								if (ChkUnJp()) Move(0,1,3);
								else if (fall>3) Dead(); 
								break;
							case 4:		// Transporter hoch+runter
								fall=0;
								if (ChkMid()==255)
								{
									FadeOut(GOS_FINISHED);
									break;
								}
								if (bspos < 40) {} //GOS_DEAD
								if (ChkUnJp()) Move(0,mv_y,4);
								else
								{
									telep=tank=0;
									hugocol[1]=190;
									Check();
								}
								break;
							case 5:	// Transporter links
								fall=0;
								if (ChkMid()==255)
								{
									FadeOut(GOS_FINISHED);
									break;
								}
								if (!ChkRight() && bsp_x>8) Move(-1,0,5);
								else
								{
									telep=tank=0;
									hugocol[1]=190;
									Check();
								}
								break;
							case 6:	// Transporter rechts
								fall=0;
								if (ChkMid()==255)
								{
									FadeOut(GOS_FINISHED);
									break;
								}
								if (!ChkLeft() && bsp_x<304) Move(1,0,6);
								else
								{
									telep=tank=0;
									hugocol[1]=190;
									Check();
								}
								break;
							case 7:	// Jump
								Check(3);
								if (GameObjectStatus!=GOS_PLAYING || isMoving==8) break;
								if (!ChkUnJp()) break;
								JumpCount=0;
								if (!ChkRight() && Right())
									hugodir=1,Move(1,0,8);
								else if (!ChkLeft() && Left())
									hugodir=0,Move(-1,0,8);
								else Move(0,1,0);
								break;
							case 8:	// Jump contd
								Check(3);
								if (GameObjectStatus!=GOS_PLAYING) break;
								if (!ChkUnJp()) break;
								if (++JumpCount==4) Move(0,1,0);
								else if ((mv_x<0 && !ChkLeft()) || (mv_x>0 && !ChkRight()))
									Move(mv_x,0,8);
								break;
							case 9:	// Sprungstein
								fall=0;
								Check(3);
								if (GameObjectStatus!=GOS_PLAYING) break;
								Move(0,-1,7);
								break;
						}
					}
				}

		}									
	}
	if (GameObjectStatus!=GOS_WAITING && theLevel->BoulderDash(bspos))
	{
		//theLevel->SetBDExplosion(bspos,FALSE);
		if (GameObjectStatus!=GOS_DEAD) Dead();
	}
	DrawLevel();
	if (!telep && GameObjectStatus!=GOS_TELEPORT && GameObjectStatus!=GOS_FINISHED && CollisionCheck()) 
		Dead();
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::DrawLevel()
{
	theLevel->Draw(offscreen);
	if (GameObjectStatus!=GOS_TELEPORT && GameObjectStatus!=GOS_FINISHED) PutHugo(offscreen);
	StatusField(FALSE);
}



////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::OpenDoor()
{
	if (opendoor<0)
	{
		switch (opendoor--)
		{
			case -1:
				theLevel->Set(bspos-1,0);
				theCharSet->SetDoorAni(0);
				break;

			case -9:
				theLevel->Set(bspos-1,32);
				if (theLevel->SB(bspos-41)!=47) 
				{
					GameObjectStatus=GOS_PLAYING;
					break;
				}
				theLevel->Set(bspos-41,0);
				theCharSet->SetDoorAni(0);
				break;

			case -17:
				theLevel->Set(bspos-41,32);
				if (theLevel->SB(bspos-81)!=47) 
				{
					GameObjectStatus=GOS_PLAYING;
					break;
				}
				theLevel->Set(bspos-81,0);
				theCharSet->SetDoorAni(0);
				break;

			case -25:
				theLevel->Set(bspos-81,32);
				GameObjectStatus=GOS_PLAYING;
				break;

			default:
				theCharSet->SetDoorAni((-opendoor-2)&7);
		}
	}
	else
	{
		switch (opendoor++)
		{
			case 1:
				theLevel->Set(bspos+1,0);
				theCharSet->SetDoorAni(0);
				break;

			case 9:
				theLevel->Set(bspos+1,32);
				if (theLevel->SB(bspos-39)!=47) 
				{
					GameObjectStatus=GOS_PLAYING;
					break;
				}
				theLevel->Set(bspos-39,0);
				theCharSet->SetDoorAni(0);
				break;

			case 17:
				theLevel->Set(bspos-39,32);
				if (theLevel->SB(bspos-79)!=47) 
				{
					GameObjectStatus=GOS_PLAYING;
					break;
				}
				theLevel->Set(bspos-79,0);
				theCharSet->SetDoorAni(0);
				break;

			case 25:
				theLevel->Set(bspos-79,32);
				GameObjectStatus=GOS_PLAYING;
				break;

			default:
				theCharSet->SetDoorAni((opendoor-2)&7);
		}
	}
}
////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::PutHugo(DIB & ddd)
{
	switch(hugodir)
	{
		case 0: // links
			ddd.TranslateSpriteEx(bsp_x+8,bsp_y-6,1,0,0,0,320,160,lpsprHugo,0,hugocol);
			break;

		case 1: // rechts
			ddd.TranslateSpriteEx(bsp_x-1,bsp_y-6,0,0,0,0,320,160,lpsprHugo,0,hugocol);
			break;

		case 2: // hoch-runter
			ddd.TranslateSpriteEx(bsp_x-2,bsp_y-6,0,0,0,0,320,160,lpsprHugo,1,hugocol);
			break;

		case 3: // links ausblend
			ddd.TranslateSpriteEx(bsp_x+8,bsp_y-6,1,0,0,0,320,160,lpsprBlend,blend,hugocol);
			break;

		case 4: // rechts ausblend
			ddd.TranslateSpriteEx(bsp_x-1,bsp_y-6,0,0,0,0,320,160,lpsprBlend,blend,hugocol);
			break;

		case 5: // h-r ausblend
			ddd.TranslateSpriteEx(bsp_x-2,bsp_y-6,0,0,0,0,320,160,lpsprBlend,blend+12,hugocol);
			break;

		case 6: // links -Pipeline
			ddd.TranslateSpriteEx(bsp_x+8,bsp_y-6,1,0,0,pipe_y,320,pipe_y+15,lpsprHugo,0,hugocol);
			break;

		case 7: // rechts -Pipeline
			ddd.TranslateSpriteEx(bsp_x-1,bsp_y-6,0,0,0,pipe_y,320,pipe_y+15,lpsprHugo,0,hugocol);
			break;

		case 8: // h-r -Pipeline
			ddd.TranslateSpriteEx(bsp_x-2,bsp_y-6,0,0,0,pipe_y,320,pipe_y+15,lpsprHugo,1,hugocol);
			break;
	}
}

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
BOOL Game::CollisionCheck()
{
static const uint8_t mask[]={0,1,0};
auto lpBits=CollChk.Bits();
int count1=0,count2=0,i;

	// y: bsp_y-6
	// x: bsp_x-2
	CollChk.Clear();
	switch(hugodir)
	{
		case 0: // links
			CollChk.TranslateSprite(10,0,1,0,lpsprHugo,0,mask);
			break;

		case 1: // rechts
			CollChk.TranslateSprite(1,0,0,0,lpsprHugo,0,mask);
			break;

		case 2: // hoch-runter
			CollChk.TranslateSprite(0,0,0,0,lpsprHugo,1,mask);
			break;

		default:
			return FALSE;
	}
	for(i=0;i<12*14;i++) count1+=lpBits[i];
	theLevel->DrawCollisionSprites(CollChk,bsp_x-2,bsp_y-6);
	for(i=0;i<12*14;i++) count2+=lpBits[i];
	return count1!=count2;
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::Home()
{

	bspos=721;
	bsp_x=8;
	rob_y=
	bsp_y=144;
	robobs=758;
	rob_x=304;
	robi2=hugodir=0;
	isMoving=FALSE;
	
	hugocol[1]=190;
	hugocol[2]=6;

	theLevel->RstFahr();
	theLevel->SpritesOn();
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::TrackPipe(int dir)
{
int i;

	do
	{
		bspos+=dir;
		i=theLevel->SB(bspos);

		switch(dir)
		{
			case 1:
				switch(i)
				{
					case 34: case 115:
						dir=1;
						break;

					case 112:
						dir=-40;
						break;

					case 113: case 162:
						dir=40;
						break;

					default:
						dir=-1;
				}
				break;

			case -1:
				switch(i)
				{
					case 34: case 115:
						dir=-1;
						break;

					case 111:
						dir=-40;
						break;

					case 114: case 162:
						dir=40;
						break;

					default:
						dir=1;
				}
				break;

			case 40:
				switch(i)
				{
					case 34: case 110:
						dir=40;
						break;

					case 111:
						dir=1;
						break;

					case 112:
						dir=-1;
						break;

					case 109:

						bsp_y=8*(bspos/40);
						bsp_x=8*(bspos%40);
						bspos+=80;
						ExitPipeDown();
						return;

					default:
						dir=-40;
				}
				break;

			case -40:
				switch(i)
				{
					case 34: case 110:
						dir=-40;
						break;

					case 114: case 162:
						dir=1;
						break;

					case 113:
						dir=-1;
						break;

					case 108:

						bsp_y=8*(bspos/40)+8;
						bsp_x=8*(bspos%40);
						bspos-=40;
						ExitPipeUp();
						return;

					default:
						dir=40;
				}
				break;
		}
	}
	while(1);
}


////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
void Game::Check(int checkmask)
{

	//if (chkkoll()) bruch();
	upflag=downflag=0;
	if (checkmask&1) switch(ChkOb())
	{
		case 1:
			Dead();
			break;
		case 2:
			EnterPipeUp();
			break;

	}

	if (checkmask&2) switch(ChkMid())
	{
		case 1:
			Dead();
			break;
		case 255:
			FadeOut(GOS_FINISHED);
			return;

		case -1: return;
	}

	if (checkmask&4) switch(ChkUn())
	{

		case 6: fall=2;
		case 1:
			fall++;
			Move(0,1,3);
			return;

		case 2:
			Dead();
			break;
		case 3:
			fall=0;
			EnterPipeDown();
			break;

		case 4:
		case 0:
			fall=0;
			break;

		case -1:
			return;

	}

	if (checkmask<8) return;

	leftflag=!ChkLeft();
	rightflag=!ChkRight();

	feuer=0;
	if (GetAsyncKeyState(VK_ESCAPE)&0x8000)
	{
		Dead();
		//PostMessage(hGameWindow,WM_COMMAND,HHC_MENU,0);
		return;
	}
	ReadControls();
	dofahr=0;
	if (feuer)
	{
		Move(0,-1,7);
	}
	else if (leftflag)
	{
		hugodir=0;
		Move(-1,0);
	}
	else if (rightflag)
	{
		hugodir=1;
		Move(1,0);
	}
	else if (upflag)
	{
		if (theLevel->SB(bspos+40)!=239) hugodir=2;
		else dofahr=1;
		Move(0,-1);
	}
	else if (downflag)
	{
		if (theLevel->SB(bspos+40)!=239) hugodir=2;
		else dofahr=1;
		Move(0,1);
	}
}


void Game::clrpos() 
{ 
	theLevel->Clear(bspos); 
	tank=0; 
	hugocol[1]=190; 
	PlaySound(SND_PING,FALSE);
}	

////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
int Game::ChkOb()
{
/* returns:

	0: ok
	1: kaputt
	2: Pipeline nach oben
*/

int i;
	if (theLevel->SB(bspos-80)==109)
	{
		return 2;
	}
	i=theLevel->SB(bspos-40);
	switch(i)
	{
		case 144: case 145: case 156: case 157:
			// flash 1a
			return theCharSet->Compare(148,32);

		case 148: case 147: case 154: case 155:
			// flash1b
			return theCharSet->Compare(144,32);

		case 166: case 168: case 172:
			return 1;

		case 171:
			return theCharSet->GetEStatus()>18 && theCharSet->GetEStatus()<29;

		case 170:
			return theCharSet->GetEStatus()>10 && theCharSet->GetEStatus()<37;

		case 169:
			return theCharSet->GetEStatus()>2 && theCharSet->GetEStatus()<47;

		case 175:
			return theCharSet->GetEStatus()<4 || theCharSet->GetEStatus()>47;

		case 174:
			return theCharSet->GetEStatus()<12 || theCharSet->GetEStatus()>35;

		case 173:
			return theCharSet->GetEStatus()<20 || theCharSet->GetEStatus()>27;

		case 247: case 249:
			// flash2a
			return theCharSet->Compare(248,32);

		case 248: case 250:
			// flash1b
			return theCharSet->Compare(247,32);

	}
	return ((i && i<=16) || i==89 || i==90);
}



////////////////////////////////////////////////////////////////////7
////////////////////////////////////////////////////////////////////7
int Game::ChkMid()
{
/* Returnwerte:

	0: ok
	1: Kaputt
	255: geschafft
	-1: Check beenden
*/
	
	theLevel->DoBlack(bspos);
	switch(theLevel->SB(bspos))
	{
		case 1:  case 2:  case 3:  case 4:
		case 5:  case 6:  case 7:  case 8:
		case 9:  case 10: case 11: case 12:
		case 13: case 14: case 15: case 16:

		case 27: //?
		case 28: //?

		case 89: case 90:

		case 134: //?

		case 166:

		case 168:

		case 172:

		case 177: //?
		case 179: //?
		case 181: //?
		case 209:
		case 234: //?
		case 240: //?

		case 225: case 226:
			// bruch
			return 1;


		case 144: case 145: case 156: case 157:
			// flash 1a
			return theCharSet->Compare(148,32);

		case 148: case 147: case 154: case 155:
			// flash1b
			return theCharSet->Compare(144,32);

		case 247: case 249:
			// flash2a
			return theCharSet->Compare(248,32);

		case 248: case 250:
			// flash1b
			return theCharSet->Compare(247,32);

		case 171:
			return theCharSet->GetEStatus()>20 && theCharSet->GetEStatus()<27;

		case 170:
			return theCharSet->GetEStatus()>12 && theCharSet->GetEStatus()<35;

		case 169:
			return theCharSet->GetEStatus()>4 && theCharSet->GetEStatus()<47;

		case 175:
			return theCharSet->GetEStatus()<4 || theCharSet->GetEStatus()>47;

		case 174:
			return theCharSet->GetEStatus()<12 || theCharSet->GetEStatus()>35;

		case 173:
			return theCharSet->GetEStatus()<20 || theCharSet->GetEStatus()>27;



		case 29:	// Brücke
			if (moveflag) return 0;
			PlaySound(SND_PING,FALSE);
			hugocol[1]=190;
			tank=0;
			moveflag=1;
			theLevel->Klapp();
			return 0;

		case 30:
			// getBomb
			theLevel->AddBombs(-1);
			bombbs++;
			AddScore(200);
			clrpos();
			return 0;

		case 59:
			// Exwandsl
			if (!torschl)
			{
				torschl=1;
				AddScore(200);
				clrpos();
				return 0;
			}
			return 0;

		case 64:
			// Schloss
			if (hsl)
			{
				//playvoice(warp);
				hsl=0;
				AddScore(100);
				score+=time*10;
				return 255;
			}
			return 0;


		case 65:
			if (theLevel->Completed())
			{
				hsl=1;
				AddScore(200);
				clrpos();
			}
			return 0;

		case 67:
		case 141:
			// Schatz
			theLevel->AddItems(-1);
			AddScore(300);
			theLevel->DoBlink();
			clrpos();
			return 0;
		case 66:
		case 68:
			theLevel->AddItems(-1);
			AddScore(100);
			theLevel->DoBlink();
			clrpos();
			return 0;
		case 69:
			theLevel->AddItems(-1);
			AddScore(200);
			theLevel->DoBlink();
			clrpos();
			return 0;

		case 83:
			// Leiter
			upflag=1;
			return 0;

		case 86:
			// fhoch (förderband)
			if (telep) return 0;
			hugodir=2;
			Move(0,-1,1);
			return -1;

		case 132:
			// Exmauer
			if (exm) return 0;
			PlaySound(SND_FORCE,FALSE);
			exm=1;
			AddScore(500);
			theLevel->ExWall();
			return 0;

		case 140:	// Erde
			theLevel->Clear(bspos); 
			return 0;
			
		case 142:
			lives++;
			AddScore(300);
			clrpos();
			return 0;

		case 158:
			time+=10;
			AddScore(100);
			clrpos();
			return 0;

		case 159:
			time+=20;
			AddScore(200);
			clrpos();
			return 0;

		case 163:
			if (!theLevel->SetSet2()) return 0;
			AddScore(200);
			clrpos();
			return 0;

		case 164:
			theCharSet->ResetMFlash();
			bombbs+=bombsused;
			bombsused=0; 
			exm=0;
			theLevel->Rebuild();

			AddScore(200);
			clrpos();
			telepos=-1;
			FadeOut(GOS_TELEPORT);
			return -1;

		case 165:
			// score*2
			scormult=2;
			AddScore(100);
			clrpos();
			return 0;


		case 167:
			if (!theLevel->IsSpriteOff(0))
			{
				theLevel->SpriteOff(0);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 176:
			if (!theLevel->IsSpriteOff(1))
			{
				theLevel->SpriteOff(1);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 178:
			if (!theLevel->IsSpriteOff(2))
			{
				theLevel->SpriteOff(2);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 180:
			if (!theLevel->IsSpriteOff(3))
			{
				theLevel->SpriteOff(3);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 182:
			if (!theLevel->IsSpriteOff(4))
			{
				theLevel->SpriteOff(4);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 183:
			if (!theLevel->IsSpriteOff(5))
			{
				theLevel->SpriteOff(5);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 184:
			if (!theLevel->IsSpriteOff(6))
			{
				theLevel->SpriteOff(6);
				tank=0;
				hugocol[1]=190;
				PlaySound(SND_PING,FALSE);
			}
			return 0;

		case 228:

			if (!tank && !telep)
			{
				tank=1;
				hugocol[1]=14;
				AddScore(10);
				PlaySound(SND_PING,FALSE);
				return 0;
			}
			return 0;

		case 229:
			if (!tank) return 1;
			theLevel->AddFack(-1);
			tank=0;
			hugocol[1]=190;
			AddScore(300);
			theLevel->DoBlink();
			clrpos();
			return 0;

		case 227:
			if (telep) return 0;
			clrpos();
			telepos=1;
			AddScore(100);
			FadeOut(GOS_TELEPORT);
			return -1;

		case 244:

			if (moveflag) return 0;
			theLevel->RstFahr();
			moveflag=1;
			PlaySound(SND_PING,FALSE);
			hugocol[1]=190;
			tank=0;
			return 0;

		case 246:
			if (moveflag) return 0;
			theCharSet->SetSwitch();
			moveflag=1;
			PlaySound(SND_PING,FALSE);
			hugocol[1]=190;
			tank=0;
			return 0;
	}
	return 0;
}


int Game::ChkUn()
{
int i,j;
/* Returnwerte:

	0: Hugo steht
	1: Hugo fällt
	2: kaputt
	3: Pipeline nach unten
	4: Fahrstuhl
	5: Pipeline nach oben	; entfällt
	6: Absturz
	255: geschafft	; entfällt
*/

	if (bspos>=720) return 0;
	switch(i=theLevel->SB(bspos+40))
	{

		case 61: // Minenexplosion-ok
			return 2;

		case 83:  // Leiter-ok
		case 140: // Erde
		case 141: // Diamant
			downflag=1;
			return 0;

		case 84:  // -ok
			if (ChkLeft()) return 0;
			hugodir=0;
			Move(-1,0,2);
			return -1;

		case 85: //-ok
			if (ChkRight()) return 0;
			hugodir=1;
			Move(1,0,2);
			return -1;

		case 87:  //-ok
			hugodir=2;
			Move(0,1,1);
			return -1;

		case 88: //-ok
			fall=0;
			return theCharSet->Compare(88,275);

		case 106:
			if (ChkLeft()) return 0;
			hugodir=0;
			Move(-1,1,1);
			return -1;

		case 107: // Rutsche
			if (ChkRight()) return 0;
			hugodir=1;
			Move(1,1,1);
			return -1;

		case 108:
			return 3;

		case 117:
			hugodir=1;
			Move(1,-1,1);
			return -1;

		case 119:
			hugodir=0;
			Move(-1,-1,1);
			return -1;

		case 120: //-ok
			fall=0;
			return theCharSet->Compare(120,275);


		case 121:  //-ok
		case 122:
		case 123:
		case 124:
		case 125:
		case 126:
			if (moveflag && --exverz) return 0;
			moveflag=1;
			exverz=5;
			theLevel->Set(bspos+40,theLevel->SB(bspos+40)+1);
			return 0;

		case 127:  //-ok
			if (moveflag && --exverz) return 0;
			moveflag=1;
			exverz=5;
			theLevel->Set(bspos+40,32);
			return 0;

		case 128:
			Move(0,-1,9);
			return -1;

		case 129: // Home  -ok
			PlaySound(SND_WARP,FALSE);
			telepos=-1;
			FadeOut(GOS_TELEPORT);
			return -1;
		
		case 130: // Keil

			if (bspos>0x300-8-40) return 0;
			j=theLevel->SB(bspos+80);
			if (bspos<680 && ((j>=64 && j<70) || j==224 || j==229 || j<17)) return 0;

			//GameObjectStatus=GOS_KEIL;

			// C64- Stil 

			if (bspos<0x2f8-80) 
			{
				theLevel->SetCol(bspos+80,130,theLevel->CB(bspos+40));
				theLevel->Set(bspos+40,32);
			}
			return 0;

		case 133:
			telep=1;
			hugocol[1]=0;
			Move(0,-1,4);
			return -1;

		case 135:
			telep=1;
			hugocol[1]=0;
			Move(0,1,4);
			return -1;

		case 137:
			telep=1;
			hugodir=0;
			hugocol[1]=0;
			Move(-1,0,5);
			return -1;

		case 136:
			telep=hugodir=1;
			hugocol[1]=0;
			Move(1,0,6);
			return -1;

		case 224:
			theLevel->AddFill(-1);
			AddScore(30);
			theLevel->Set(bspos+40,72);
			return 0;

		case 235:
		case 236:
		case 237:
		case 238:

			if (moveflag) return 0;
			PlaySound(SND_WARP,FALSE);
			moveflag=1;
			telepos=i;
			FadeOut(GOS_TELEPORT);
			return -1;

		case 239:
			// Fahrstuhl
			upflag=theLevel->SB(bspos)==60;
			downflag=theLevel->SB(bspos+80)==60;
			return 0;

		case 241: // Mine
			AddScore(30);
			theLevel->ZuendMine(bspos);
			return 0;


		case 70:  case 71:  case 72:  case 73:  case 74:
		case 75:  case 76:  case 77:  case 78:  case 79:
		case 80:  case 81:  case 82:  case 86:  case 131:
		case 138: case 139:
		case 149: case 150: case 151: case 152:
		case 185: case 186: case 187: case 188: case 189:
		case 190: case 191: case 192: case 193: case 194:
		case 195: case 196: case 197:
		case 230: case 231: case 232: case 242:

			return 0;


		default: return 1;
	}
}


int Game::ChkLeft()
{
int i=theLevel->SB(bspos-1);

	if (i==138 || i==139)
	{
		if (theLevel->SB(bspos-2)==32 && !telep && Left())
		{
			if (!--push)
			{
				push=10+rand()%25;
				theLevel->SetCol(bspos-2,i,theLevel->CB(bspos-1));
				theLevel->Set(bspos-1,32);
			}
		}
		return 1;
	}
	if (i==47 && !telep) 
	{
		if (!torschl || plMove==7 || plMove==8) return 1;
		opendoor=-1;
		GameObjectStatus=GOS_OPENDOOR;
		torschl=0;
	}
	return i==220 || i==221 || i=='Q' || i=='R' || i==131;
}

int Game::ChkRight()
{
int i=theLevel->SB(bspos+1);

	if (i==138 || i==139)
	{
		if (theLevel->SB(bspos+2)==32 && !telep && Right())
		{
			if (!--push)
			{
				push=10+rand()%25;
				theLevel->SetCol(bspos+2,i,theLevel->CB(bspos+1));
				theLevel->Set(bspos+1,32);
			}
		}
		return 1;
	}
	if (i==47 && !telep) 
	{
		if (!torschl || plMove==7 || plMove==8) return 1;
		opendoor=1;
		GameObjectStatus=GOS_OPENDOOR;
		torschl=0;
	}
	return i==220 || i==221 || i=='Q' || i=='R' || i==131;
}


int Game::ChkUnJp(void)
{
/* Returnwerte:

	0: Hugo steht
	1: Hugo fällt
*/

	if (bspos>727) return 0;
	switch(theLevel->SB(bspos+40))
	{
		case 83:  case 84:  case 85:  case 86:  case 87:  case 88:  case 106: case 107:
		case 108: case 117: case 119: case 120: case 121: case 122: case 123: case 124:
		case 125: case 126: case 127: case 128: case 129: case 130: case 133: case 135:
		case 136: case 137: case 235: case 236: case 237: case 238: case 239: case 241:
		case 70:  case 71:  case 72:  case 73:  case 74:
		case 75:  case 76:  case 77:  case 78:  case 79:
		case 80:  case 81:  case 82:  case 131: case 138: case 139: case 140: case 141:
		case 149: case 150: case 151: case 152:
		case 185: case 186: case 187: case 188: case 189:
		case 190: case 191: case 192: case 193: case 194:
		case 195: case 196: case 197: case 224:
		case 230: case 231: case 232: case 242:

			return 0;

		default: return 1;
	}
}



