#include <Windows.h>
#include <stdint.h>
/****************************************
 *									    *
 *									    *
 *  Sprite display routines for Win32   *
 *									    *
 *									    *
 *	   written and (c) 1993,1996 by     *
 *									    *
 *									    *
 *	         Christoph Oelckers         *
 *									    *
 *									    *
 ****************************************/

#define FC(a,b,c,d) mmioFOURCC((a),(b),(c),(d))

enum codebytes2 {	// ID PKSC,PKSW
	copy,		// copy, length= 2.byte+80
	fill,		// fill, length= 2.byte+80
	trans,		// transparent, length = 2.byte+80
	linend_b,	// line end, offset as byte  
	linend_w,	// line end, offset as word
	copy_p_336,	// copy, length= 2.byte+336
	fill_p_336,	// fill, length= 2.byte+336
	header2,	// skip (2.byte) bytes
	ende,		// end of sprite
	trans_p_336,// transparent, length= 2.byte+336
	copy_w,		// copy, length as word
	fill_w,		// fillen, length as word
	trans_w,	// transparent, length as word
	palette,	// palette data 
	__unused__,
	escape,		// escape-code for future extensions
	copy_1,		// copy 1 byte
	copy_2,         // etc.
	copy_3, copy_4, copy_5, copy_6, copy_7, copy_8, copy_9, copy_10, copy_11,
	copy_12, copy_13, copy_14, copy_15, copy_16, copy_17, copy_18, copy_19,
	copy_20, copy_21, copy_22, copy_23, copy_24, copy_25, copy_26, copy_27,
	copy_28, copy_29, copy_30, copy_31, copy_32, copy_33, copy_34, copy_35,
	copy_36, copy_37, copy_38, copy_39, copy_40, copy_41, copy_42, copy_43,
	copy_44, copy_45, copy_46, copy_47, copy_48, copy_49, copy_50, copy_51,
	copy_52, copy_53, copy_54, copy_55, copy_56, copy_57, copy_58, copy_59,
	copy_60, copy_61, copy_62, copy_63, copy_64, copy_65, copy_66, copy_67,
	copy_68, copy_69, copy_70, copy_71, copy_72, copy_73, copy_74, copy_75,
	copy_76, copy_77, copy_78, copy_79, copy_80,
	fill_1, fill_2, fill_3, fill_4, fill_5, fill_6, fill_7, fill_8, fill_9,
	fill_10, fill_11, fill_12, fill_13, fill_14, fill_15, fill_16, fill_17,
	fill_18, fill_19, fill_20, fill_21, fill_22, fill_23, fill_24, fill_25,
	fill_26, fill_27, fill_28, fill_29, fill_30, fill_31, fill_32, fill_33,
	fill_34, fill_35, fill_36, fill_37, fill_38, fill_39, fill_40, fill_41,
	fill_42, fill_43, fill_44, fill_45, fill_46, fill_47, fill_48, fill_49,
	fill_50, fill_51, fill_52, fill_53, fill_54, fill_55, fill_56, fill_57,
	fill_58, fill_59, fill_60, fill_61, fill_62, fill_63, fill_64, fill_65,
	fill_66, fill_67, fill_68, fill_69, fill_70, fill_71, fill_72, fill_73,
	fill_74, fill_75, fill_76, fill_77, fill_78, fill_79, fill_80,
	trans_1, trans_2, trans_3, trans_4, trans_5, trans_6, trans_7, trans_8,
	trans_9, trans_10, trans_11, trans_12, trans_13, trans_14, trans_15,
	trans_16, trans_17, trans_18, trans_19, trans_20, trans_21, trans_22,
	trans_23, trans_24, trans_25, trans_26, trans_27, trans_28, trans_29,
	trans_30, trans_31, trans_32, trans_33, trans_34, trans_35, trans_36,
	trans_37, trans_38, trans_39, trans_40, trans_41, trans_42, trans_43,
	trans_44, trans_45, trans_46, trans_47, trans_48, trans_49, trans_50,
	trans_51, trans_52, trans_53, trans_54, trans_55, trans_56, trans_57,
	trans_58, trans_59, trans_60, trans_61, trans_62, trans_63, trans_64,
	trans_65, trans_66, trans_67, trans_68, trans_69, trans_70, trans_71,
	trans_72, trans_73, trans_74, trans_75, trans_76, trans_77, trans_78,
	trans_79, trans_80
};

enum indexbytes {
	copy_imp = 16, fill_imp, trans_imp
};


static const uint8_t indextab[] =
{
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,

	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,

	18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,
	18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18
};



/********************************************************************/
/* DIBDisplaySpriteEx												*/
/*																	*/
/* This function display a sprite and clips it into a clipping 		*/
/* rectangle     													*/
/********************************************************************/
int DIBDisplaySpriteEx(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		int		clip_left,				// clipping rectangle
		int		clip_top,
		int		clip_right,
		int		clip_bottom,
		uint8_t *	sprbuf,					// sprite data
		int		index)					// sprite number

{
uint8_t * lineofs, * lineptr;
uint8_t * srcptr=sprbuf;
int ClipXDo,ClipYDo;
int ClipXSkip,ClipYSkip;
int ClipXDoStore,ClipXSkipStore;
int dstWidth=(lpBMI->biWidth+3)&-4;
int dstHeight=lpBMI->biHeight;
int i,j,h;

__try
{
	if ((j=*(int32_t*)srcptr)==0x50534449)
	{
		if (index>=*(int32_t*)(srcptr+4)) 
		{
			return 0;
		}
		srcptr+=*(int32_t*)(srcptr+4*index+8);
		j=*(int32_t*)srcptr;
	}
	if (j!=FC('P','K','S','W') && j!=FC('P','K','S','C') && j!=FC('P','K','X','W'))
	{
		return 0;
	}
	if (j==FC('P','K','X','W'))
	{
		if (srcptr[5]<8) return 0;
		xpos+=((short*)srcptr)[5]*(mirr_x?-1:1);
		ypos+=((short*)srcptr)[6]*(mirr_y?-1:1);
	}
	srcptr+=4;

	ClipYDo=clip_bottom-clip_top+1;
	ClipYSkip=mirr_y? ypos-clip_bottom:clip_top-ypos;
	if (ClipYSkip<0) ClipYDo+=ClipYSkip;
	ClipXDoStore=clip_right-clip_left+1;
	ClipXSkipStore=mirr_x? xpos-clip_right:clip_left-xpos;
	if (ClipXSkipStore<0) ClipXDoStore+=ClipXSkipStore;
	if (ClipXDoStore<=0 || ClipYDo<=0) return 1;

	if (dstHeight>0)	// von unten nach oben!
	{
		mirr_y=!mirr_y;
		ypos=dstHeight-1-ypos;
	}

	lineptr=dstDIB+ypos*dstWidth+xpos;
	if (mirr_y) dstWidth=-dstWidth;
	lineptr-=dstWidth;



	if (!mirr_x) do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy;

			case copy_imp:
				i-=copy_1-1;

			docopy:
				if (ClipXDo<=0)
				{
					srcptr+=i;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr+=i;
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					srcptr+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				h=i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				memcpy(lineofs,srcptr,i);
				lineofs+=i;
				srcptr+=h;
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill;

			case fill_imp:
				i-=fill_1-1;

			dofill:
				if (ClipXDo<=0)
				{
					srcptr++;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr++;
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				memset(lineofs,*srcptr++,i);
				lineofs+=i;
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans;

			case trans_imp:
				i-=trans_1-1;

			dotrans:
				if (ClipXDo<=0)	break;
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs+=i;
				break;

			case linend_b:
				i=*srcptr++;
				goto dolinend;

			case linend_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);

			dolinend:
				lineofs=lineptr+=dstWidth;
				ClipXDo=ClipXDoStore;
				ClipXSkip=ClipXSkipStore;
				if (ClipYSkip>0)
				{
					ClipYSkip--;
					srcptr+=i;
					break;
				}
				if (--ClipYDo==0) return 1;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
	else do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy_m;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy_m;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy_m;

			case copy_imp:
				i-=copy_1-1;

			docopy_m:
				if (ClipXDo<=0)
				{
					srcptr+=i;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr+=i;
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					srcptr+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				h=i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
			   	h-=i;
				while(i)
				{
					*lineofs--=*srcptr++;
					i--;
				}
				srcptr+=h;
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill_m;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill_m;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill_m;

			case fill_imp:
				i-=fill_1-1;

			dofill_m:
				if (ClipXDo<=0)
				{
					srcptr++;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr++;
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs-=i;
				memset(lineofs+1,*srcptr++,i);
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans_m;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans_m;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans_m;

			case trans_imp:
				i-=trans_1-1;

			dotrans_m:
				if (ClipXDo<=0)	break;
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs-=i;
				break;


			case linend_b:
				i=*srcptr++;
				goto dolinend_w;

			case linend_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);

			dolinend_w:
				lineofs=lineptr+=dstWidth;
				ClipXDo=ClipXDoStore;
				ClipXSkip=ClipXSkipStore;
				if (ClipYSkip>0)
				{
					ClipYSkip--;
					srcptr+=i;
					break;
				}
				if (--ClipYDo==0) return 1;
				break;


			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
}
__except(EXCEPTION_EXECUTE_HANDLER)
{
	return 0;
}
}


/********************************************************************/
/* DIBDisplaySprite													*/
/*																	*/
/* This function display a sprite                              		*/
/********************************************************************/
int DIBTranslateSprite(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		uint8_t *	sprbuf,					// sprite data
		int		index,					// sprite number
		const uint8_t *  xtable)					// translation table

{
int dstWidth=(lpBMI->biWidth+3)&-4;
int dstHeight=lpBMI->biHeight;

uint8_t * lineofs,* lineptr;
uint8_t * srcptr=sprbuf;
int i,j;

__try
{
	if ((j=*(int32_t*)srcptr)==FC('I','D','S','P'))
	{
		if (index>=*(int32_t*)(srcptr+4)) 
		{
			return 0;
		}
		srcptr+=*(int32_t*)(srcptr+4*index+8);
		j=*(int32_t*)srcptr;
	}
	if (j!=FC('P','K','S','W') && j!=FC('P','K','S','C') && j!=FC('P','K','X','W'))
	{
		return 0;
	}
	if (j==FC('P','K','X','W'))
	{
		if (srcptr[5]<8) return 0;
		xpos+=((short*)srcptr)[5]*(mirr_x?-1:1);
		ypos+=((short*)srcptr)[6]*(mirr_y?-1:1);
	}
	srcptr+=4;

	if (dstHeight>0)	// von unten nach oben!
	{
		mirr_y=!mirr_y;
		ypos=dstHeight-1-ypos;
	}

	lineptr=dstDIB+ypos*dstWidth+xpos;
	if (mirr_y) dstWidth=-dstWidth;
	lineptr-=dstWidth;


	if (!mirr_x) do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy;

			case copy_imp:
				i-=copy_1-1;

			docopy:
				while (i--) *lineofs++=xtable[*srcptr++];
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill;

			case fill_imp:
				i-=fill_1-1;

			dofill:
				memset(lineofs,xtable[*srcptr++],i);
				lineofs+=i;
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans;

			case trans_imp:
				i-=trans_1-1;

			dotrans:
				lineofs+=i;
				break;


			case linend_b:
				srcptr--;
			case linend_w:
				srcptr+=2;
				lineofs=lineptr+=dstWidth;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
	else do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy_m;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy_m;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy_m;

			case copy_imp:
				i-=copy_1-1;

			docopy_m:
				while (i--) *lineofs--=xtable[*srcptr++];
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill_m;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill_m;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill_m;

			case fill_imp:
				i-=fill_1-1;

			dofill_m:
				lineofs-=i;
				memset(lineofs+1,xtable[*srcptr++],i);
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans_m;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans_m;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans_m;

			case trans_imp:
				i-=trans_1-1;

			dotrans_m:
				lineofs-=i;
				break;


			case linend_b:
				srcptr--;
			case linend_w:
				srcptr+=2;
				lineofs=lineptr+=dstWidth;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
}
__except(EXCEPTION_EXECUTE_HANDLER)
{
	return 0;
}
}


/********************************************************************/
/* DIBDisplaySpriteEx												*/
/*																	*/
/* This function display a sprite and clips it into a clipping 		*/
/* rectangle     													*/
/********************************************************************/
int DIBTranslateSpriteEx(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		int		clip_left,				// clipping rectangle
		int		clip_top,
		int		clip_right,
		int		clip_bottom,
		uint8_t *	sprbuf,					// sprite data
		int		index,					// sprite number
		const uint8_t * xtable)

{
uint8_t * lineofs, * lineptr;
uint8_t * srcptr=sprbuf;
int ClipXDo,ClipYDo;
int ClipXSkip,ClipYSkip;
int ClipXDoStore,ClipXSkipStore;
int dstWidth=(lpBMI->biWidth+3)&-4;
int dstHeight=lpBMI->biHeight;
int i,j,h;

__try
{
	if ((j=*(int32_t*)srcptr)==0x50534449)
	{
		if (index>=*(int32_t*)(srcptr+4)) 
		{
			return 0;
		}
		srcptr+=*(int32_t*)(srcptr+4*index+8);
		j=*(int32_t*)srcptr;
	}
	if (j!=FC('P','K','S','W') && j!=FC('P','K','S','C') && j!=FC('P','K','X','W'))
	{
		return 0;
	}
	if (j==FC('P','K','X','W'))
	{
		if (srcptr[5]<8) return 0;
		xpos+=((short*)srcptr)[5]*(mirr_x?-1:1);
		ypos+=((short*)srcptr)[6]*(mirr_y?-1:1);
	}
	srcptr+=4;

	ClipYDo=clip_bottom-clip_top+1;
	ClipYSkip=mirr_y? ypos-clip_bottom:clip_top-ypos;
	if (ClipYSkip<0) ClipYDo+=ClipYSkip;
	ClipXDoStore=clip_right-clip_left+1;
	ClipXSkipStore=mirr_x? xpos-clip_right:clip_left-xpos;
	if (ClipXSkipStore<0) ClipXDoStore+=ClipXSkipStore;
	if (ClipXDoStore<=0 || ClipYDo<=0) return 1;

	if (dstHeight>0)	// von unten nach oben!
	{
		mirr_y=!mirr_y;
		ypos=dstHeight-1-ypos;
	}

	lineptr=dstDIB+ypos*dstWidth+xpos;
	if (mirr_y) dstWidth=-dstWidth;
	lineptr-=dstWidth;



	if (!mirr_x) do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy;

			case copy_imp:
				i-=copy_1-1;

			docopy:
				if (ClipXDo<=0)
				{
					srcptr+=i;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr+=i;
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					srcptr+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				h=i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				h-=i;
				while (i--) *lineofs++=xtable[*srcptr++];
				srcptr+=h;
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill;

			case fill_imp:
				i-=fill_1-1;

			dofill:
				if (ClipXDo<=0)
				{
					srcptr++;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr++;
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				memset(lineofs,xtable[*srcptr++],i);
				lineofs+=i;
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans;

			case trans_imp:
				i-=trans_1-1;

			dotrans:
				if (ClipXDo<=0)	break;
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						lineofs+=i;
						ClipXSkip=j;
						break;
					}
					lineofs+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs+=i;
				break;

			case linend_b:
				i=*srcptr++;
				goto dolinend;

			case linend_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);

			dolinend:
				lineofs=lineptr+=dstWidth;
				ClipXDo=ClipXDoStore;
				ClipXSkip=ClipXSkipStore;
				if (ClipYSkip>0)
				{
					ClipYSkip--;
					srcptr+=i;
					break;
				}
				if (--ClipYDo==0) return 1;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
	else do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy_m;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy_m;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy_m;

			case copy_imp:
				i-=copy_1-1;

			docopy_m:
				if (ClipXDo<=0)
				{
					srcptr+=i;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr+=i;
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					srcptr+=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				h=i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
			   	h-=i;
				while(i)
				{
					*lineofs--=xtable[*srcptr++];
					i--;
				}
				srcptr+=h;
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill_m;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill_m;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill_m;

			case fill_imp:
				i-=fill_1-1;

			dofill_m:
				if (ClipXDo<=0)
				{
					srcptr++;
					break;
				}
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						srcptr++;
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs-=i;
				memset(lineofs+1,xtable[*srcptr++],i);
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans_m;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans_m;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans_m;

			case trans_imp:
				i-=trans_1-1;

			dotrans_m:
				if (ClipXDo<=0)	break;
				if (ClipXSkip>0)
				{
					j=ClipXSkip-i;
					if (j>=0)
					{
						lineofs-=i;
						ClipXSkip=j;
						break;
					}
					lineofs-=ClipXSkip;
					ClipXSkip=0;
					i=-j;
				}
				j=ClipXDo-i;
				if (j<=0)
				{
					i=ClipXDo;
					ClipXDo=0;
				}
				else
				{
					ClipXDo=j;
				}
				lineofs-=i;
				break;


			case linend_b:
				i=*srcptr++;
				goto dolinend_w;

			case linend_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);

			dolinend_w:
				lineofs=lineptr+=dstWidth;
				ClipXDo=ClipXDoStore;
				ClipXSkip=ClipXSkipStore;
				if (ClipYSkip>0)
				{
					ClipYSkip--;
					srcptr+=i;
					break;
				}
				if (--ClipYDo==0) return 1;
				break;


			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
}
__except(EXCEPTION_EXECUTE_HANDLER)
{
	return 0;
}
}


/********************************************************************/
/* DIBDisplaySprite													*/
/*																	*/
/* This function display a sprite                              		*/
/********************************************************************/
int DIBMaskSprite(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		uint8_t *	sprbuf,					// sprite data
		int		index,					// sprite number
		int		maskval)

{
int dstWidth=(lpBMI->biWidth+3)&-4;
int dstHeight=lpBMI->biHeight;

uint8_t * lineofs,* lineptr;
uint8_t * srcptr=sprbuf;
int i,j;

__try
{
	if ((j=*(int32_t*)srcptr)==FC('I','D','S','P'))
	{
		if (index>=*(int32_t*)(srcptr+4)) 
		{
			return 0;
		}
		srcptr+=*(int32_t*)(srcptr+4*index+8);
		j=*(int32_t*)srcptr;
	}
	if (j!=FC('P','K','S','W') && j!=FC('P','K','S','C') && j!=FC('P','K','X','W'))
	{
		return 0;
	}
	srcptr+=4;
	if (j==FC('P','K','X','W'))
	{
		if (*srcptr==header2 || srcptr[1]<12) return 0;
		xpos+=((short*)srcptr)[3]*(mirr_x?-1:1);
		ypos+=((short*)srcptr)[4]*(mirr_y?-1:1);
		srcptr+=2+srcptr[1];
	}

	if (dstHeight>0)	// von unten nach oben!
	{
		mirr_y=!mirr_y;
		ypos=dstHeight-1-ypos;
	}

	lineptr=dstDIB+ypos*dstWidth+xpos;
	if (mirr_y) dstWidth=-dstWidth;
	lineptr-=dstWidth;


	if (!mirr_x) do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy;

			case copy_imp:
				i-=copy_1-1;

			docopy:
				memset(lineofs,maskval,i);
				lineofs+=i;
				srcptr+=i;
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill;

			case fill_imp:
				i-=fill_1-1;

			dofill:
				memset(lineofs,maskval,i);
				srcptr++;
				lineofs+=i;
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans;

			case trans_imp:
				i-=trans_1-1;

			dotrans:
				lineofs+=i;
				break;


			case linend_b:
				srcptr--;
			case linend_w:
				srcptr+=2;
				lineofs=lineptr+=dstWidth;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
	else do
	{
		switch(indextab[i=*srcptr++])
		{

			case copy:
				i=80+ *srcptr++;
				goto docopy_m;

			case copy_p_336:
				i=336+ *srcptr++;
				goto docopy_m;

			case copy_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto docopy_m;

			case copy_imp:
				i-=copy_1-1;

			docopy_m:
				srcptr+=i;
				lineofs-=i;
				memset(lineofs,maskval,i);
				break;


			case fill:
				i=80+ *srcptr++;
				goto dofill_m;

			case fill_p_336:
				i=336+ *srcptr++;
				goto dofill_m;

			case fill_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dofill_m;

			case fill_imp:
				i-=fill_1-1;

			dofill_m:
				lineofs-=i;
				memset(lineofs+1,maskval,i);
				srcptr++;
				break;


			case trans:
				i=80+ *srcptr++;
				goto dotrans_m;

			case trans_p_336:
				i=336+ *srcptr++;
				goto dotrans_m;

			case trans_w:
				i=*((WORD*)srcptr);
				srcptr+=sizeof(WORD);
				goto dotrans_m;

			case trans_imp:
				i-=trans_1-1;

			dotrans_m:
				lineofs-=i;
				break;


			case linend_b:
				srcptr--;
			case linend_w:
				srcptr+=2;
				lineofs=lineptr+=dstWidth;
				break;

			case header2:
				srcptr+=1+*srcptr;
				break;

			case palette:
				srcptr+=2+*(WORD*)srcptr;
				break;

			case ende:
				return 1;
		}
	}
	while(1);
}
__except(EXCEPTION_EXECUTE_HANDLER)
{
	return 0;
}
}



/********************************************************************/
/* DIBCopyBlock     												*/
/*																	*/
/* This function copies a block from one DIB to another   			*/
/*																	*/
/********************************************************************/
int DIBCopyBlock(
		uint8_t * 	dstDIB,
		LPBITMAPINFOHEADER lpbmiDST,
		int		dxpos,
		int		dypos,
		int		xsize,
		int		ysize,
		uint8_t * 	srcDIB,
		LPBITMAPINFOHEADER lpbmiSRC,
		int		sxpos,
		int		sypos
)
{
int dstWidth=(lpbmiDST->biWidth+3)&-4;
int dstHeight=lpbmiDST->biHeight;
int srcWidth=(lpbmiSRC->biWidth+3)&-4;
int srcHeight=lpbmiSRC->biHeight;

uint8_t *dlineofs,*slineofs;
int mirr_s,mirr_d,mirr_x;


	mirr_s=0;
	mirr_d=ysize<0;
	ysize=abs(ysize);
	mirr_x=xsize<0;
	xsize=abs(xsize);

	if (dstHeight>0)
	{
		mirr_d=!mirr_d;
		dypos=dstHeight-1-dypos;
	}
	if (srcHeight>0)
	{
		mirr_s=1;
		sypos=srcHeight-1-sypos;
	}

	dlineofs=dstDIB +dxpos+dstWidth*dypos;
	slineofs=srcDIB +sxpos+srcWidth*sypos;

	if (mirr_d) dstWidth=-dstWidth;
	if (mirr_s) srcWidth=-srcWidth;

	__try
	{
		if (mirr_x==0)
		{
			while(ysize)
			{
				memcpy(dlineofs,slineofs,xsize);
				dlineofs+=dstWidth;
				slineofs+=srcWidth;
				ysize--;
			}
		}
		else
		{
			while(ysize)
			{
			int xs=xsize;

				while(xs)
				{
					*dlineofs--=*slineofs++;
					xs--;
				}
				dlineofs+=dstWidth+xsize;
				slineofs+=srcWidth-xsize;
				ysize--;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}
	return TRUE;
}


/********************************************************************/
/* DIBSetBlock	    												*/
/*																	*/
/* This function fills a block in a DIB with a given color			*/
/*																	*/
/********************************************************************/
int DIBSetBlock(
		uint8_t * 	dstDIB,
		LPBITMAPINFOHEADER lpbmiDST,
		int		dxpos,
		int		dypos,
		int		xsize,
		int		ysize,
		int		fillval
)
{
int dstWidth=(lpbmiDST->biWidth+3)&-4;
int dstHeight=lpbmiDST->biHeight;

uint8_t * dlineofs;
int mirr_d;


	mirr_d=ysize<0;
	ysize=abs(ysize);
	if (xsize<0) dxpos+=xsize+1;
	xsize=abs(xsize);

	if (dstHeight>0)
	{
		mirr_d=!mirr_d;
		dypos=dstHeight-1-dypos;
	}

	dlineofs=dstDIB+dxpos+dstWidth*dypos;

	if (mirr_d) dstWidth=-dstWidth;

	__try
	{
		while(ysize)
		{
			memset(dlineofs,fillval,xsize);
			ysize--;
			dlineofs+=dstWidth;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}
	return TRUE;
}
