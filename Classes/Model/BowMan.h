#ifndef __BOWMAN_H__
#define __BOWMAN_H__

#include "Soilder.h"
class CBowMan : public Soilder
{
public:
	virtual bool init(int soilderID, Vec2 pos);
	static CBowMan* create(int soilderID, Vec2 pos);
protected:
	virtual void loadArm();
	virtual void atk(ITarget* pTarget[], int size_target);
};
#endif