#include "Soilder.h"
#include "../Utils/Config.h"

Soilder* Soilder::create(int soilderID, Vec2 pos)
{
    Soilder *pRet = new(std::nothrow) Soilder();
    if (pRet && pRet->init(soilderID, pos)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}


bool Soilder::init(int soilderID, Vec2 pos)
{
    if ( !Sprite::init() ) {
        return false;
    }

	_id = soilderID;
	_type = soilderID;
    _pos = pos;
	_pTarget = NULL;
	_type = 2;

    loadData();
    showUI();
    addHPBar();
	Hero::addTouch();
    
    return true;
}


void Soilder::loadData()
{
	Hero::loadData();
	setName("soilder");
}


void Soilder::showUI()
{
    //switch (_type) {
    //    case SOILDER_TYPE_FIGHTER: {
    //        _arm = Armature::create(ANIM_NAME_FIGHTER);
    //    }
    //        break;
    //    case SOILDER_TYPE_BOWMAN: {
    //        _arm = Armature::create(ANIM_NAME_BOWMAN);
    //    }
    //        break;
    //    case SOILDER_TYPE_GUNNER: {
    //        _arm = Armature::create(ANIM_NAME_GUNNER);
    //    }
    //        break;
    //    case SOILDER_TYPE_MEAT: {
    //        _arm = Armature::create(ANIM_NAME_MEATSHIELD);
    //    }
    //        break;
    //    default:
    //        break;
    //}
	loadArm();
	setContentSize(_arm->getContentSize());
    this->addChild(_arm);
    
    this->setPosition(_pos);
    this->setLocalZOrder((int)_pos.x + (int)_pos.y);
    
    //this->setScale(0.8f);
	setSelect(false);

	setPhysicsBody();
}


void Soilder::addHPBar()
{
    auto bg = Sprite::create(IMG_BUILD_PRO_BK);
    _hpBar = LoadingBar::create(IMG_BUILD_PRO);
    bg->setPosition(0, _arm->getContentSize().height/2 + 10);
    _hpBar->setPosition(bg->getContentSize()/2);
    bg->addChild(_hpBar);
    this->addChild(bg, 9, "Bar");
    
    bg->setScale(0.7f);
    _hpBar->setPercent(100.0 * _healthPoint / _totalHP);
}





void Soilder::showMe()
{

}

// cocos×ø±ê
Vec2 Soilder::getCenter()
{
	return getPosition() - Vec2(14, 14);
}

void Soilder::loadArm()
{
	_arm = Armature::create(ANIM_NAME_FIGHTER);
}




