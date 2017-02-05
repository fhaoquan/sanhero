#ifndef __MOVE_PATH_H__
#define __MOVE_PATH_H__

#include <vector>
using namespace std;
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld;
// �������߼�����
namespace MovePath {
vector<Vec2> GetTargetPoss(vector<Vec2> vecUser, Vec2 posTarget);

// �������ߵ�Ŀ������, ���ظ��������Ŀ���
vector<Vec2> Move_Path(const vector<Vec2>& points, Vec2 pos);


// ��0ad���Ѱ·�㷨
class FindPath
{
public: 
	FindPath(HelloWorld* pLayer);
	~FindPath();

	// ���һ�����������·��
	vector<Vec2> getPaths(Vec2 startPos, Vec2 endPos);
private:
	// ���ϰ����rectת��Ϊ�������ϰ�λ��
	void convertObstacleTo0ad(HelloWorld* pLayer);
	vector<Rect> _vecRect;
	HelloWorld* _pLayer;
};

};


#endif