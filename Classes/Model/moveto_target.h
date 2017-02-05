#ifndef __MOVETO_TARGET_H__
#define __MOVETO_TARGET_H__

#include "Hero.h"
// 对于多个目标向一个目标点移动， 不能靠碰撞后再确定位置， 而是预先计算出不会碰到的位置，再移动
Vec2 calcTargetPos(Hero* pHero);


#endif 
