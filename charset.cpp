#include "hugo.h"


#include <stdio.h>

charset::charset(char * rscname)
{
int i;

	charbuffer=(uint8_t*)GetResourcePointer(NULL,rscname,"BINARY");
	charptr=charbuffer+440;

	//ClearFlags();
	for(i=0;i<256;i++) charmap[i]=i;
	charmap[84]=charmap[85]=260;
	charmap[86]=charmap[87]=264;
	charmap[87]=268;
	charmap[120]=275;
	charmap[141]=420;
	charmap[144]=292;
	charmap[145]=293;
	charmap[147]=charmap[148]=charmap[154]=charmap[155]=32;
	charmap[156]=298;
	charmap[157]=299;
	for(i=0;i<4;i++)
	{
		charmap[168+i]=300+i;
		charmap[172+i]=392+i;
	}
	charmap[229]=294;
	charmap[242]=296;
	charmap[247]=292;
	charmap[249]=293;
	charmap[248]=charmap[250]=32;

	flashdelay=
	blinkdelay=
	exdelay=exphase=
	efelddelay=efeldphase=
	aniphase=0;

	switchit=FALSE;
}

charset::~charset()
{
}


void charset::PutChar(uint8_t * lpBits,int width,int x,int y,int c,int color)
{
int i;
auto cp=charptr+64*c;

	lpBits+=x+width*y;
	if (charbuffer[c])
	{
		for(i=0;i<8;i++)
		{
			memcpy(lpBits,cp,8);
			lpBits+=width;
			cp+=8;
		}
	}
	else
	{
		color=(color&255)*0x1010101;
		for(i=0;i<8;i++)
		{
			*(long*)lpBits=(*(long*)cp)&color;
			*(long*)(lpBits+4)=(*(long*)(cp+4))&color;
			lpBits+=width;
			cp+=8;
		}
	}
}

void charset::PutSpace(uint8_t * lpBits,int width,int x,int y)
{
int i;

	lpBits+=x+width*y;
	for(i=0;i<8;i++)
	{
		memset(lpBits,0,8);
		lpBits+=width;
	}
}


void charset::Animate(int count)
{
int c;

	for(;count;count--)
	{
		if (++blinkdelay==3)
		{
			blinkdelay=0;

			if (++charmap[141]==424) charmap[141]=420;
			
			c=charmap[144];
			charmap[144]=charmap[145];
			charmap[145]=c;
			c=charmap[148];
			charmap[148]=charmap[147];
			charmap[147]=c;
			c=charmap[154];
			charmap[154]=charmap[155];
			charmap[155]=c;
			c=charmap[156];
			charmap[156]=charmap[157];
			charmap[157]=c;

			c=charmap[247];
			charmap[247]=charmap[249];
			charmap[249]=c;
			c=charmap[248];
			charmap[248]=charmap[250];
			charmap[250]=c;

			charmap[229]^=1;
			charmap[242]^=1;

			/*
			charflags[144]=charflags[145]=charflags[156]=charflags[157]=charmap[144]!=32;
			charflags[147]=charflags[148]=charflags[154]=charflags[155]=charmap[147]!=32;
			charflags[247]=charflags[249]=charmap[247]!=32;
			charflags[248]=charflags[250]=charmap[248]!=32;
			charflags[229]=charflags[242]=TRUE;
			*/
			if (++flashdelay==70)
			{
				flashdelay=0;
				c=charmap[144];
				charmap[144]=charmap[154];
				charmap[154]=c;
				c=charmap[145];
				charmap[145]=charmap[155];
				charmap[155]=c;
				c=charmap[147];
				charmap[147]=charmap[156];
				charmap[156]=c;
				c=charmap[148];
				charmap[148]=charmap[157];
				charmap[157]=c;
				/*
				charflags[144]=charflags[145]=charflags[156]=charflags[157]=
				charflags[147]=charflags[148]=charflags[154]=charflags[155]=TRUE;
				*/
			}
		}
		if (++exdelay==5)
		{
		static const short cnr2[]={268,269,270,271,272,273,274,275,
							 274,273,272,271,270,269,268,268};
		static const short cnr1[]={274,273,272,271,270,269,268,268,
				             268,269,270,271,272,273,274,275};

			exdelay=0;
			exphase=(exphase+1)&15;
			charmap[88]=cnr2[exphase];
			charmap[120]=cnr1[exphase];
			//charflags[88]=charflags[120]=TRUE;
		}
		if (++efelddelay==30)
		{
		static const short ecnr1[]={392,388,384,380,376,372,368,364,360,356,352,348,
							  344,340,336,332,328,324,320,316,312,308,304,300,
							  300,304,308,312,316,320,324,328,332,336,340,344,
							  348,352,356,360,364,368,372,376,380,384,388,392};

		static const short ecnr2[]={300,304,308,312,316,320,324,328,332,336,340,344,
							  348,352,356,360,364,368,372,376,380,384,388,392,
							  392,388,384,380,376,372,368,364,360,356,352,348,
							  344,340,336,332,328,324,320,316,312,308,304,300};
			efelddelay=0;
			efeldphase=(efeldphase+1)%48;
			for(int i=0;i<4;i++)
			{
				charmap[168+i]=ecnr2[efeldphase]+i;
				charmap[172+i]=ecnr1[efeldphase]+i;
			}
			//memset(charflags+168,TRUE,8);
		}
		aniphase=(aniphase+1)&7;
		charmap[87]=264+(aniphase&3);
		charmap[86]=267-(aniphase&3);
		
		charmap[85]=260+(aniphase&3);
		charmap[84]=263-(aniphase&3);
		//memset(charflags+84,TRUE,4);

		charmap[116]=283-aniphase;
		charmap[117]=291-aniphase;
		charmap[118]=403-aniphase;
		charmap[119]=411-aniphase;
		//memset(charflags+116,TRUE,4);

		if (switchit)
		{
			switchit=FALSE;
			c=charmap[247];
			charmap[247]=charmap[248];
			charmap[248]=c;
			c=charmap[249];
			charmap[249]=charmap[250];
			charmap[250]=c;
			//charflags[247]=charflags[248]=charflags[249]=charflags[250]=TRUE;
		}
	}
}

