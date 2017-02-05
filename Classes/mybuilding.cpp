#include "mybuilding.h"
#include "Classes/Utils/Config.h"
#include <string>
using namespace std;
#include "Classes/Utils/GlobalManager.h"
#include "Classes/HelloWorldScene.h"


Building* Building::create(int ID, Vec2 pos)
{
	Building *pRet = new(std::nothrow) Building();
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

bool Building::init(int ID, Vec2 pos)
{
	if (!Sprite::init()) {
		return false;
	}
	AddToManager();
	_pos = pos;
	_arm = nullptr;
	_pTarget = NULL;
	if (getCamp() == BaseSprite::Camp::Me)
		_pepole._pepole_number = 5;

	// 加载图形
	this->setTexture(IMG_BUILDING_Floor);
	string nomal_IMG = GM()->getBuildingIMG(1);
	_normal = Sprite::create(nomal_IMG);
	_broken = Sprite::create(GM()->getBuildingBrokenIMG(1));
	_broken->setOpacity(200);
	_broken->setVisible(false);
	setPosition(pos);
	//_normal->setAnchorPoint(Vec2::ZERO);
	_normal->setPosition(Vec2(30, 30));
	this->addChild(_normal, 1);
	this->addChild(_broken, 1);
	loadData();

	addHPBar();
	Vec2 posBg = getChildByName("Bar")->getPosition();
	getChildByName("Bar")->setPosition(posBg + Vec2(30, 30));
	//posBg = _hpBar->getPosition();
	//_hpBar->setPosition(posBg + Vec2(30, 30));

	setSelect(false);
	_id = ID;
	_heroID = ID;
	this->setLocalZOrder((int)_pos.x + (int)_pos.y);
	setScale(2.0f);
	setContentSize(Size(140, 140));
	//setAnchorPoint(Vec2(2,2));

	// produce
	_build_process = BuildProcess::create();
	_build_process->set(this);
	_build_process->setPosition(Vec2(30, 56));
	addChild(_build_process,200);
	_posRally = getCenter() + Vec2(40, 40);
	_secondProduce = 5;
	_start_produce_time = 0;
	schedule(schedule_selector(Building::ProduceUpdate), _secondProduce);
	return true;
}

void Building::Produce()
{
	// 二个士兵一个弓箭手
	srand(time(0));
	int v = rand()%3;
	// 因为不在init中使用addchild会造成Zorder问题， 精灵不可见， 因此在init先统一生成，再选择一个
	Hero* pFinded = NULL;
	for (auto child : ((HelloWorld*)getParent())->getChildren()) {
		if (child->getName() == "soilder") {
			Hero* p = (Hero*)child;
			if (v == 0) {
				if (p->GetID() > 200 && !p->isVisible())
				{
					pFinded = p;
					break;
				}
			}
			else
			{
				if (p->GetID() > 100 && !p->isVisible())
				{
					pFinded = p;
					break;
				}

			}
		}
	}
	
	// move
	if (pFinded && !_pepole.IsLimit()) {
		Vec2 pos_start = getCenter();
		pFinded->Reset();
		pFinded->setCamp(getCamp());
		pFinded->setPosition(pos_start);
		pFinded->setVisible(true);
		CTarget target(_posRally);
		pFinded->run(&target);
		_pepole.Add();
		if (getCamp()==BaseSprite::Camp::Me)
			((HelloWorld*)getParent())->updatePepole();
	}
}

void Building::die()
{
	// 显示建筑破坏后的图形
	unscheduleAllCallbacks();
	auto bg = (Sprite*)this->getChildByName("Bar");
	bg->removeFromParent();
	_build_process->removeFromParent();
	setVisible(true);

	_normal->setVisible(false);
	_broken->setPosition(Vec2(40, 40));
	_broken->setVisible(true);
}

void Building::setSelect(bool select)
{
	if (getChildByTag(0) == nullptr) {
		auto circle = DrawNode::create();
		float radius = getContentSize().width / 2;
		circle->drawCircle(Vec2(30, 30), radius, CC_DEGREES_TO_RADIANS(90), 30, false, Color4F::GREEN);
		circle->setTag(0);

		this->addChild(circle, -1);
	}

	_isSelect = select;
	getChildByTag(0)->setVisible(_isSelect);
}

void Building::run(ITarget* pTarget, bool allow_attack /*= false*/)
{
	Vec2 pos = pTarget->getCenter();
	_posRally = pos;
	pos = convertToNodeSpace(pos);
	Vec2 posMe = getCenter();
	//int tag = 2;
	//if (getChildByTag(2) == nullptr) {
	//	auto drawer = DrawNode::create();
	//	drawer->drawLine(Vec2(20,20), Vec2(100,100), Color4F::GREEN);
	//	drawer->setTag(2);
	//	//drawer->setLocalZOrder(10000);
	//	addChild(drawer, -1);
	//}
	//getChildByTag(2)->setVisible(_isSelect);
	DrawNode* drawer = ((DrawNode*)getChildByTag(0));
	drawer->clear();
	float radius = getContentSize().width / 4;
	drawer->drawCircle(Vec2(30, 30), radius, CC_DEGREES_TO_RADIANS(90), 30, false, Color4F::GREEN);
	drawer->drawLine(Vec2(20, 20), pos, Color4F::GREEN);
}

float Building::getProduceProcess()
{
	if (_pepole.IsLimit())
		return 0;
	return float(GetTickCount()-_start_produce_time) / float(_secondProduce*1000);
}

void Building::ProduceUpdate(float dt)
{
	if (_start_produce_time>0)
		Produce();
	_start_produce_time = GetTickCount();
	
}

void Building::setCamp(int camp)
{
	Hero::setCamp(camp);
	if (camp == BaseSprite::Camp::Enemy)
		_posRally = getCenter() - Vec2(160,160);
}


CPepoleManager::CPepoleManager()
{
	_pepole_limit = 30;
	_pepole_number = 0;
}

bool CPepoleManager::IsLimit()
{
	return _pepole_number >= _pepole_limit;
}

void CPepoleManager::Add()
{
	_pepole_number++;
}
