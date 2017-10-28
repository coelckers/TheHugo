
class Editor : public Context
{
	int value;
	int levno;
	BOOL changed;

	level * theLevel;
	charset * theCharset;

	LPBITMAPFILEHEADER lpbfMenubar;
	LPBITMAPINFOHEADER lpbiMenubar;
	uint8_t *lpbitsMenubar;

	int currentSelection;
	int currentColor;

	int mou_X,mou_Y;
	const uint8_t * mou_P;
	const uint8_t * lpsprBall;

	int action;
	const uint8_t * cSelect;
	int extSelect;

	int EditSprite;
	int EditSpriteMode;

	POINT sprP[3];


	void CreateSprite();

public:

	Editor(level*,charset*);
	~Editor();


	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnLButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags);
	virtual void OnRButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseMove(HWND,int x,int y,UINT);

	virtual void Activate();
	virtual void Deactivate();
	virtual void Draw();

	virtual void Loop(int);


	void SetLevel(int no) { value=no; }

};
