#ifndef __BulletSprite_H__
#define __BulletSprite_H__

#include "Classes/Utils/stdafx.h"
//#include "BaseSprite.h"
#include "src/c_sprite_action.h"

class BulletSprite : public cocos2d::Sprite
{
public:
	virtual bool init(Vec2 src, Vec2 des, int damage, ITarget* target, IAttack* pAttack, std::string img);
	static BulletSprite* create(Vec2 src, Vec2 des, int damage, ITarget* target, IAttack* pAttack, std::string img);

public:
    void showUI();
    void arrowClose(float dt);
public:
    Vec2 _src;
    Vec2 _des;
    int _damage;
    ITarget* _target;
	IAttack* _pAttack;
    std::string _img;
};

#endif // __BulletSprite_H__
