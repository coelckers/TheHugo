

class Menu : public Context
{
	int lastobj;
	int status;
	const uint8_t * sprptr;

public:

	Menu();
	~Menu();


	virtual void OnKey(HWND hwnd,UINT vk,BOOL fDown,int cRepeat,UINT flags);
	virtual void OnLButtonDown(HWND hw,BOOL fDbl,int x,int y,UINT keyFlags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseMove(HWND,int x,int y,UINT);

	//virtual void Activate() {};
	//virtual void Deactivate() {};
	virtual void Draw();
};