
#define GOS_WAITING 1
#define GOS_PLAYING 2
#define GOS_FINISHED 3
#define GOS_FADEIN 4
#define GOS_FADEOUT 5
#define GOS_ENTERPIPEUP 6
#define GOS_ENTERPIPEDOWN 7
#define GOS_EXITPIPEUP 8
#define GOS_EXITPIPEDOWN 9
#define GOS_TRACKPIPE 10
#define GOS_DEAD 11
#define GOS_TELEPORT 12
#define GOS_OPENDOOR 13

class Game : public Context
{
	int levno;
	int startat0;
	int score;
	int lives;
	int time;
	int timer;
	int bombbs;
	int scormult;
	int telep;
	int tank;
	int torschl;
	int hsl;
	int bombflg;
	int exm;
	int bombsused;
	int opendoor;
	int push;

	int telepos;

	int bspos;
	int bsp_x,bsp_y;
	int robobs;
	int rob_x,rob_y;
	int robi2;

	int pipe_x,pipe_y;
	int trackpipe;

	int hugodir;
	int blend;
	int dead;

	uint8_t hugocol[4];


	int isMoving;
	int mv_x,mv_y;
	int plMove;
	int JumpCount;

	int upflag,downflag,leftflag,rightflag;
	int feuer;
	int fall,dofahr,exverz;
	int moveflag;

	level * theLevel;
	charset * theCharSet;

	const uint8_t * lpsprHugo;
	const uint8_t * lpsprText;
	const uint8_t * lpsprBall;
	const uint8_t * lpsprBlend;

	DIBSect CollChk;

	int GameObjectStatus,NextGOS;


	int joyid;
	JOYCAPS jcaps;

	void PutHugo(DIB & ddd);
	// Spielroutinen
	int ChkOb();
	int ChkMid();
	int ChkUn();
	int ChkLeft();
	int ChkRight();
	int ChkUnJp();
	void Home();
	void Check(int checkmask=15);
	void TrackPipe(int);
	void OpenDoor();
	BOOL CollisionCheck();
	void StatusField(BOOL fCreate=FALSE);


	void Move(int x,int y,int pl=0)
	{
		isMoving=8;
		mv_x=x;
		mv_y=y;
		plMove=pl;
		moveflag=0;
	}

	void Dead();

	void FadeOut(int next)
	{
		blend=11;
		NextGOS=next;
		GameObjectStatus=GOS_FADEOUT;
	}

	void FadeIn(int next)
	{
		blend=0;
		NextGOS=next;
		GameObjectStatus=GOS_FADEIN;
	}

	void EnterPipeUp()
	{
		GameObjectStatus=GOS_ENTERPIPEUP;
		pipe_y=bsp_y-8;
		bspos-=80;
		trackpipe=-40;
		blend=0;
	}

	void EnterPipeDown()
	{
		GameObjectStatus=GOS_ENTERPIPEDOWN;
		pipe_y=bsp_y-7;
		bspos+=40;
		trackpipe=40;
		blend=0;
	}

	void ExitPipeDown()
	{
		GameObjectStatus=GOS_EXITPIPEDOWN;
		pipe_y=bsp_y+8;
		blend=0;
	}

	void ExitPipeUp()
	{
		GameObjectStatus=GOS_EXITPIPEUP;
		pipe_y=bsp_y-23;
		blend=0;
	}

	void clrpos() ;

	void AddScore(int add)
	{
		if (time) score+=scormult*add;
	}


	void ReadControls();
	BOOL Left();
	BOOL Right();

public:

	Game(level *,charset *);
	~Game();


	void StartGame(int level);
	BOOL StartLevel();
	void EndLevel(BOOL);
	void DrawLevel();


	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);

	virtual void Activate();
	virtual void Draw();
	virtual void Loop(int count);


};