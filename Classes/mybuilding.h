#ifndef __MYBUILDING_H__
#define __MYBUILDING_H__

#include "Classes/Utils/stdafx.h"
#include "src/c_sprite_action.h"
#include "Classes/Model/Hero.h"
#include "Classes/Model/BuildProcess.h"

class CPepoleManager
{
public:
	CPepoleManager();
	bool IsLimit();
	void Add();

	int _pepole_limit;		// 人口上限
	int _pepole_number;		// 当前人口
};
class Building : public Hero, public IBuildAction
{
public:
	static Building* create(int ID, Vec2 pos);
	virtual bool init(int ID, Vec2 pos);

	//CC_CREATE_NO_PARAM(Building);

	// ISpriteAction
	virtual void run(ITarget* pTarget, bool allow_attack = false);

	//IBuildAction
	virtual void Produce();
	virtual float getProduceProcess();

	// ISpriteAction
	virtual void die();
	virtual void setSelect(bool select);

	virtual void setCamp(int camp);

	CPepoleManager _pepole;
protected:
	void DrawDeline(Vec2 pos_start, Vec2 pos_end);
	void ProduceUpdate(float dt);

	Vec2 _posRally;		// 集结点
	Sprite* _normal;    // 正常图片
	Sprite* _broken;    // 毁坏图片
	BuildProcess* _build_process;

	int _secondProduce;	// 生产花费的秒数
	int _start_produce_time;

};
#endif