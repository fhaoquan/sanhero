#pragma once
#include "../Utils/stdafx.h"
class MySprite : public cocos2d::Sprite
{
public:
	virtual bool init();
	CREATE_FUNC(MySprite);
	MySprite() {}
	virtual ~MySprite() {}
	void update(float dt);
};


