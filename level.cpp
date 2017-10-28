#include "hugo.h"

extern "C" int _fltused=0;

level::level(charset * cs)
{
	theCharSet=cs;
	lpsprBall=(const uint8_t*)GetResourcePointer(NULL,MAKEINTRESOURCE(IDR_SPRITE),"BINARY");
	currentLevel=-1;
	HANDLE h=OpenFile("HUGO.LEV");
	numLevels=GetFileSize(h,NULL)/0x660;
	CloseFile(h);
	prepared=FALSE;
}

BOOL level::Load(int nr)
{
	HANDLE h=OpenFile("HUGO.LEV");
	SetFilePointer(h,nr*0x660+8,NULL,FILE_BEGIN);
	int i=ReadFile(h,rawlevbuf,0x660);
	CloseFile(h);
	prepared=FALSE;
	return i==0x660 && *rawlevbuf=='R';
}

void level::Save(int nr)
{
sprite * sp;
int i,sn;

	memcpy(rawlevbuf,screenbuf,0x300);
	memcpy(rawlevbuf+0x300,colbuf,0x300);
	memset(rawlevbuf+0x604,0,0x5c);
	for(sp=sprites,i=2,sn=0;sp<sprites+7;sp++,i<<=1,sn++) if (sp->present)
	{
		rawlevbuf[0x603]|=i;
		rawlevbuf[0x604+sn]=sp->direction;
		switch(sp->direction)
		{
			case 1:
				*(short*)(rawlevbuf+0x627+2*sn)=sp->ypos;
				*(short*)(rawlevbuf+0x60b+2*sn)=sp->lorand;
				*(short*)(rawlevbuf+0x619+2*sn)=sp->rurand;
				break;
			case 2:
				*(short*)(rawlevbuf+0x627+2*sn)=sp->xpos;
				*(short*)(rawlevbuf+0x60b+2*sn)=sp->lorand;
				*(short*)(rawlevbuf+0x619+2*sn)=sp->rurand;
				break;
			case 3:
			case 4:
				*(short*)(rawlevbuf+0x60b+2*sn)=sp->sx;
				*(short*)(rawlevbuf+0x619+2*sn)=sp->sy;
				*(short*)(rawlevbuf+0x627+2*sn)=sp->ex;
				*(short*)(rawlevbuf+0x635+2*sn)=sp->ey;
				break;
			case 6:
			case 7:
				*(short*)(rawlevbuf+0x60b+2*sn)=sp->mx;
				*(short*)(rawlevbuf+0x619+2*sn)=sp->my;
				*(short*)(rawlevbuf+0x627+2*sn)=sp->radx;
				*(short*)(rawlevbuf+0x635+2*sn)=sp->rady;
				break;
		}
	}
	HANDLE h=OpenFile("HUGO.LEV",GENERIC_WRITE,0);
	SetFilePointer(h,nr*0x660+8,NULL,FILE_BEGIN);
	WriteFile(h,rawlevbuf,0x660);
	CloseFile(h);
}


void level::PrepareSprites()
{
int i,sn;
sprite *sp;

	for(sp=sprites,i=2,sn=0;sp<sprites+7;sp++,i<<=1,sn++)
	{
		sp->off=0;
		sp->present=rawlevbuf[0x603]&i;
		sp->direction=rawlevbuf[0x604+sn];
		if (sp->direction<3)
		{
			sp->lorand=*(short*)(rawlevbuf+0x60b+2*sn);
			sp->rurand=*(short*)(rawlevbuf+0x619+2*sn);
			if (sp->direction==1)
			{
				sp->xpos=sp->lorand;
				sp->ypos=*(short*)(rawlevbuf+0x627+2*sn);
			}
			else
			{
				sp->ypos=sp->lorand;
				sp->xpos=*(short*)(rawlevbuf+0x627+2*sn);
			}
		}
		else switch(sp->direction)
		{
			case 3:
			case 4:
				sp->sx=*(short*)(rawlevbuf+0x60b+2*sn);
				sp->sy=*(short*)(rawlevbuf+0x619+2*sn);
				sp->ex=*(short*)(rawlevbuf+0x627+2*sn);
				sp->ey=*(short*)(rawlevbuf+0x635+2*sn);
				sp->xpos=sp->sx;
				sp->ypos=sp->sy;
				sp->yadd=sp->sy<sp->ey? 1:-1;
				sp->xadd=sp->sx<sp->ex? 1:-1;
				sp->lenX=abs(sp->ex-sp->sx);
				sp->lenY=abs(sp->ey-sp->sy);
				break;

			case 6:
			case 7:
				sp->mx=*(short*)(rawlevbuf+0x60b+2*sn);
				sp->my=*(short*)(rawlevbuf+0x619+2*sn);
				sp->radx=*(short*)(rawlevbuf+0x627+2*sn);
				sp->rady=*(short*)(rawlevbuf+0x635+2*sn);
				sp->xpos=sp->mx-sp->rady;
				sp->ypos=sp->sy;
				sp->angle=0;
				break;
		}
	}
}


void level::PrepareGame(void)
{
ball *bp;
int i;

	boulder=blink=fillct=items=fackeln=items2=fack2=bombs=items=sbalct=wbalct=0;

	memcpy(colbuf,rawlevbuf+0x300,0x300);
	memcpy(colblink,rawlevbuf+0x300,0x300);
	//memset(offspr,0,7);

	if (GetSpecials()&128) memset(colblink,0,0x300);
	PrepareSprites();
	for(i=0;i<0x300;i++)
	{
		switch(rawlevbuf[i])
		{
			case 30:
				bombs++;
				break;

			case 33:
				if (wbalct<8)
				{
					bp=wballs+wbalct++;
					bp->b_ani=0;
					bp->b_pos=i;
					bp->b_yp=(i/40)<<3;
					bp->b_oldppos=bp->b_xp=(i%40)<<3;
					bp->b_color=rawlevbuf[i+0x300];
					screenbuf[i]=8;
					continue;
				}
				screenbuf[i]=32;
				continue;

			case 39:
				screenbuf[i]=18;
				continue;

			case 42:
				screenbuf[i]=19;
				continue;

			case 66:
			case 67:
			case 68:
			case 69:
			case 141:
				items++;
				break;

			case 229:
				fackeln++;
				break;

			case 243:
				if (sbalct<8)
				{
					bp=sballs+sbalct++;
					bp->b_ani=0;
					bp->b_pos=i;
					bp->b_oldppos=bp->b_yp=(i/40)<<3;
					bp->b_xp=(i%40)<<3;
					bp->b_color=rawlevbuf[i+0x300];
					screenbuf[i]=16;
					continue;
				}
				screenbuf[i]=32;
				continue;

			case 224:
				fillct++;
				break;

			case 251:
				fack2++;
				screenbuf[i]=32;
				continue;

			case 252:
			case 253:
			case 254:
			case 255:
				items2++;
				screenbuf[i]=32;
				continue;
		}
		screenbuf[i]=rawlevbuf[i];
	}
	memcpy(sballbak,sballs,sizeof(sballs));
	memcpy(wballbak,wballs,sizeof(wballs));
	balldelay=1;
	prepared=1;
}


void level::PrepareEditor()
{
	memcpy(colbuf,rawlevbuf+0x300,0x300);
	memcpy(screenbuf,rawlevbuf,0x300);
	//memset(offspr,0,7);
	PrepareSprites();
	prepared=2;
}


void level::MoveBalls()
{
int i;
ball * bsp, * bwp;
//static signed char cadd[]={0,-8,-8,-8,-0,-8,0,0,0};
//static char cpos[]={0,6,4,2,0,8,2,4,6};
//int pos;

	if (--balldelay) return;
	balldelay=2;
	for(i=0,bsp=sballs,bwp=wballs;i<8;bsp++,bwp++,i++)
	{
		if (i<wbalct)
		{
			if (bwp->b_wait)
			{
				bwp->b_wait=0;
				continue;
			}
			switch(bwp->b_ani)
			{
				case 0:

					if (screenbuf[bwp->b_pos+1]==32)
					{
						screenbuf[bwp->b_pos]=1;
						screenbuf[bwp->b_pos+1]=2;
						colbuf[bwp->b_pos]=colbuf[bwp->b_pos+1]=bwp->b_color;
						bwp->b_ani++;
					}
					else if (screenbuf[bwp->b_pos-1]==32)	// rechts ist blockiert!
					{
						screenbuf[bwp->b_pos-1]=5;
						screenbuf[bwp->b_pos]=6;
						colbuf[bwp->b_pos]=colbuf[bwp->b_pos-1]=bwp->b_color;
						bwp->b_ani=-4;
						bwp->b_wait=1;
					}
					break;
				case 1:
					screenbuf[bwp->b_pos]=3;
					screenbuf[bwp->b_pos+1]=4;
					colbuf[bwp->b_pos]=colbuf[bwp->b_pos+1]=bwp->b_color;
					bwp->b_ani++;
					break;
				case 2:
					screenbuf[bwp->b_pos]=5;
					screenbuf[bwp->b_pos+1]=6;
					colbuf[bwp->b_pos]=colbuf[bwp->b_pos+1]=bwp->b_color;
					bwp->b_ani++;
					break;
				case 3:
					screenbuf[bwp->b_pos]=32;
					screenbuf[bwp->b_pos+1]=8;
					colbuf[bwp->b_pos+1]=bwp->b_color;
					bwp->b_pos++;
					bwp->b_ani=0;
					break;

				case -5:
					if (screenbuf[bwp->b_pos-1]==32)
					{
						screenbuf[bwp->b_pos-1]=5;
						screenbuf[bwp->b_pos]=6;
						colbuf[bwp->b_pos]=colbuf[bwp->b_pos-1]=bwp->b_color;
						bwp->b_ani++;
					}
					else if (screenbuf[bwp->b_pos+1]==32) // links ist blockiert
					{
						screenbuf[bwp->b_pos]=1;
						screenbuf[bwp->b_pos+1]=2;
						colbuf[bwp->b_pos]=colbuf[bwp->b_pos+1]=bwp->b_color;
						bwp->b_ani=1;
						bwp->b_wait=1;
					}
					break;
				case -4:
					screenbuf[bwp->b_pos-1]=3;
					screenbuf[bwp->b_pos]=4;
					colbuf[bwp->b_pos]=colbuf[bwp->b_pos-1]=bwp->b_color;
					bwp->b_ani++;
					break;
				case -3:
					screenbuf[bwp->b_pos-1]=1;
					screenbuf[bwp->b_pos]=2;
					colbuf[bwp->b_pos]=colbuf[bwp->b_pos-1]=bwp->b_color;
					bwp->b_ani++;
					break;
				case -2:
					screenbuf[bwp->b_pos-1]=8;
					screenbuf[bwp->b_pos]=32;
					colbuf[bwp->b_pos-1]=bwp->b_color;
					bwp->b_pos--;
					bwp->b_ani=-5;
					break;

			}
			//bwp->b_oldppos= (pos=bwp->b_xp+cadd[bwp->b_ani+5])+cpos[bwp->b_ani+5];
		}
		if (i<sbalct)
		{
			if (bsp->b_wait)
			{
				bsp->b_wait=0;
				continue;
			}
			switch(bsp->b_ani)
			{
				case 0:

					if (screenbuf[bsp->b_pos+40]==32)
					{
						screenbuf[bsp->b_pos]=9;
						screenbuf[bsp->b_pos+40]=10;
						colbuf[bsp->b_pos]=colbuf[bsp->b_pos+40]=bsp->b_color;
						bsp->b_ani++;
					}
					else if (screenbuf[bsp->b_pos-40]==32)
					{
						screenbuf[bsp->b_pos-40]=13;
						screenbuf[bsp->b_pos]=14;
						colbuf[bsp->b_pos]=colbuf[bsp->b_pos-40]=bsp->b_color;
						bsp->b_ani=-4;
						bsp->b_wait=1;
					}
					break;
				case 1:
					screenbuf[bsp->b_pos]=11;
					screenbuf[bsp->b_pos+40]=12;
					colbuf[bsp->b_pos]=colbuf[bsp->b_pos+40]=bsp->b_color;
					bsp->b_ani++;
					break;
				case 2:
					screenbuf[bsp->b_pos]=13;
					screenbuf[bsp->b_pos+40]=14;
					colbuf[bsp->b_pos]=colbuf[bsp->b_pos+40]=bsp->b_color;
					bsp->b_ani++;
					break;
				case 3:
					screenbuf[bsp->b_pos]=32;
					screenbuf[bsp->b_pos+40]=16;
					colbuf[bsp->b_pos+40]=bsp->b_color;
					bsp->b_pos+=40;
					bsp->b_ani=0;
					break;

				case -5:
					if (screenbuf[bsp->b_pos-40]==32)
					{
						screenbuf[bsp->b_pos-40]=13;
						screenbuf[bsp->b_pos]=14;
						colbuf[bsp->b_pos]=colbuf[bsp->b_pos-40]=bsp->b_color;
						bsp->b_ani++;
					}
					else if (screenbuf[bsp->b_pos+40]==32) // oben ist blockiert
					{
						screenbuf[bsp->b_pos]=9;
						screenbuf[bsp->b_pos+40]=10;
						colbuf[bsp->b_pos]=colbuf[bsp->b_pos+40]=bsp->b_color;
						bsp->b_ani=1;
						bsp->b_wait=1;
					}
					break;
				case -4:
					screenbuf[bsp->b_pos-40]=11;
					screenbuf[bsp->b_pos]=12;
					colbuf[bsp->b_pos]=colbuf[bsp->b_pos-40]=bsp->b_color;
					bsp->b_ani++;
					break;
				case -3:
					screenbuf[bsp->b_pos-40]=9;
					screenbuf[bsp->b_pos]=10;
					colbuf[bsp->b_pos]=colbuf[bsp->b_pos-40]=bsp->b_color;
					bsp->b_ani++;
					break;
				case -2:
					screenbuf[bsp->b_pos-40]=16;
					screenbuf[bsp->b_pos]=32;
					colbuf[bsp->b_pos-40]=bsp->b_color;
					bsp->b_pos-=40;
					bsp->b_ani=-5;
					break;

			}
		}
	}
}


void level::MoveSprites()
{
sprite *sp;

	for(sp=sprites;sp<sprites+7;sp++) if (sp->present)
	{
		switch(sp->direction)
		{
			case -2:
				if (--sp->ypos==sp->lorand) sp->direction=2;
				break;

			case -1:
				if (--sp->xpos==sp->lorand) sp->direction=1;
				break;

			case 1:
				if (++sp->xpos==sp->rurand) sp->direction=-1;
				break;

			case 2:
				if (++sp->ypos==sp->rurand) sp->direction=-2;
				break;

			case 3:
				if (sp->lenX>sp->lenY) 
				{
					sp->xpos+=sp->xadd;
					sp->ypos=sp->sy+sp->yadd*abs(sp->xpos-sp->sx)*sp->lenY/sp->lenX;
					if (sp->xpos==sp->ex) sp->direction=-3;
				}
				else
				{
					sp->ypos+=sp->yadd;
					sp->xpos=sp->sx+sp->xadd*abs(sp->ypos-sp->sy)*sp->lenX/sp->lenY;
					if (sp->ypos==sp->ey) sp->direction=-3;
				}
				break;

			case -3:
				if (sp->lenX>sp->lenY) 
				{
					sp->xpos-=sp->xadd;
					sp->ypos=sp->sy+sp->yadd*abs(sp->xpos-sp->sx)*sp->lenY/sp->lenX;
					if (sp->xpos==sp->sx) sp->direction=3;
				}
				else
				{
					sp->ypos-=sp->yadd;
					sp->xpos=sp->sx+sp->xadd*abs(sp->ypos-sp->sy)*sp->lenX/sp->lenY;
					if (sp->ypos==sp->sy) sp->direction=3;
				}
				break;

			case 4:
				if ((sp->xpos+=sp->xadd)==sp->ex) sp->direction=5;
				break;

			case 5:
				if ((sp->ypos+=sp->yadd)==sp->ey) sp->direction=-4;
				break;

			case -4:
				if ((sp->xpos-=sp->xadd)==sp->sx) sp->direction=-5;
				break; 

			case -5:
				if ((sp->ypos-=sp->yadd)==sp->sy) sp->direction=4;
				break;

			case 6:
				sp->angle+=2*M_PI/360;
				if (sp->angle>=2*M_PI) sp->angle=0;
				sp->xpos=(int)(sp->mx-sp->radx*cos(sp->angle));
				sp->ypos=(int)(sp->my-sp->rady*sin(sp->angle));
				break;

			case 7:
				sp->angle+=2*M_PI/360;
				if (sp->angle>=2*M_PI) sp->angle=0;
				sp->xpos=(int)(sp->mx-sp->radx*cos(sp->angle));
				sp->ypos=(int)(sp->my+sp->rady*sin(sp->angle));
				break;

		}
	}
}



void level::Animate(int count)
{
	if (prepared==1) for(;count;count--) 
	{
		MoveSprites();
		MoveBalls();
		DoBomb();
		DoMine();
	}
}


void level::Draw(DIB & ddd,BOOL drawsprites)
{
int x,y,i;
sprite * sp;
uint8_t * cbp;


	int width=ddd.Width();
	auto lpBits=ddd.Bits();

	cbp= (GetSpecials() && drawsprites) ? colblink:colbuf;
	for(i=0,y=0;y<152;y+=8) for(x=0;x<320;x+=8,i++)
	{
		if (cbp[i]) theCharSet->DrawChar(lpBits,width,x,y,screenbuf[i],cbp[i]);
		else theCharSet->PutSpace(lpBits,width,x,y);
	}
	for(x=0;x<320;x+=8) theCharSet->DrawChar(lpBits,width,x,152,'R',rawlevbuf[0x300]);

	if (drawsprites) for(x=0,sp=sprites;x<7;x++,sp++)
	{
		if (sp->present && !sp->off) ddd.DisplaySpriteEx(sp->xpos,sp->ypos,0,0,0,0,320,160,lpsprBall,x);
	}
}


void level::DrawSpriteTracks(HDC hDC)
{
HPEN hp,hp1=(HPEN)GetCurrentObject(hDC,OBJ_PEN);
HBRUSH hbr=(HBRUSH)GetCurrentObject(hDC,OBJ_BRUSH);
int x;
sprite * sp;
static COLORREF crs[]={PALETTERGB(255,255,0),PALETTERGB(255,128,0),PALETTERGB(0,255,0),
  PALETTERGB(0,255,255),PALETTERGB(0,70,255),PALETTERGB(230,0,255),PALETTERGB(255,0,130)};

	for(x=0,sp=sprites;x<7;x++,sp++)
	{
		hp=CreatePen(PS_SOLID,1,crs[x]);
		SelectObject(hDC,hp);
		if (sp->present) 
		{
			switch(sp->direction)
			{
				case 1:
					Line(hDC,sp->lorand+12,sp->ypos+10,sp->rurand+12,sp->ypos+10);
					break;

				case 2:
					Line(hDC,sp->xpos+12,sp->lorand+10,sp->xpos+12,sp->rurand+10);
					break;

				case 3:
					Line(hDC,sp->sx+12,sp->sy+10,sp->ex+12,sp->ey+10);
					break;

				case 4:
					SelectObject(hDC,GetStockObject(NULL_BRUSH));
					Rectangle(hDC,sp->sx+12,sp->sy+10,sp->ex+12,sp->ey+10);
					break;

				case 6:
				case 7:
					SelectObject(hDC,GetStockObject(NULL_BRUSH));
					Ellipse(hDC,sp->mx+12-sp->radx,sp->my+10-sp->rady,sp->mx+12+sp->radx,sp->my+10+sp->rady);
					break;
			}
		}
		SelectObject(hDC,hp1);
		DeleteObject(hp);
	}
	SelectObject(hDC,hbr);
}


void level::DrawCollisionSprites(DIB & cd,int xx,int y)
{
static const uint8_t mask[]={0,0,0,0,0,0,0,0};
int x;
sprite * sp;

	for(x=0,sp=sprites;x<7;x++,sp++)
	{
		if (sp->present && !sp->off) 
		{
			cd.TranslateSpriteEx(sp->xpos-xx,sp->ypos-y,0,0,0,0,12,14,lpsprBall,x,mask-200);
		}
	}
}


void level::ClearHints()
{
int i,l;

	for(i=0;i<0x300;i++)
	{
		l=screenbuf[i];
		if (l==18 || l==19) screenbuf[i]=32;
	}
}

void level::SpritesOn()
{
int x;
sprite * sp;

	for(x=0,sp=sprites;x<7;x++,sp++)
	{
		sp->off=0;
	}
	off1=off2=-1;
}


void level::SpriteOff(int no)
{
	if (off1!=no && off2!=no)
	{
		sprites[no].off=TRUE;
		if (off2!=-1) sprites[off2].off=FALSE;
		off2=off1;
		off1=no;
	}
}

void level::RstFahr()
{
int i,l;

	for(i=0;i<0x300;i++)
	{
		l=rawlevbuf[i];
		if (l==60 || l==239) screenbuf[i]=l;
	}
}


void level::RstMin()
{
int i,l;

	ClrMinTab();
	for(i=0;i<0x300;i++)
	{
		l=rawlevbuf[i];
		if (l==241 && screenbuf[i]==32) screenbuf[i]=l;
	}
}


void level::Klapp()
{
int k,l;

	for(k=0;k<0x300;k++)
	{
		l=screenbuf[k];
		if (l==35 || l==36 || l==230 || l==231)
		{
			if (l>200) l-=195; else l+=195;
			screenbuf[k]=l;
		}
	}
}


void level::ExWall()
{
int k,l;

	for(k=0;k<0x300;k++)
	{
		l=rawlevbuf[k];
		if (l==131 || l==166) l=32;
		else if (l==39) l=131;
		else if (l==42) l=166;
		else continue;
		screenbuf[k]=l;
	}
}


void level::Rebuild()
{
int k,l;

	// rebx
	KillBomb();
	memcpy(sballs,sballbak,sizeof(sballs));
	memcpy(wballs,wballbak,sizeof(wballs));
	for(k=0;k<0x300;k++)
	{
		l=rawlevbuf[k];
		if (l<251 && (l>69 || l<64) && l!=229 && l!=163 && l!=141
			&& l!=164 && l!=142 && l!=30 && l!=158
			&& l!=159 && l!=33 && l!=243 && l!=screenbuf[k])
		{
			if (l==224) l='H';
			else if (l==39 || l==42) l=32;
			screenbuf[k]=l;
			colbuf[k]=rawlevbuf[k+0x300];
		}
	}
	ClrMinTab();
}

int level::SetSet2()
{
int k,l;
static const uint8_t transs[]={229,66,67,68,69};

	if ((items+fackeln) || GetItems2()==0) return 0;
	for(k=0;k<0x300;k++)
	{
		l=rawlevbuf[k];
		if (l>250) screenbuf[k]=transs[l-251];
	}
	items=items2;
	fackeln=fack2;
	items2=fack2=0;
	return 1;
}


BOOL level::SetBomb(int pos)
{
	if (bomb_count) return FALSE;
	theCharSet->SetBombAni(0);
	screenbuf[pos]=31;
	colbuf[pos]=144;
	bomb_bs=pos;
	bomb_count=192;
	//AddBombs(-1);
	return TRUE;
}


void level::DoBomb(void)
{
int l,i;
static signed char bsp_of[]={-41,-40,-39,-1,0,1,39,40,41};

	if (bomb_count)
	{
		switch(--bomb_count)
		{
			case 168:
				l=1;
				goto lunte;

			case 144:
				l=2;
				goto lunte;

			case 120:
				l=3;
			lunte:
				theCharSet->SetBombAni(l);
				return;

			case 96:
				//playvoice(explo1);
				for(i=0;i<9;i++)
				{
					if (bomb_bs+bsp_of[i]>0x2f8) continue;
					l=screenbuf[bomb_bs+bsp_of[i]];
					if (l==64 || l==65 || l==82) continue;
					switch(l)
					{
						case 66:
						case 67:
						case 68:
						case 69:
						case 141:
							items--;
							break;

						case 229:
							fackeln--;
							break;

						case 224:
							fillct--;
							break;

						case 30:
							bombs--;
							break;
					}
					screenbuf[bomb_bs+bsp_of[i]]=41;
					colbuf[bomb_bs+bsp_of[i]]=100;
				}
				return;

			case 68: case 67:
			case 66: case 65: case 64: case 63: case 62: case 61:
			case 60: case 59: case 58: case 57: case 56: case 55:
			case 54: case 53: case 52: case 51: case 50: case 49:
			case 48: case 47: case 46: case 45: case 44: case 43:
			case 42: case 41: case 40: case 39: case 38: case 37:
			case 36: case 35: case 34: case 33: case 32: case 31:
			case 30: case 29: case 28: case 27: case 26: case 25:
			case 24: case 23: case 22: case 21: case 20: case 19:
			case 18: case 17: case 16: case 15: case 14: case 13:

				for(i=0;i<9;i++)
				{
					if (bomb_bs+bsp_of[i]<=0x2f8 && screenbuf[bomb_bs+bsp_of[i]]==41) 
						colbuf[bomb_bs+bsp_of[i]]++;
				}
				return;

			case 1:
				for(i=0;i<9;i++)
				{
					if (bomb_bs+bsp_of[i]>0x2f8) continue;
					l=screenbuf[bomb_bs+bsp_of[i]];
					if (l==41)  screenbuf[bomb_bs+bsp_of[i]]=32;
				}
				return;
		}
	}
}



void level::ZuendMine(int bspos)
{
mine *mp;

	for(mp=mintab;mp<mintab+47;mp++)
	{
		if (!mp->counter)
		{
		i:
			mp->bspos=bspos+40;
			mp->counter=120;
			screenbuf[mp->bspos]=242;
			return;
		}
	}
	goto i;
}


void level::DoMine()
{
mine *mp;
int c;

	for(mp=mintab;mp<mintab+48;mp++)
	{
		if (mp->counter)
		{
			switch(--mp->counter)
			{
				case 0:
					c=32;
					break;

				case 24:
					c=61;
					break;

				default: continue;
			}
			screenbuf[mp->bspos]=c;
		}
	}
}


/*
BOOL level::SetExplosion(int pos)
{
	if (explo_count) return FALSE;
	explo_count=96;
	explo_bs=pos;
	return TRUE;
}


void level::DoExplosion(void)
{
int l,i;
static signed char bsp_of[]={-41,-40,-39,-1,0,1,39,40,41};

	if (explo_count)
	{
		switch(--explo_count)
		{
			case 95:
				//playvoice(explo1);
				for(i=0;i<9;i++)
				{
					if (explo_bs+bsp_of[i]>0x2f8) continue;
					l=screenbuf[explo_bs+bsp_of[i]];
					if (l==64 || l==65 || l==82) continue;
					switch(l)
					{
						case 66:
						case 67:
						case 68:
						case 69:
						case 141:
							items--;
							break;

						case 229:
							fackeln--;
							break;

						case 224:
							fillct--;
							break;

						case 30:
							explos--;
							break;
					}
					screenbuf[explo_bs+bsp_of[i]]=41;
					colbuf[explo_bs+bsp_of[i]]=100;
				}
				return;

			case 68: case 67:
			case 66: case 65: case 64: case 63: case 62: case 61:
			case 60: case 59: case 58: case 57: case 56: case 55:
			case 54: case 53: case 52: case 51: case 50: case 49:
			case 48: case 47: case 46: case 45: case 44: case 43:
			case 42: case 41: case 40: case 39: case 38: case 37:
			case 36: case 35: case 34: case 33: case 32: case 31:
			case 30: case 29: case 28: case 27: case 26: case 25:
			case 24: case 23: case 22: case 21: case 20: case 19:
			case 18: case 17: case 16: case 15: case 14: case 13:

				for(i=0;i<9;i++)
				{
					if (explo_bs+bsp_of[i]<=0x2f8 && screenbuf[explo_bs+bsp_of[i]]==41) 
						colbuf[explo_bs+bsp_of[i]]++;
				}
				return;

			case 1:
				for(i=0;i<9;i++)
				{
					if (explo_bs+bsp_of[i]>0x2f8) continue;
					l=screenbuf[explo_bs+bsp_of[i]];
					if (l==41)  screenbuf[explo_bs+bsp_of[i]]=32;
				}
				return;
		}
	}
}
*/


void level::DoBlink()
{
static const uint8_t blinktab[]={
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,
0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0};
int j,i;

	if (!(GetSpecials()&64)) return;
	if (blink=!blink)
	{
		// nur Spielfeld (0+1)
		for(i=41;i<759;i++)
		{
			j=i%40;
			if (j==0 || j==39) continue;
			if (blinktab[screenbuf[i]]!=2) colblink[i]=colbuf[i];
			else colblink[i]=0;
		}
	}
	else
	{
		// nur Spielfeld (0+1)
		for(i=41;i<759;i++)
		{
			j=i%40;
			if (j==0 || j==39) continue;
			if (blinktab[screenbuf[i]]!=0) colblink[i]=colbuf[i];
			else colblink[i]=0;
		}
	}

}



void level::DoBlack(int bspos)
{
	if (!(GetSpecials()&128)) return;
	if (bspos>80) colblink[bspos-80]=colbuf[bspos-80];
	if (bspos>40)
	{
		colblink[bspos-41]=colbuf[bspos-41];
		colblink[bspos-40]=colbuf[bspos-40];
		colblink[bspos-39]=colbuf[bspos-39];
	}
	colblink[bspos-1]=colbuf[bspos-1];
	colblink[bspos]=colbuf[bspos];
	colblink[bspos+1]=colbuf[bspos+1];
	if (bspos<0x2f8-41)
	{
		colblink[bspos+41]=colbuf[bspos+41];
		colblink[bspos+40]=colbuf[bspos+40];
		colblink[bspos+39]=colbuf[bspos+39];
	}
}






void level::CreateEmptyLevel()
{
int i;

	memset(rawlevbuf,32,0x300);
	memset(rawlevbuf+0x300,6,0x300);
	memset(rawlevbuf+0x600,0,0x60);
	memset(rawlevbuf,'R',40);
	memset(rawlevbuf+0x2f8,'R',8);
	for(i=0;i<19;i++)
	{
		rawlevbuf[i*40]=rawlevbuf[i*40+39]='R';
	}
	PrepareEditor();
}

void level::SetFrameColor(int c)
{
int i;

	memset(colbuf,c,40);
	for(i=0;i<19;i++)
	{
		colbuf[i*40]=colbuf[i*40+39]=c;
	}
	rawlevbuf[0x300]=c;
}




BOOL level::BoulderDash(int bspos)
{
int i,j;
BOOL f=FALSE;

	if (++boulder==10)
	{
		boulder=0;
		for(j=40;j<0x2f8-40;j++)
		{
			i=screenbuf[j];
			if (i==138 || i==139 || i==141)
			{
				if (screenbuf[j+40]==32 && bspos!=j+40)
				{
					screenbuf[j+40]=i-90;
					colbuf[j+40]=colbuf[j];
					colblink[j+40]=colblink[j];
					screenbuf[j]=32;
				}
				else if (i!=138)
				{
					if (screenbuf[j+39]==32 && bspos!=j+39 && screenbuf[j-1]==32)
					{
						screenbuf[j+39]=i-90;
						colbuf[j+39]=colbuf[j];
						colblink[j+39]=colblink[j];
						screenbuf[j]=32;
					}
					else if (screenbuf[j+41]==32 && bspos!=j+41 && screenbuf[j+1]==32)
					{
						screenbuf[j+41]=i-90;
						colbuf[j+41]=colbuf[j];
						colblink[j+41]=colblink[j];
						screenbuf[j]=32;
					}
				}
			}
		}
		for(j=40;j<0x2f8;j++)
		{
			i=screenbuf[j];
			if (i==48 || i==49 || i==51) 
			{
				screenbuf[j]+=90;
				if (bspos==j+40) f=TRUE;
			}
		}
	}
	return f;
}
