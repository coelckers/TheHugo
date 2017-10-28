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
		int		index);					// sprite number

int DIBTranslateSprite(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		uint8_t *	sprbuf,					// sprite data
		int		index,					// sprite number
		const uint8_t *  xtable);					// translation table

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
		const uint8_t * xtable);

int DIBMaskSprite(
		uint8_t *	dstDIB,					// destination DIB
		LPBITMAPINFOHEADER lpBMI,		// destination BITMAPINFOHEADER
		int		xpos,					// x-position
		int		ypos,					// y-position
		int		mirr_x,					// x-mirror flag
		int		mirr_y,					// y-mirror flag
		uint8_t *	sprbuf,					// sprite data
		int		index,					// sprite number
		int		maskval);

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
);

int DIBSetBlock(
		uint8_t * 	dstDIB,
		LPBITMAPINFOHEADER lpbmiDST,
		int		dxpos,
		int		dypos,
		int		xsize,
		int		ysize,
		int		fillval
);
