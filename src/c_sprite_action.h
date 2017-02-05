#ifndef __C_SPRITE_ACTION_H_
#define __C_SPRITE_ACTION_H_

namespace cocos2d {
	class Vec2;
	class Sprite;
}
struct ISpriteAction;
// 位置描述
struct ITarget {
	// 如果对象是sprite，那么通过sprite计算位置
	virtual Vec2 getCenter() = 0;
	virtual bool isDeath() = 0;
	virtual ISpriteAction* GetAction() = 0;
	virtual Sprite* GetSprite() = 0;
	virtual int getCamp() = 0;
	virtual void setCamp(int camp) = 0;
};
//技能与普通攻击产生的伤害是不一样的, 因此需要攻击接口
struct IAttack {	
	virtual bool canAttack() = 0;		// 能否攻击， 由CD或其它因素影响
	virtual float getAttackHurt() = 0;
	virtual void notifyTargetDeath() = 0; // 目标已死亡
};
struct ISpriteAction {
	virtual void idle() = 0;
	virtual void run(ITarget* pTarget, bool allow_attack=false) = 0;		// 固定点 或跟随, A or R
	virtual void atk(ITarget* pTarget[], int size_target) = 0;   // 攻击, size_target=1为dps，>1为AOE
	virtual void hurt(IAttack* pAttacker) = 0;           // 受伤, 根据攻击公式计算最后的伤害
	virtual void die() = 0;             // 死亡
	virtual void putSkill(int SkillId) = 0;    // 释放技能
	//virtual void SkillFinishedCallBack(int type) = 0;		// ? 技能结束时回调
};

struct IAI {
	virtual ITarget* findAttackTarget() = 0;		// 计算需要攻击的目标, 1.可视 2.可攻击 3.多目标选择最近的
};

struct IBuildAction {
	virtual void Produce() = 0;			// 生产
	//virtual void setRallyPoint(Vec2 pos) = 0;	// 设置集结点	由run提供
	virtual float getProduceProcess() = 0;	// 获得当前生产进度
};

class CSpriteAction {
public:
	CSpriteAction(void);
	~CSpriteAction(void);
};

#endif	// __C_SPRITE_ACTION_H_	
		