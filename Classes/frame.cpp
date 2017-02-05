#include "frame.h"
LRESULT FrameWnd::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	RECT rc;
	this->GetClientRect(&rc);
	pt.y = rc.bottom - pt.y;
	int nType = _fn(pt.x, pt.y);
	if (nType>=0) {
		bHandled = true;
		_cursor = SetCursor(_cursor2[nType]);
	}
	else {
		if (_cursor != NULL)
			SetCursor(_cursor);
	}
	return 0;
}

void FrameWnd::SetFn(std::function<int(unsigned int, unsigned int)>& fn)
{
	_cursor = NULL;
	_cursor2[0] = LoadCursorFromFileA(("Work_Gea.cur"));	// me
	_cursor2[1] = LoadCursorFromFileA(("Altr_Fsh.cur"));	// friend
	_cursor2[2] = LoadCursorFromFileA(("unav_mox.cur"));	// enemy

	_fn = fn;
}

HWND FindWindowAtProcess()
{
	return FindWindowExA(NULL, NULL, "GLFW30", "sanhero");
}
