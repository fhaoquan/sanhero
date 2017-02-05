#ifndef __C_SPRITE_ACTION_H_
#define __C_SPRITE_ACTION_H_

namespace cocos2d {
	class Vec2;
	class Sprite;
}
struct ISpriteAction;
// λ������
struct ITarget {
	// ���������sprite����ôͨ��sprite����λ��
	virtual Vec2 getCenter() = 0;
	virtual bool isDeath() = 0;
	virtual ISpriteAction* GetAction() = 0;
	virtual Sprite* GetSprite() = 0;
	virtual int getCamp() = 0;
	virtual void setCamp(int camp) = 0;
};
//��������ͨ�����������˺��ǲ�һ����, �����Ҫ�����ӿ�
struct IAttack {	
	virtual bool canAttack() = 0;		// �ܷ񹥻��� ��CD����������Ӱ��
	virtual float getAttackHurt() = 0;
	virtual void notifyTargetDeath() = 0; // Ŀ��������
};
struct ISpriteAction {
	virtual void idle() = 0;
	virtual void run(ITarget* pTarget, bool allow_attack=false) = 0;		// �̶��� �����, A or R
	virtual void atk(ITarget* pTarget[], int size_target) = 0;   // ����, size_target=1Ϊdps��>1ΪAOE
	virtual void hurt(IAttack* pAttacker) = 0;           // ����, ���ݹ�����ʽ���������˺�
	virtual void die() = 0;             // ����
	virtual void putSkill(int SkillId) = 0;    // �ͷż���
	//virtual void SkillFinishedCallBack(int type) = 0;		// ? ���ܽ���ʱ�ص�
};

struct IAI {
	virtual ITarget* findAttackTarget() = 0;		// ������Ҫ������Ŀ��, 1.���� 2.�ɹ��� 3.��Ŀ��ѡ�������
};

struct IBuildAction {
	virtual void Produce() = 0;			// ����
	//virtual void setRallyPoint(Vec2 pos) = 0;	// ���ü����	��run�ṩ
	virtual float getProduceProcess() = 0;	// ��õ�ǰ��������
};

class CSpriteAction {
public:
	CSpriteAction(void);
	~CSpriteAction(void);
};

#endif	// __C_SPRITE_ACTION_H_	
		