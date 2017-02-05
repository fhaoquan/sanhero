#ifndef __BaseSprite_H__
#define __BaseSprite_H__

#include "cocos2d.h"
USING_NS_CC;

class BaseSprite : public cocos2d::Sprite
{
public:
    virtual bool init();
    CREATE_FUNC(BaseSprite);
    BaseSprite() {}
    virtual ~BaseSprite() {}
	enum Camp {
		Me = 0,
		Friend,
		Enemy
	};
public:
	void setCamp(int camp);
	int getCamp();

public:
    Vec2 _pos;
	int _camp;

};


#endif // __BaseSprite_H__
