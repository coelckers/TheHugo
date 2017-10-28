class GameOver : public Context
{
public:

	GameOver() {}
	~GameOver() {}

	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	virtual void Draw();

};

