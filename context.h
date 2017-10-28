


class Context
{
protected:

	Context() {}

public:

	virtual void Loop(int count) { WaitMessage(); }

	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags) {};
	virtual void OnChar(HWND,UINT,int) {}
	virtual void OnLButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags) {};
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {};
	virtual void OnRButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags) {};
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags) {};
	virtual void OnMouseMove(HWND,int x,int y,UINT) {};
	virtual void OnKBMouseEvent(int,int) {};

	virtual void Activate() { Draw(); }
	virtual void Deactivate() {};
	virtual void Draw() {};
};