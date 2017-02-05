#include "BowMan.h"
#include "../Utils/Config.h"
#include "Classes/Model/bulletsprite.h"
#include "HelloWorldScene.h"

bool CBowMan::init(int soilderID, Vec2 pos)
{
	if (!Soilder::init(soilderID, pos)) {
		return false;
	}
	_shootRange = 300;

	return true;
}

CBowMan* CBowMan::create(int soilderID, Vec2 pos)
{
	CBowMan *pRet = new(std::nothrow) CBowMan();
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

void CBowMan::loadArm()
{
	_arm = Armature::create(ANIM_NAME_BOWMAN);
}
extern int g_kCamera;

void CBowMan::atk(ITarget* pTarget[], int size_target)
{
	if (_isbroken == true || _healthPoint <= 0 || !isVisible()) {
		return;
	}

	for (int i = 0; i < size_target; i++) {
		float distance = getCenter().distance(pTarget[i]->getCenter());
		if (distance < _shootRange) {
			// 这里pos使用的是View坐标, 左下角为原点
			Vec2 pos = getCenter();
			Vec2 pos2 = pTarget[i]->getCenter();
			pos2 -= pos;
			auto camera = getParent()->getChildByTag(g_kCamera);
			pos -= camera->getPosition();
			auto bullet = BulletSprite::create(pos, pos2,
				_damage, pTarget[i], this, IMG_BULLET_FIRE);
			this->getParent()->addChild(bullet, 99);
			bullet->setLocalZOrder(200);
		}
	}
}
