

class charset
{
	uint8_t *charbuffer;
	uint8_t *charptr;

	int charmap[256];
	uint8_t charflags[256];

	int blinkdelay;

	int exdelay;
	int exphase;

	int efelddelay;
	int efeldphase;

	int aniphase;
	int switchit;

	int flashdelay;


public:

	charset(char * fn);
	~charset();


	void PutSpace(uint8_t * lpBits,int width,int x,int y);

	void ClearFlags() { memset(charflags,0,256); }
	void Animate(int count);

	void PutChar(uint8_t * lpBits,int width,int x,int y,int c,int color);

	void DrawChar(uint8_t * lpBits,int width,int x,int y,int c,int color)
	{
		PutChar(lpBits,width,x,y,charmap[c],color);
	}

	void UpdateChar(uint8_t * lpBits,int width,int x,int y,int c,int color)
	{
		if (charflags[c]) PutChar(lpBits,width,x,y,charmap[c],color);
	}

	void SetSwitch() { switchit=TRUE; }
	BOOL Compare(int c,int map) { return charmap[c]==map; }
	int GetEStatus() { return efeldphase; }
	void ResetMFlash() { charmap[247]=292; charmap[249]=293; charmap[248]=charmap[250]=32; }
	void SetBombAni(int i) { charmap[31]=256+i; }
	void SetDoorAni(int i) { charmap[0]=412+i; }

};

