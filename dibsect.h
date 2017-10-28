
extern LPBITMAPFILEHEADER lpbfPal;

class DIB
{
protected:

	uint8_t *lpBits;
	long absHeight;		// absolute height
	LPBITMAPINFOHEADER lpHeader;

public:


	// Width of DIB
	long Width()
	{
		return Header()->biWidth;
	};

	// Height of DIB
	long Height()
	{
		return absHeight;
	};

	long ImageSize()
	{
		return ((Width()+3)&-4)*Height();
	}

	// pointer to BITMAPINFOHEADER of DIB
	LPBITMAPINFOHEADER Header()
	{
		return lpHeader;
	};

	// pointer to BITMAPINFO of DIB
	LPBITMAPINFO Info()
	{
		return (LPBITMAPINFO)lpHeader;
	}

	// pointer to image of DIB
	uint8_t * Bits()
	{
		return lpBits;
	};

	void Clear(int col=0)
	{
		memset(Bits(),col,ImageSize());
	};

	// draw a sprite onto DIB
	int MaskSprite(int x,int y,int mx,int my,const uint8_t * modules,int no,int ci)
	{
		return DIBMaskSprite(Bits(),Header(),x,y,mx,my,(uint8_t*)modules,no,ci);
	};

	int TranslateSprite(int x,int y,int mx,int my,const uint8_t * modules,int no,const uint8_t * ci)
	{
		return DIBTranslateSprite(Bits(),Header(),x,y,mx,my,(uint8_t*)modules,no,(uint8_t*)ci);
	};

	// draw a sprite onto DIB into a clipping rectangle
	int DisplaySpriteEx(int x,int y,int mx,int my,int cl,int ct,int cr,int cb,
						const uint8_t * mod,int no)
	{
		return DIBDisplaySpriteEx(Bits(),Header(),x,y,mx,my,cl,ct,cr,cb,(uint8_t*)mod,no);
	};

	// draw a sprite onto DIB into a clipping rectangle
	int DisplaySpriteEx(int x,int y,int mx,int my,RECT & clip,const uint8_t * mod,int no)
	{
		return DIBDisplaySpriteEx(Bits(),Header(),x,y,mx,my,
			clip.left,clip.top,clip.right,clip.bottom,(uint8_t*)mod,no);
	};

	int TranslateSpriteEx(int x,int y,int mx,int my,int cl,int ct,int cr,int cb,
						const uint8_t * mod,int no,const uint8_t * ci)
	{
		return DIBTranslateSpriteEx(Bits(),Header(),x,y,mx,my,cl,ct,cr,cb,(uint8_t*)mod,no,(uint8_t*)ci);
	};

	// copy a block from another DIB onto this one
	void CopyBlock(int dx,int dy,int sizx,int sizy,DIB& source,int sx,int sy)
	{
		DIBCopyBlock(Bits(),Header(),dx,dy,sizx,sizy,
				source.Bits(),source.Header(),sx,sy);
	};

	// copy a block from another DIB onto this one
	void CopyBlock(int dx,int dy,int sizx,int sizy,uint8_t *lpsBits,LPBITMAPINFOHEADER lpsH,int sx,int sy)
	{
		DIBCopyBlock(Bits(),Header(),dx,dy,sizx,sizy,lpsBits,lpsH,sx,sy);
	};

	// clear a part of the DIB
	void SetBlock(int x,int y,int sizx,int sizy,int fill=0)
	{
		DIBSetBlock(Bits(),Header(),x,y,sizx,sizy,fill);
	};
};

class DIBSect: public DIB
{
public:

#pragma pack(1)
struct BMPHEADER
{
	BITMAPINFOHEADER bi;
	RGBQUAD	pal[256];
};
#pragma pack()

protected:
	BMPHEADER buffer;

	DIBSect(bool)
	{
	}
public:

	DIBSect(int x=1,int y=1)
	{
	long xa=(x+3)&-4;

		buffer.bi.biWidth=x;
		buffer.bi.biHeight=y; 
		absHeight=y;
		lpHeader=&buffer.bi;
		lpBits=(uint8_t*)malloc(xa*y);
		memset(lpBits,0,y*xa);
	}

	~DIBSect() { if (lpBits) free(lpBits); }
};

struct DIBSection : public DIBSect
{
	HBITMAP hBmp;
	HBITMAP dcBitmap;

	DIBSection()
	{
		hBmp = (HBITMAP)INVALID_HANDLE_VALUE;
		lpBits = NULL;
	}
	void Create(int w, int h)
	{
		buffer.bi.biSize = sizeof(BITMAPINFOHEADER);
		buffer.bi.biWidth = w;
		buffer.bi.biHeight = -h;
		buffer.bi.biPlanes = 1;
		buffer.bi.biBitCount = 8;
		buffer.bi.biCompression = BI_RGB;
		buffer.bi.biSizeImage = 0;
		buffer.bi.biXPelsPerMeter = 100000;
		buffer.bi.biYPelsPerMeter = 100000;
		buffer.bi.biClrUsed = 0;
		buffer.bi.biClrImportant = 0;
		LPBITMAPINFO lpbi = (LPBITMAPINFO)(lpbfPal + 1);
		memcpy(buffer.pal, lpbi->bmiColors, 1024);

		HDC dc = ::GetDC(NULL);
		hBmp = CreateDIBSection(dc, (BITMAPINFO*)&buffer, DIB_RGB_COLORS, (void**)&lpBits, NULL, 0);
		::ReleaseDC(NULL, dc);
		absHeight = h;
	}

	~DIBSection()
	{
		DeleteObject(hBmp);
		lpBits = NULL;
	}

	HDC GetDC()
	{
		HDC dc = ::GetDC(NULL);
		HDC cdc = CreateCompatibleDC(dc);
		::ReleaseDC(NULL, dc);
		dcBitmap = SelectBitmap(cdc, hBmp);
		return cdc;
	}

	void ReleaseDC(HDC dc)
	{
		SelectBitmap(dc, dcBitmap);
		DeleteDC(dc);
	}
};

void InitObj();
