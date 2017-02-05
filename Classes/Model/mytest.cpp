#include "mytest.h"

bool MySprite::init()
{
	if (!Sprite::init()) {
		return false;
	}

	schedule(schedule_selector(MySprite::update), 1);
	return true;
}

void MySprite::update(float dt)
{
	CCLOG("%f", dt);
}

