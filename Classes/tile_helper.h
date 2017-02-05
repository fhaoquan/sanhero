#ifndef __TILE_HELPER_H__
#define __TILE_HELPER_H__

//#include "external/tinyxml2/tinyxml2.h"
#include "cocos2d/external/tinyxml2/tinyxml2.h"
#include "cocos2d.h"
using namespace cocos2d;
#include <vector>
using namespace std;

class CTileObject {
public:
	string name;
	int id;
	Vec2 pos;
	Size size;
};

void getTileObjects(vector<CTileObject> & vecObject);

#endif