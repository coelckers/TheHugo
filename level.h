
struct sprite
{
	int xpos,ypos;
	int direction;
	int present,off;
	union
	{
		struct
		{
			int lorand,rurand;
		};
		struct
		{
			int sx,sy,ex,ey;
			int xadd,yadd;
			int lenX,lenY;
		};
		struct
		{
			int mx,my;
			int radx,rady;
			double angle;
		};
	};
} ;


class level
{
struct ball
{
	int b_pos,b_xp,b_yp;
	int b_ani;
	int b_color;
	int b_oldppos;
	int b_wait;
} ;

struct mine
{
	int bspos;
	int counter;
};


	charset * theCharSet;
	const uint8_t * lpsprBall;

	int currentLevel;
	int numLevels;
	BOOL prepared;

	uint8_t rawlevbuf[0x660];

	uint8_t screenbuf[0x300];
	uint8_t colbuf[0x300];
	uint8_t colblink[0x300];
	
	sprite sprites[7];
	ball wballs[8],wballbak[8];
	ball sballs[8],sballbak[8];
	mine mintab[48];
	int balldelay;
	int off1,off2;

	int blink;
	int boulder;

	int bomb_bs,bomb_count;

	int fillct,items,fackeln,items2,fack2,bombs,sbalct,wbalct;
	//char offspr[7];

	void PrepareSprites();
	void MoveBalls();
	void MoveSprites();


public:

	level(charset*);

	BOOL Load(int no);
	void Save(int no);
	int GetLevelCount() { return numLevels; }

	void PrepareGame();
	void PrepareEditor(); 

	void Animate(int);
	void Draw(DIB &,BOOL sprites=TRUE); 
	void ClearHints();

	int GetTime() { return rawlevbuf[0x600]+256*rawlevbuf[0x602]; }
	void SetTime(int t) { rawlevbuf[0x600]=LOBYTE(t); rawlevbuf[0x602]=HIBYTE(t); }
	int GetSpecials() { return rawlevbuf[0x601]&-2; }
	void SetSpecial(int val) { rawlevbuf[0x601]=val; }
	//...

	void SpritesOn();
	void RstFahr();
	void RstMin();
	void SpriteOff(int no);
	BOOL IsSpriteOff(int no) { return sprites[no].off; }
	void Klapp();
	void ExWall();
	void Rebuild();
	int  SetSet2();
	void ClrMinTab() { memset(mintab,0,sizeof(mintab)); }
	void DoBlink();
	void DoBlack(int);

	uint8_t SB(int i) { return screenbuf[i]; }
	uint8_t CB(int i) { return colbuf[i]; }
	void Set(int pos,int c) { screenbuf[pos]=c; }
	void SetCol(int pos,int c,int col) { screenbuf[pos]=c; colbuf[pos]=col; }
	void Clear(int pos) { screenbuf[pos]=32; }
	BOOL SetBomb(int pos);
	void KillBomb() { bomb_count=0; }
	void DoBomb();
	void ZuendMine(int bspos);
	void DoMine();

	
	int GetBombs() { return bombs; }
	void AddBombs(int add) { bombs+=add; }
	int GetItems() { return items; }
	void AddItems(int add) { items+=add; }
	int GetFill() { return fillct; }
	void AddFill(int add) { fillct+=add; }
	int GetFack() { return fackeln; }
	void AddFack(int add) { fackeln+=add; }
	int GetItems2() { return items2+fack2; }

	int Completed() { return !(bombs+items+fillct+fackeln+items2+fack2); }

	void DrawCollisionSprites(DIB & cd,int x,int y);


	void DrawSpriteTracks(HDC hDC);
	void CreateEmptyLevel();
	void SetFrameColor(int);
	void SetSprite(int no,sprite * sp)
	{
		sprites[no]=*sp;
	}		

	BOOL BoulderDash(int);
};

