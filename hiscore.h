
class Hiscore : public Context
{
struct hientry
{
	int score;
	int level;
	char name[24];
};

	hientry hlist[10];

public:

	Hiscore();
	~Hiscore();

	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	virtual void Draw();

	BOOL CheckScore(int score,int level);
	void InsertScore(int score,int level,char * name);
	void Save();
};


class HiscoreIn : public Context
{

	char entername[24];
	int enterpos;
	int score,level;
	Hiscore * hitab;

public:

	HiscoreIn(Hiscore*);
	~HiscoreIn();

	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnChar(HWND,UINT,int);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	virtual void Activate();
	virtual void Draw();

	void SetScore(int scor,int lev) { score=scor; level=lev; }
};


