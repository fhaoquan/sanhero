#pragma once

#include <atlbase.h>
#include <atlwin.h>
#include <functional>

class FrameWnd : public CWindowImpl<FrameWnd>
{
public:
	std::function<int(unsigned int, unsigned int)> _fn;
	void SetFn(std::function<int(unsigned int, unsigned int)>& fn);
	BEGIN_MSG_MAP(FrameWnd)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	END_MSG_MAP()
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HCURSOR _cursor;
	HCURSOR _cursor2[3];
};

HWND FindWindowAtProcess();