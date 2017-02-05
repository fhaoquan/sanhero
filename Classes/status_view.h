#ifndef __STATUS_VIEW_H__
#define __STATUS_VIEW_H__

#include "cocos2d.h"
using namespace cocos2d;
#include "Utils/stdafx.h"


class CStatusView : public Layer
{
public:
	CStatusView();
	virtual ~CStatusView();

	CREATE_FUNC(CStatusView);
};

class CHeroStatusView : public CStatusView
{
public:
	CHeroStatusView();
	virtual ~CHeroStatusView();
	virtual bool init();
	CREATE_FUNC(CHeroStatusView);

	void skillCallback(Ref* sender, Widget::TouchEventType type);
	void setMapLayer(Layer* layer);
private:
	Layer* _mapLayer;
};
#endif