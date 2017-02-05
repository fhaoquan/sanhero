#ifndef __Soilder_H__
#define __Soilder_H__

//#include "..\Utils\Public.h"
#include "..\Utils\stdafx.h"
#include "BaseSprite.h"
#include "Hero.h"

class Soilder : public Hero
{
public:
    virtual bool init(int soilderID, Vec2 pos);
    static Soilder* create(int soilderID, Vec2 pos);

protected:
	virtual void loadArm();
    void loadData();    // 加载数据
    void showUI();      // 显示UI
	void showMe();
    void addHPBar();    // 加血条
	virtual Vec2 getCenter();

};

#endif // __Soilder_H__
