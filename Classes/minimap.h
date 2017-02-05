#ifndef __MINIMAP_H__
#define __MINIMAP_H__

#include "cocos2d.h"
USING_NS_CC;
#include <map>
using namespace std;


class CMiniMap : public cocos2d::Sprite
{
public:
	CMiniMap();
	virtual ~CMiniMap();
	virtual bool init();
	CREATE_FUNC(CMiniMap);

	void set(Camera* camera, Node* map, Vec2 initPos=Vec2(0,0));

protected:
	virtual bool onTouchBegan(Touch* pTouch, Event* pEvent);
	virtual void onTouchMoved(Touch* pTouch, Event* pEvent);
	virtual void onTouchEnded(Touch* pTouch, Event* pEvent);
	void onMouseUp(cocos2d::EventMouse* event);
	void MyUpdate(float dt);

	void updatePos();		// 根据地图位置，确定小地图当前位置， 移动选择框
private:
	DrawNode* _selected_frame;
	Vec2 _pos;		// click point, select_frame center
	bool isInCortorler(Vec2 pos);
	float _x_scale;
	float _y_scale;

	Size _total_tile_map_size;
	Camera* _camera;
	Node* _map;
	void MoveScene(Vec2 delta);//移动除小地图外的其它元素

	Vec2 converMainViewToMiniMap(Vec2 pos);
	void CreateElement(Vec2 s, int tag,bool visibility, Vec2 pos, const Color4F& color);
	std::map<int, Node*> _mapElements;
};
#endif