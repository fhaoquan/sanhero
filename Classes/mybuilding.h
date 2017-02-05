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

	int _pepole_limit;		// �˿�����
	int _pepole_number;		// ��ǰ�˿�
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

	Vec2 _posRally;		// �����
	Sprite* _normal;    // ����ͼƬ
	Sprite* _broken;    // �ٻ�ͼƬ
	BuildProcess* _build_process;

	int _secondProduce;	// �������ѵ�����
	int _start_produce_time;

};
#endif