#include "BulletSprite.h"
#include <string>
using namespace std;

BulletSprite* BulletSprite::create(Vec2 src, Vec2 des, int damage, ITarget* target, IAttack* pAttack, string img)
{
    BulletSprite *pRet = new(std::nothrow) BulletSprite();
    if (pRet && pRet->init(src, des, damage, target, pAttack, img)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}


bool BulletSprite::init(Vec2 src, Vec2 des, int damage, ITarget* target, IAttack* pAttack, string img)
{
    if ( !Sprite::init() ) {
        return false;
    }
    
    _src = src;
    _des = des;
    _damage = damage;
    _target = target;
	_pAttack = pAttack;
    _img = img;
	    
    showUI();
    
    return true;
}


void BulletSprite::showUI()
{
	//addChild(Sprite::create("army1_1.png"));
    this->setTexture(_img);
    this->setPosition(_src);
    this->setScale(0.7f);

	auto delta = _des;
    //auto delta = _des - _src;
    auto ang = atan2(delta.y, delta.x) * 180 / acos(-1.0);
    ang = 360 - ang;
    this->setRotation(ang);
	//setLocalZOrder(100);

    auto move = MoveBy::create(2.0f, delta);
    //auto func = CallFunc::create(CC_CALLBACK_0(BulletSprite::atk, this));
    //this->runAction(Sequence::create(move, func, nullptr));
	runAction(move);
	scheduleOnce(schedule_selector(BulletSprite::arrowClose), 2.0f);
}


void BulletSprite::arrowClose(float dt)
{
    //_target->hurt(_damage);
	_target->GetAction()->hurt(_pAttack);
    this->removeFromParent();
}

