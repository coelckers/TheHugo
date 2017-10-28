
class EnterLevNo : public Context
{
protected:

	char entername[5];
	int enterpos;
	int maxlevel;
	int action;

public:

	EnterLevNo(int);
	~EnterLevNo();

	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnChar(HWND,UINT,int);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	virtual void Activate();
	virtual void Draw();

	void SetAction(int act) { action=act; }
};


class EnterTime: public EnterLevNo
{

public:

	EnterTime();
	~EnterTime();

	virtual void Draw();
};


