#include "moveto_target.h"
#include <vector>
using namespace std;

cocos2d::Vec2 calcTargetPos(Hero* pHero)
{
	// get select object
	vector<Hero*> vecSelected;
	for (unsigned i = 0; i < g_Heros.size(); i++) {
		if (g_Heros[i]->isSelected()) {
			vecSelected.push_back(g_Heros[i]);
		}
	}

	//
	return Vec2(0, 0);
}
