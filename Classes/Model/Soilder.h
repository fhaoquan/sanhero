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
    void loadData();    // ��������
    void showUI();      // ��ʾUI
	void showMe();
    void addHPBar();    // ��Ѫ��
	virtual Vec2 getCenter();

};

#endif // __Soilder_H__
