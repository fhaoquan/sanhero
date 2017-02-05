#ifndef __MOVE_PATH_H__
#define __MOVE_PATH_H__

#include <vector>
using namespace std;
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld;
// 处理行走及攻击
namespace MovePath {
vector<Vec2> GetTargetPoss(vector<Vec2> vecUser, Vec2 posTarget);

// 集体行走的目标点计算, 返回各个对象的目标点
vector<Vec2> Move_Path(const vector<Vec2>& points, Vec2 pos);


// 用0ad里的寻路算法
class FindPath
{
public: 
	FindPath(HelloWorld* pLayer);
	~FindPath();

	// 获得一个对象的行走路径
	vector<Vec2> getPaths(Vec2 startPos, Vec2 endPos);
private:
	// 把障碍物的rect转换为单个的障碍位置
	void convertObstacleTo0ad(HelloWorld* pLayer);
	vector<Rect> _vecRect;
	HelloWorld* _pLayer;
};

};


#endif