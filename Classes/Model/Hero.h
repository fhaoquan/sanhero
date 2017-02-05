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
	virtual	bool isDeath();             // �Ƿ�����
	virtual int getCamp();
	virtual void setCamp(int camp);
};
//1.����
//2.���ߣ� ����Ŀ�꣬ ָ��λ��
//3.����
//4.��·
//5.����
//6.����
//7.��λ

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
	virtual void atk(ITarget* pTarget[], int size_target);   // ����, size_target=1Ϊdps��>1ΪAOE
	virtual void hurt(IAttack* pAttacker);           // ����, ���ݹ�����ʽ���������˺�
	virtual void die();             // ����
	virtual void putSkill(int SkillId);    // �ͷż���

    
	// ITarget
	virtual Vec2 getCenter();
	virtual ISpriteAction* GetAction();
	virtual Sprite* GetSprite();
	virtual	bool isDeath();             // �Ƿ�����
	virtual int getCamp();
	virtual void setCamp(int camp);

	// IAttack
	virtual bool canAttack();
	virtual float getAttackHurt();
	virtual void notifyTargetDeath();

	// ���ֿ����ж���Ŀ�꣬ ������ˣ��򼺷�����Ŀ��, �����һ��Ŀ�꣬��ôѡ�������һ��
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
	void loadData();    // ��������
	void showUI();      // ��ʾUI
	void addHPBar();    // ��Ѫ��
	void setPhysicsBody();
	virtual bool onTouchBegan(Touch* pTouch, Event* pEvent);
	virtual void onTouchMoved(Touch* pTouch, Event* pEvent);
	virtual void onTouchEnded(Touch* pTouch, Event* pEvent);
	void onMouseUp(cocos2d::EventMouse* event);

	bool IsInTargetPos();
	int GetStatus();
	void run();
	void attack();
	void finishSkill(Armature* arm, int state); // ���ܽ���

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
	int _Vison;					// �Ӿ�
	int _attack_cd;				//������cd
	unsigned int _pre_attack_tick;		//��һ�ι�����ʱ��
    
    float _delta;
    LoadingBar* _hpBar; // Ѫ��
    
	bool _isRunning;
    bool _isSelect;
    bool _isbroken;
    int _dir;
    Armature* _arm;
    Armature* _skill1;
    Armature* _skill2;
    
    int _state;         // ״̬
    Vec2 _targetPos;
	Vec2 _startPos;
	ITarget* _pTarget;
	int _tickTimer;
	bool _isRunAttack;		// A��Ŀ���
	
    float _expReward;
	int _camp;
	Vec2 _pos;		// 

	void AddToManager();	// ��Ҫ����

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
