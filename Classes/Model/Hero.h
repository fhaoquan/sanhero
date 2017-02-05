#ifndef __Hero_H__
#define __Hero_H__

//#include "..\Utils\Public.h"
#include "..\Utils\stdafx.h"
#include "src\c_sprite_action.h"
#include "BaseSprite.h"

class CTarget : public Vec2, public ITarget
{
public:
	CTarget(const Vec2& pos);
	// ITarget
	virtual Vec2 getCenter();
	virtual ISpriteAction* GetAction();
	virtual Sprite* GetSprite();
	virtual	bool isDeath();             // 是否死亡
	virtual int getCamp();
	virtual void setCamp(int camp);
};
//1.闲置
//2.行走， 跟随目标， 指定位置
//3.攻击
//4.让路
//5.受伤
//6.死亡
//7.卡位

class Hero : public Sprite, public ISpriteAction, public ITarget, public IAttack
	,public IAI
{
public:
    virtual bool init(int ID, Vec2 pos);
    static Hero* create(int ID, Vec2 pos);

	Hero();
	virtual ~Hero();
public:
	int GetID();

    virtual void setSelect(bool select);
	bool isSelected() { return _isSelect; }

	// ISpriteAction
	virtual void idle();
	virtual void run(ITarget* pTarget, bool allow_attack = false);
	virtual void atk(ITarget* pTarget[], int size_target);   // 攻击, size_target=1为dps，>1为AOE
	virtual void hurt(IAttack* pAttacker);           // 受伤, 根据攻击公式计算最后的伤害
	virtual void die();             // 死亡
	virtual void putSkill(int SkillId);    // 释放技能

    
	// ITarget
	virtual Vec2 getCenter();
	virtual ISpriteAction* GetAction();
	virtual Sprite* GetSprite();
	virtual	bool isDeath();             // 是否死亡
	virtual int getCamp();
	virtual void setCamp(int camp);

	// IAttack
	virtual bool canAttack();
	virtual float getAttackHurt();
	virtual void notifyTargetDeath();

	// 发现可以行动的目标， 比如敌人，或己方增益目标, 如果有一堆目标，那么选择最近的一个
	// IAI
	virtual ITarget* findAttackTarget();

	void Reset();
	Vec2 getCenterOffset();

	bool IsClickMe(Vec2 pos);
	bool IsSelected() { return _isSelect; }
	void setDeath(bool death) { _isbroken = death; }
protected:
	bool CanAttack();
	void MyUpdate(float dt);
	void StopMove(float dt);
	void FinishSkill(float dt);
	void changeConcatStatus(float dt) { _InConcat = 2; }
	bool onContactBegin(cocos2d::PhysicsContact& contact);
	void onContactPostSolve(cocos2d::PhysicsContact& contact, const cocos2d::PhysicsContactPostSolve& solve);
	void onContactSeperate(cocos2d::PhysicsContact& contact);

	void addTouch();
	void loadData();    // 加载数据
	void showUI();      // 显示UI
	void addHPBar();    // 加血条
	void setPhysicsBody();
	virtual bool onTouchBegan(Touch* pTouch, Event* pEvent);
	virtual void onTouchMoved(Touch* pTouch, Event* pEvent);
	virtual void onTouchEnded(Touch* pTouch, Event* pEvent);
	void onMouseUp(cocos2d::EventMouse* event);

	bool IsInTargetPos();
	int GetStatus();
	void run();
	void attack();
	void finishSkill(Armature* arm, int state); // 技能结束

protected:
    int _id;
    int _heroID;
    int _type;
    int _level;
    int _name;
    
    int _healthPoint;
    int _totalHP;
    int _damage;
    int _attackSpeed;
    int _shootRange;
	int _moveSpeed;
	int _Vison;					// 视距
	int _attack_cd;				//攻击的cd
	unsigned int _pre_attack_tick;		//上一次攻击的时间
    
    float _delta;
    LoadingBar* _hpBar; // 血条
    
	bool _isRunning;
    bool _isSelect;
    bool _isbroken;
    int _dir;
    Armature* _arm;
    Armature* _skill1;
    Armature* _skill2;
    
    int _state;         // 状态
    Vec2 _targetPos;
	Vec2 _startPos;
	ITarget* _pTarget;
	int _tickTimer;
	bool _isRunAttack;		// A到目标点
	
    float _expReward;
	int _camp;
	Vec2 _pos;		// 

	void AddToManager();	// 需要废弃

	int getVison();
	int getDir(Vec2 delta);	
	int _InConcat;
	//Vec2 _concatPostSolvePos;
	Vec2 convertToTileView(Vec2 pos);
	void SetZOrder();
	Vec2 _posFollowTarget;
};
bool SpritesHaveIntersect(Hero* pHero);
extern std::vector<Hero*> g_Heros;
#endif // __Hero_H__
