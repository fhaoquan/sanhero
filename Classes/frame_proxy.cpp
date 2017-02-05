#include "frame_proxy.h"
#include "frame.h"

FrameWnd* pFrameWnd = NULL;
void InstallFrameMessageHanlder(std::function<int(unsigned int, unsigned int)> fn)
{
	HWND hwnd = FindWindowAtProcess();
	pFrameWnd = new FrameWnd();
	pFrameWnd->SetFn(fn);
	pFrameWnd->SubclassWindow(hwnd);
}

void UnInstallFrameMessageHanlder()
{
	pFrameWnd->UnsubclassWindow();
}
