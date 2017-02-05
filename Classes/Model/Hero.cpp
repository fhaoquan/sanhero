#include "Hero.h"
#include "../Utils/Config.h"
#include "Utils/GlobalManager.h"
#include <math.h>
#include <vector>
#include "HelloWorldScene.h"
#include "Classes/mybuilding.h"

enum 
{
	Select_Circle,
	Run_Action
};
static const int STATE_IDLE = 0;
static const int STATE_RUN = 1;
static const int STATE_ATK = 2;
static const int STATE_SKILL = 3;
static const int STATE_DEATH = 4;
static const int STATE_FOLLOW = 5;

std::vector<Hero*> g_Heros;
void filter(std::vector<Hero*>& vec, std::function<void(Hero*)> fn) {
	for (unsigned i = 0; i < vec.size(); i++) {
		fn(vec.at(i));
	}
}
Hero* Hero::create(int ID, Vec2 pos)
{
    Hero *pRet = new(std::nothrow) Hero();
    if (pRet && pRet->init(ID, pos)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}


bool Hero::init(int ID, Vec2 pos)
{
    if ( !Sprite::init() ) {
        return false;
    }
    
    _id = ID;
    _pos = pos;
	_startPos = pos;
	_pTarget = NULL;
    _isSelect = true;
    _state = -1;
	_InConcat = 0;
	_attack_cd = 2;
	    
    loadData();
    showUI();
    addHPBar();
    
    addTouch();
    
    return true;
}

void Hero::loadData()
{
    //ValueMap& data = getHero(_id);
    //
    //_heroID         = data["HeroID"].asInt();
    //_type           = data["Type"].asInt();
    //_level          = data["Level"].asInt();
    //_name           = data["Name"].asInt();
    //
    //_healthPoint    = data["HealthPoint"].asInt();
    //_totalHP        = _healthPoint;
    //_damage         = data["Damage"].asInt();
    //_attackSpeed    = data["AttackSpeed"].asInt();
    //_shootRange     = data["ShootRange"].asInt();

	_heroID = 20101;
	_type = 1;
	_level = 1;
	_name = 1;

	_healthPoint = 100;
	_totalHP        = _healthPoint;
	_damage = 10;
	_attackSpeed = 1;
	_shootRange = 60;
	_moveSpeed = 80;

    _isbroken       = false;
    _expReward      = 0;
	_Vison = 200;

	setName("hero");
}

int g_Category = 0x1;
void Hero::showUI()
{
	_arm = Armature::create(ANIM_NAME_ARAGORN);
	_arm->getAnimation()->play("idle0");
	this->addChild(_arm);
	setContentSize(_arm->getContentSize());
	setSelect(false);

	_skill1 = Armature::create(ANIM_NAME_SKILL_1);
	_skill2 = Armature::create(ANIM_NAME_SKILL_2);
    _skill1->setVisible(false);
    _skill2->setVisible(false);
    this->addChild(_skill1);
    this->addChild(_skill2);

	// Label
	char sz[10];
	sprintf(sz, "%d", _id);
	TTFConfig ttfConfig("fonts/arial.ttf", 16);
	auto label = Label::createWithTTF(ttfConfig, sz);
	addChild(label);

    //auto scaleUp = ScaleTo::create(0.3f, 1.1f);
    //auto scaleDown = ScaleTo::create(0.3f, 1.0f);
    //circle->runAction(RepeatForever::create(Sequence::create(scaleUp, scaleDown, nullptr)));
    
    _dir = GM()->getDir(_pos, Vec2(19, 19));
    //this->setPosition(GM()->getMapPos(_pos));


	//setAnchorPoint(Vec2(0.5f, 0.5f));
	//setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	Vec2 pos_anchor = getAnchorPoint();
	this->setLocalZOrder((int)_pos.x + (int)_pos.y);
    this->setScale(0.8f);
	this->setPosition(_pos);

	setPhysicsBody();
}

void Hero::setPhysicsBody()
{
	float r = getContentSize().width / 2;
	auto body = PhysicsBody::createCircle(r, PhysicsMaterial(0, 0, 0.0f));
	//auto body = PhysicsBody::createCircle(r);
	body->setCategoryBitmask(1);    // 0001
	body->setContactTestBitmask(-1); // 0100	this function control listener
	body->setCollisionBitmask(-1);   // 0011
	body->setGravityEnable(false);
	addComponent(body);
}



void Hero::addHPBar()
{
    auto bg = Sprite::create(IMG_BUILD_PRO_BK);
    _hpBar = LoadingBar::create(IMG_BUILD_PRO);
    bg->setPosition(0, getContentSize().height/2 + 10);
    _hpBar->setPosition(bg->getContentSize()/2);
    bg->addChild(_hpBar);
    this->addChild(bg, 9, "Bar");
    
    bg->setScale(0.7f);
    _hpBar->setPercent(100.0 * _healthPoint / _totalHP);
}


void Hero::idle()
{
	if (_state != STATE_IDLE) {
		StopMove(1);
		_arm->getAnimation()->play("idle0");
		_pos = getCenter();
		_targetPos = _pos;
		_startPos = _pos;
	}
	//_arm->getAnimation()->play("atk1");
	_state = STATE_IDLE;
	SetZOrder();
}


void Hero::atk(ITarget* pTarget[], int size_target)
{
    if (_isbroken == true || _healthPoint <= 0 || !isVisible()) {
        return;
    }

	// can attack
	if (canAttack()) {
		_expReward += _damage / 50.0;
		for (int i = 0; i < size_target; i++) {
			pTarget[i]->GetAction()->hurt((IAttack*)this);
		}
		_pre_attack_tick = GetTickCount();
	}
    
}

bool Hero::canAttack() {
	// normol attack cd
	return GetTickCount() > _pre_attack_tick + 2 * 1000;
}

float Hero::getAttackHurt()
{
	// 普通攻击伤害
	return _damage;
}


// 受伤
void Hero::hurt(IAttack* pAttacker)
{
    if (_isbroken == true || _healthPoint <= 0) {
        _isbroken = true;
        return;
    }

	float x = pAttacker->getAttackHurt();
    _healthPoint -= x;
    if (_healthPoint <= 0) {
        _isbroken = true;
        this->setVisible(false);
		if (_arm)
			_arm->getAnimation()->stop();
		// 关闭碰撞
		removeAllComponents();
		die();	//Demo期暂时不修正g_Heros的错误
		pAttacker->notifyTargetDeath();
    }
    else {
        _hpBar->setPercent(100.0 * _healthPoint / _totalHP);
    }
}


// 技能结束
void Hero::finishSkill(Armature* arm, int state)
{
    _state = state;
    arm->setVisible(false);
    arm->getAnimation()->stop();
}

void Hero::FinishSkill(float dt)
{
	if (dt > 0.999 && dt < 1.001) {
		_skill1->setVisible(false);
		_skill1->getAnimation()->stop();

	}
	if (dt > 1.009 && dt < 1.012) {
		_skill2->setVisible(false);
		_skill2->getAnimation()->stop();

	}
}

// 释放技能
void Hero::putSkill(int type)
{
    int state = _state;
    _state = STATE_SKILL;
    // 技能1
    if (type == 1) {
        _skill1->setVisible(true);
        _skill1->getAnimation()->play("Skill");
        //auto delay = DelayTime::create(1.0f);
        //auto func = CallFunc::create(CC_CALLBACK_0(Hero::finishSkill, this, _skill1, state));
        //this->runAction(Sequence::create(delay, func, nullptr));
		scheduleOnce(schedule_selector(Hero::FinishSkill), 1);
    }
    // 技能2
    else {
        _skill2->setVisible(true);
        _skill2->getAnimation()->play("Skill");
        //auto delay = DelayTime::create(1.0f);
        //auto func = CallFunc::create(CC_CALLBACK_0(Hero::finishSkill, this, _skill2, state));
        //this->runAction(Sequence::create(delay, func, nullptr));
		scheduleOnce(schedule_selector(Hero::FinishSkill), 1.01f);
    }
}


// 是否死亡
bool Hero::isDeath()
{
	if (!isVisible())
		return true;
    return _isbroken;
}


void Hero::setSelect(bool select)
{
	if (getChildByTag(Select_Circle) == nullptr) {
		auto circle = DrawNode::create();
		//frame->setPosition(-1*getContentSize());
		//frame->drawRect(convertToNodeSpace(getPosition()), convertToNodeSpace(getPosition() + _arm->getContentSize()), Color4F::GREEN);
		//auto circle = Sprite::create(IMG_CIRCLE);
		float radius = getContentSize().width / 2;
		circle->drawCircle(getAnchorPoint(), radius, CC_DEGREES_TO_RADIANS(90), 30, false, Color4F::GREEN);
		//circle->setPosition(Vec2(-radius, -radius));
		circle->drawCircle(Vec2(0,0), radius, CC_DEGREES_TO_RADIANS(90), 30, false, Color4F::GREEN);
		//circle->setPosition(Vec2(-radius/2, -radius/2));
		//circle->setScale(.6f);
		//circle->setVisible(false);
		circle->setTag(Select_Circle);

		this->addChild(circle, -1);
	}

    _isSelect = select;
	getChildByTag(Select_Circle)->setVisible(_isSelect);
}


void Hero::addTouch()
{
	AddToManager();

	// 添加碰撞处理
	// 注册碰撞事件
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Hero::onContactBegin, this);
	contactListener->onContactPostSolve = CC_CALLBACK_2(Hero::onContactPostSolve, this);
	contactListener->onContactSeparate = CC_CALLBACK_1(Hero::onContactSeperate, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    auto dispatcher = this->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(Hero::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(Hero::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(Hero::onTouchEnded, this);
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//auto mouse_listener = EventListenerMouse::create();
	//mouse_listener->onMouseUp = CC_CALLBACK_1(Hero::onMouseUp, this);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);

	auto listener2 = EventListenerKeyboard::create();
	listener2->onKeyPressed = [](EventKeyboard::KeyCode KeyCode, Event* event) {
		if (KeyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
			filter(g_Heros, [](Hero* pHero) {pHero->setSelect(false); });
		}
	};
	dispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

	_targetPos = getCenter();

	schedule(schedule_selector(Hero::MyUpdate), 0.81f);

}

bool Hero::onTouchBegan(Touch* pTouch, Event* pEvent)
{
    //if (_isSelect == false) return false;
	
    _delta = 0.0;
    return true;
}

void Hero::onTouchMoved(Touch* pTouch, Event* pEvent)
{
    _delta += GM()->getDistance(Vec2(0, 0), pTouch->getDelta());
}

void Hero::onTouchEnded(Touch* pTouch, Event* pEvent)
{
	return;
}

Vec2 getNextPos(Vec2 src, Vec2 des, bool isHero)
{
	Vec2 pos = src;
	int mindis = INF;
	for (int i = 0; i < 9; i++) {
		int x = src.x + DX[i] + EPS;
		int y = src.y + DY[i] + EPS;
		if (x < 0 || x > TILED_TOTAL_X || y < 0 || y > TILED_TOTAL_Y) continue;
		if (isHero && GM()->isCovered(Vec2(x, y))) continue;
		int dis = GM()->getManhadun(Vec2(x, y), des);
		if (dis < mindis) {
			mindis = dis;
			pos = Vec2(DX[i], DY[i]);
		}
	}
	return pos;
}
bool VecEq(const Vec2& pt1, const Vec2& pt2) {
	return fabs(pt1.x - pt2.x) <= 1e-3 && fabs(pt1.y - pt2.y) <= 1e-3;
}
void Hero::MyUpdate(float dt)
{
	CCLOG("%s, %f, %d", __FUNCTION__, dt, _id);
	//return;
	//CCLOG("Hero::update %d, %f, %08x", GetTickCount(), dt, this);
	_tickTimer = GetTickCount();
    if (_isbroken == true) {
        this->unscheduleAllCallbacks();
        return;
    }

	if ((_isRunAttack && _pTarget == nullptr) || _state==STATE_IDLE) {	// A 中途发现敌人
		_pTarget = findAttackTarget();
		if (_pTarget) {
			float distance = _pTarget->getCenter().distance(getCenter());
			if (distance < _shootRange && !_pTarget->isDeath() && getCamp() != _pTarget->getCamp())
				attack();
			else
			{
				run(_pTarget);
			}
			// 敌人走远了, 不追了
			distance = _pTarget->getCenter().distance(_targetPos);
			if (_pTarget->isDeath() || distance > _Vison)
				_pTarget = nullptr;
		}
	}else if (_pTarget) {		// 跟随目标
		float distance = getCenter().distance(_pTarget->getCenter());
		if (distance < _shootRange && !_pTarget->isDeath() && getCamp() != _pTarget->getCamp())
			attack();
		else
		{
			// 跟随的目标是否在移动
			bool is_moving = !VecEq(_pTarget->getCenter(), _posFollowTarget);
			if (is_moving) {
				// 重新跟随
				StopMove(1);
				_state = -1;
				run();
			}
			_posFollowTarget = _pTarget->getCenter();

			// 防止碰撞后继续挤压， 只要挨到了目标即可
			Vec2 pt_center = getCenter();
			Vec2 pt_target = _pTarget->getCenter();
			Size size_me = getContentSize();
			Size size_target = _pTarget->GetSprite()->getContentSize();
			if (pt_center.distance(pt_target) < size_me.width / 2 + size_target.width / 2){
				idle();
			}
			else if (IsInTargetPos())
			{
				idle();
			}
			else
			{
				run();
			}
		}
	}
	if (!_pTarget)		// 移动到目标点
	{
		if (IsInTargetPos()){
			if (_id == 22) {
				CCLOG("");
			}
			idle();
		}
		else
		{
			run();
		}
	}

}

void Hero::run()
{
	//Vec2 pos = getNextPos(_pos, _target->_pos, true);
	//_dir = GM()->getDir(_targetPos-_pos);
	//_dir = getDir(_pos - _targetPos);
	Vec2  m = std::min(_pos, _targetPos);
	_dir = GM()->getDir(_targetPos - m, _pos- m);

	//stopAllActions();
	if (_state != STATE_RUN && _arm) {	// state changed
		Vec2 mypos = getCenter();
		float distance = sqrt(pow((_targetPos.x - mypos.x), 2) + pow((_targetPos.y - mypos.y), 2));
		Vec2 deltaPos;
		deltaPos.setPoint((_targetPos.x - mypos.x) , (_targetPos.y - mypos.y) );
		//CCLOG("deltaPos = %f,%f", deltaPos.x, deltaPos.y);
		//CCLOG("_pos=%f,%f", _pos.x, _pos.y);
		//CCLOG("_targetPos=%f,%f", _targetPos.x, _targetPos.y);
		StopMove(1);
		float dt = distance / _moveSpeed;
		auto run_action = MoveBy::create(dt, deltaPos);
		run_action->setTag(Run_Action);
		
		this->runAction(run_action);
		scheduleOnce(schedule_selector(Hero::StopMove), dt);
		// 这种动画必须和Action一起调用，且只调用一次，update不停调用的话只会播放第一帧
		char szAnimation[100];
		sprintf(szAnimation, "run%d", _dir);
		_arm->getAnimation()->play(szAnimation);
	}
	//this->runAction(MoveBy::create(0.8f, GM()->getMapDelta(_dir)));
	//_arm->getAnimation()->play("run1");
	//_arm->getAnimation()->play("run" + GM()->getIntToStr(_dir <= 7 ? _dir : 1));

	_pos = getCenter();
	//setLocalZOrder((int)_pos.x + (int)_pos.y);
	SetZOrder();
	_state = STATE_RUN;

}

void Hero::StopMove(float dt)
{
	stopAllActions();
	unschedule(schedule_selector(Hero::StopMove));
}

bool Hero::onContactBegin(cocos2d::PhysicsContact& contact)
{
	Hero * p = (Hero*)contact.getCurrentTarget();
	//碰撞开始后， 有能够攻击的对象就停止走动，开始攻击
	if (CanAttack())
		MyUpdate(1);
	//if (_state == STATE_ATK) {
	//	StopMove(1);
	//	_arm->getAnimation()->play("atk2");
	//}
	//CCLOG("%f,%f,%f,%f", contact.getShapeA()->getCenter().x, contact.getShapeA()->getCenter().y, getCenter().x, getCenter().y);
	//if (SpritesHaveIntersect(this))
	_InConcat = 1;
	//CCLOG("Hero::onContactBegin %08x", this);
	return true;
}

void Hero::AddToManager()
{
	g_Heros.push_back(this);
}

ITarget* Hero::findAttackTarget()
{
	float min_distance = 999;
	Hero * pResult = nullptr;
	for (unsigned int i = 0; i < g_Heros.size(); i++) {
		Hero* p = g_Heros[i];
		if (p->getCamp() != getCamp() && !p->isDeath()) {
			float distance = p->getCenter().distance(getCenter());
			if (distance < this->getVison()) {
				if (distance < min_distance) {
					pResult = p;
					min_distance = distance;
				}
			}
		}
	}
	return pResult;
}

cocos2d::Vec2 Hero::getCenter()
{
	Vec2 pos = getPosition();
	auto run_action = getActionByTag(Run_Action);
	if (run_action && !run_action->isDone()) {
		pos = run_action->getOriginalTarget()->getPosition();
		//CCLOG("Hero::IsInTargetPos=%f,%f", pos.x, pos.y);
	}
	//return getPosition() - Vec2(getContentSize().width / 2, getContentSize().height / 2);
	return pos - getCenterOffset();
}

class SpritesInTargetPos {
	vector<Hero*> _vecTravled;	//已经遍历过的节点
	vector<Hero*> _vec;
	Hero* _pCur;
public:
	bool IsInTargetPos(Vec2 pos, Hero * pThis) {
		_pCur = pThis;
		GetScopeHeros(pos, pThis->getCenter());
		return GetChild(pos, pThis);
		return false;
	}
private:
	bool IsTraveled(Hero* p) {
		for (unsigned i = 0; i < _vecTravled.size(); i++) {
			if (p == _vecTravled[i])
				return true;
		}
		return false;
	}
	void GetScopeHeros(Vec2& pos_target, Vec2& pos_cur) {
		// 取范围内的对象
		float d = GM()->getDistance(pos_target, pos_cur);
		for (unsigned i = 0; i < g_Heros.size(); i++) {
			Hero* p = g_Heros[i];
			if (!p->isDeath()) {
				Vec2 pos = p->getCenter();
				if (GM()->getDistance(pos, pos_target) <= d) {
					_vec.push_back(p);
				}
			}
		}
	}
	bool GetChild(Vec2 pos_target, Hero* pThis) {
		float r3 = 1.01f;
		for (unsigned i = 0; i < _vec.size(); i++) {
			Hero* p = g_Heros[i];
			if (p != pThis && !p->isDeath() && !IsTraveled(p)) {
				_vecTravled.push_back(p);
				//相交
				Vec2 pos_me = pThis->getCenter();
				float r = pThis->getCenterOffset().x;
				Vec2 pos_child = p->getCenter();
				float r2 = p->getCenterOffset().x;
				float distance = GM()->getDistance(pos_me, pos_child);
				if (distance < r + r2) {
					// it's child
					distance = GM()->getDistance(pos_target, pos_child);
					if (distance < r2 + r3) {
						return true;
					}
					else
					{
						return GetChild(pos_target, p);
					}
				}
			}
		}
		return false;
	}
};

bool SpritesHaveIntersect(Hero* pHero) {
	for (unsigned i = 0; i < g_Heros.size(); i++) {
		Hero* p = g_Heros[i];
		if (p != pHero && !p->isDeath()) {
			Vec2 pos_me = pHero->getCenter();
			float r = pHero->getCenterOffset().x;
			Vec2 pos_child = p->getCenter();
			float r2 = p->getCenterOffset().x;
			float distance = GM()->getDistance(pos_me, pos_child);
			if (distance < r + r2) {
				return true;
			}
		}
	}
	return false;
}

// 两元素相交
bool IsInterset(Hero* p1, Hero* p2) {
	if (p1->isDeath() || p2->isDeath())
		return false;
	Vec2 pos_me = p1->getCenter();
	float r = p1->getCenterOffset().x;
	Vec2 pos_child = p2->getCenter();
	float r2 = p2->getCenterOffset().x;
	float distance = GM()->getDistance(pos_me, pos_child);
	if (distance < r + r2) {
		return true;
	}
	return false;
}
bool IsInterset(Hero* p1, Vec2 pos) {
	Vec2 pos_me = p1->getCenter();
	float r = p1->getCenterOffset().x;
	float distance = GM()->getDistance(pos_me, pos);
	return (distance < r + 0.5);
}

bool IsSamePoint(Vec2 p1, Vec2 p2) {
	return fabs(p1.x - p2.x) <= 1 && fabs(p1.y - p2.y) <= 1;
}
bool Hero::IsInTargetPos()
{
	Vec2 pos = getCenter();
	if (fabs(pos.x - _targetPos.x) <= 1e-3 && fabs(pos.y - _targetPos.y) <= 1e-3) {	// 当前点等于目标点
		return true;
	}
	else
	{
		if (this->_id == 1) {
			CCLOG("");
		}
		if (_InConcat == 2) {	//碰撞解决
			//IsSamePoint(getCenter(), _concatPostSolvePos);
			// 在边缘
			float d = pos.distance(_targetPos);
			vector<Hero*> vecInterset;
			for (unsigned i = 0; i < g_Heros.size(); i++) {
				Hero* p = g_Heros[i];
				if (p != this) {
					if (IsInterset(this, p))
						vecInterset.push_back(p);
				}
			}
			//// 目标点和所有相交元素与当前元素的距离一致， 说明碰撞结束
			//bool bEdge = true;
			//for (unsigned i = 0; i < vecInterset.size(); i++) {
			//	float d2 = _targetPos.distance(vecInterset[i]->getCenter());
			//	if (fabs(d - d2) > 5)
			//		bEdge = false;
			//}
			//if (vecInterset.size() == 0)
			//	bEdge = false;

			//int id = 22;
			//if (_id == id) {
			//	CCLOG("");
			//}
			//if (bEdge) {
			//	//if (IsInterset(this, _targetPos))
			//		return true;

			//}
			//如果目标点与元素相交，且其它相交元素处于停止状态， 那么也停止

			if (IsInterset(this, _targetPos))
				return true;
			// 集体的中心点
			//this->_state;
			SpritesInTargetPos calc;
			if (calc.IsInTargetPos(_targetPos, this))
				return true;


		}

	}
	return false;
}

void Hero::onContactPostSolve(cocos2d::PhysicsContact& contact, const cocos2d::PhysicsContactPostSolve& solve)
{
	//idle();
	//if (this->_id == 22)
	//	CCLOG("Hero::onContactPostSolve %d", this->_id);
	
	//if (_InConcat == 1) {
	//	_concatPostSolvePos = getCenter();
	//	//std::function<void(float t)> fn = [this](float t){_InConcat = 2; };
	//	//scheduleOnce(fn, 1.0f, );
	//	scheduleOnce(schedule_selector(Hero::changeConcatStatus), 1);
	//}
	_InConcat = 2;

	//Vec2 pos = contact.getShapeA()->getCenter();
	//CCLOG("%f,%f", pos.x, pos.y);
}

int Hero::getVison()
{
	return _Vison;
}

void Hero::attack()
{
	if (_state != STATE_ATK) {
		StopMove(1);
		Vec2 targetPos = _pTarget->getCenter();
		Vec2 pos = getCenter();
		int dir;
		Vec2 m = std::min(pos, targetPos);
		dir = GM()->getDir(targetPos-m, pos-m);

		//dir = getDir( targetPos - pos);
		char szAnimation[100];
		sprintf(szAnimation, "atk%d", dir);
		_arm->getAnimation()->play(szAnimation);
	}
	//auto delay = DelayTime::create(0.6f);
	//auto func = CallFunc::create(CC_CALLBACK_0(Hero::atk, this, _target));
	//this->runAction(Sequence::create(delay, func, nullptr));
	atk(&_pTarget, 1);
	_state = STATE_ATK;
	_pos = getCenter();
}

void Hero::onContactSeperate(cocos2d::PhysicsContact& contact)
{
	//CCLOG("Hero::onContactSeperate");
	//_isInConcat = false;
}

cocos2d::Vec2 Hero::getCenterOffset()
{
	return Vec2(28, 20);
}

void Hero::onMouseUp(cocos2d::EventMouse* event)
{
	// Right
	if (_isSelect && event->getMouseButton() == 1) {
		Vec2 pos = event->getLocationInView();
		pos = convertToTileView(pos);
		_targetPos = pos;
		_state = -1;
		_InConcat = -1;
		run();
	}
}

extern int g_kCamera;
cocos2d::Vec2 Hero::convertToTileView(Vec2 pos)
{
	auto camera = getParent()->getChildByTag(g_kCamera);
	return pos+camera->getPosition();
}

int Hero::GetID()
{
	return _id;
}

void Hero::SetZOrder()
{
	Vec2 pos_tile_block = ((HelloWorld*)getParent())->ViewConvertToTilePos(_pos);
	pos_tile_block.set(pos_tile_block.x / 32, pos_tile_block.y / 32);
	this->setLocalZOrder((int)pos_tile_block.x + (int)pos_tile_block.y);
}

int Hero::GetStatus()
{
	return _state;
}

void Hero::run(ITarget* pTarget, bool allow_attack)
{
	_isRunAttack = allow_attack;
	_pTarget = pTarget;
	if (pTarget->GetAction() == NULL)
		_pTarget = nullptr;
	_targetPos = pTarget->getCenter();
	_state = -1;
	_InConcat = -1;
	run();
}

Hero::Hero()
{
	_pre_attack_tick = 0;
	_camp = 0;
	_posFollowTarget = Vec2(0, 0);
}

Hero::~Hero()
{

}

bool Hero::IsClickMe(Vec2 pos)
{
	//判断鼠标是否点击到该对象上
	Vec2 pt = getPosition();
	Vec2 pt_anchor = getAnchorPoint();
	Vec2 pt_center = getCenter();
	Size s = getContentSize();
	Rect rc(Vec2(pt_center.x - s.width/2,pt_center.y - s.height/2), s);
	return rc.containsPoint(pos);
}

bool Hero::CanAttack()
{
	if (_pTarget) {
		float distance = getCenter().distance(_pTarget->getCenter());
		return _pTarget->getCamp() != getCamp() && distance < _shootRange && !_pTarget->isDeath();

	}
	return false;
}

int Hero::getCamp()
{
	return _camp;
}

void Hero::setCamp(int camp)
{
	_camp = camp;
}

ISpriteAction* Hero::GetAction()
{
	return this;
}

void Hero::die()
{
	// 暂时没有死亡动画
	//removeFromParent();
	int kCity1 = 2000;
	int city_tag = kCity1 + 1;
	if (getCamp() == BaseSprite::Camp::Enemy)
		city_tag = kCity1;
	Building* city = (Building*)(getParent()->getChildByTag(city_tag));
	city->_pepole._pepole_number--;
	((HelloWorld*)getParent())->updatePepole();
}

Sprite* Hero::GetSprite()
{
	return this;
}

// 得到面对的朝向
int Hero::getDir(Vec2 delta)
{
	int x = (int)delta.x;
	int y = (int)delta.y;
	if (x >0 && y >0) return 0;
	if (x >0 && y == 0) return 1;
	if (x >0 && y <0) return 2;
	if (x == 0 && y <0) return 3;
	if (x <0  && y <0) return 4;
	if (x <0 && y == 0) return 5;
	if (x <0 && y >0) return 6;
	if (x == 0 && y >0) return 7;
	return 9;
}

void Hero::Reset()
{
	setSelect(false);
	_isbroken = false;
	_healthPoint = 100;
	_totalHP = _healthPoint;
	_hpBar->setPercent(100.0 * _healthPoint / _totalHP);
}

void Hero::notifyTargetDeath()
{
	idle();
}

cocos2d::Vec2 CTarget::getCenter()
{
	return *this;
}

ISpriteAction* CTarget::GetAction()
{
	return NULL;
}

bool CTarget::isDeath()
{
	return false;
}

CTarget::CTarget(const Vec2& pos)
{
	this->x = pos.x;
	this->y = pos.y;
}

int CTarget::getCamp()
{
	return 0;
}

void CTarget::setCamp(int camp)
{

}

Sprite* CTarget::GetSprite()
{
	return nullptr;
}
