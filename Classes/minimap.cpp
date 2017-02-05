#include "minimap.h"
#include "Model/Hero.h"
#include "Classes/Model/BaseSprite.h"
#include "tile_to_view.h"
//#include <vector>
//using namespace std;

CMiniMap::CMiniMap()
{
	_selected_frame = nullptr;
	_camera = nullptr;
}

CMiniMap::~CMiniMap()
{

}

bool CMiniMap::init()
{
	if (!Sprite::init()) {
		return false;
	}
	setContentSize(Size(240, 120));

	auto bg = DrawNode::create();
	//border->drawRect(getPosition() - getContentSize(), getPosition(), Color4F::RED);
	bg->drawSolidRect(Vec2(0, 0), getContentSize(), Color4F::WHITE);
	addChild(bg);

	//auto map = TMXTiledMap::create("TileMaps/wk_mini.tmx");
	auto map = TMXTiledMap::create("TileMaps/wk.tmx");
	//auto map = Sprite::create("minimap.png");
	map->setPosition(map->getPositionX()+2, map->getPositionY()+2);
	map->setScale(0.08f, 0.08f);
	//map->setRotation(90);
	addChild(map);

	// 
	auto border = DrawNode::create();
	//border->drawRect(getPosition() - getContentSize(), getPosition(), Color4F::RED);
	border->drawRect(Vec2(0, 0), getContentSize(), Color4F::RED);
	addChild(border);

	auto dispatcher = this->getEventDispatcher();
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(CMiniMap::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(CMiniMap::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(CMiniMap::onTouchEnded, this);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//auto mouse_listener = EventListenerMouse::create();
	//mouse_listener->onMouseUp = CC_CALLBACK_1(CMiniMap::onMouseUp, this);
	//dispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);

	schedule(schedule_selector(CMiniMap::MyUpdate), 0.81f);

	return true;
}

// 在addchild后调用
void CMiniMap::set(Camera* camera, Node* map, Vec2 initPos)
{
	if (_camera == nullptr && getParent()) {
		//_camera = Camera::create();
		//_camera->setCameraFlag(CameraFlag::USER1);
		//getParent()->addChild(_camera);
		//_camera->setPosition(0, 0);
	}
	_camera = camera;
	_map = map;

	//_map = map;
	//Size win_size = CCDirector::getInstance()->getWinSize();
	//_view_size = view_size;
	//Size total_tile_map_size = Size(_map->getTileSize().width*_map->getMapSize().width, _map->getTileSize().height*_map->getMapSize().height);
	//_x_scale = total_tile_map_size.width / getContentSize().width;
	//_y_scale = total_tile_map_size.height / getContentSize().height;
	_x_scale = 1 / 0.08f;
	_y_scale = 1 / 0.08f;

	// 
	//_pos = Vec2(100, 100);	// select_frame position
	_pos = initPos;
	updatePos();

	MyUpdate(0.81f);
}

void CMiniMap::updatePos()
{
	//map size to view size
	Size frame_size = CCDirector::getInstance()->getWinSize() / 10;

	if (_selected_frame == nullptr) {
		_selected_frame = DrawNode::create(1);
		_selected_frame->drawRect(Vec2(0, 0), frame_size, Color4F::ORANGE);
		addChild(_selected_frame);
	}
	// 先计算当前视图中心点在map中的百分比偏移
	Vec2 pos_center(_pos.x - frame_size.width / 2, _pos.y - frame_size.height / 2);
	_selected_frame->setPosition(pos_center);

	//MoveScene(-1*Vec2((_pos.x-30)*_x_scale, (_pos.y-30)*_y_scale));
	MoveScene(Vec2((_pos.x-170)*_x_scale, (_pos.y-105)*_y_scale));
	//float x_percent = _pos.x / getContentSize().width;
	//float y_percent = _pos.y / getContentSize().height;
	//Size total_tile_map_size = Size(_map->getTileSize().width*_map->getMapSize().width, _map->getTileSize().height*_map->getMapSize().height);
	//MoveScene(-1 * Vec2(x_percent*total_tile_map_size.width, y_percent*total_tile_map_size.height));
}

void CMiniMap::MoveScene(Vec2 delta)
{
	auto diff = delta;
	auto layer = getParent();
	//Vector<Node*> vec = layer->getChildren();
	//for (int i = 0; i < vec.size(); i++) {
	//	auto node = vec.at(i);
	//	if ((Node*)this != node) {
	//		//Vec2 pos = node->getPosition();
	//		node->setPosition(delta);
	//	}
	//}
	//_map->setPosition(delta);
	if (_camera)
		_camera->setPosition(delta.x, delta.y);

}

bool CMiniMap::onTouchBegan(Touch* pTouch, Event* pEvent)
{
	return true;
}

void CMiniMap::onTouchMoved(Touch* pTouch, Event* pEvent)
{
	Vec2 pos = convertToNodeSpace(pTouch->getLocation());
	if (isInCortorler(pos)) {
		_pos = pos;

		updatePos();
	}
;
}

void CMiniMap::onTouchEnded(Touch* pTouch, Event* pEvent)
{
	Vec2 pos = convertToNodeSpace(pTouch->getLocation());
	if (isInCortorler(pos)) {
		_pos = pos;

		updatePos();

	}

}

bool CMiniMap::isInCortorler(Vec2 pos)
{
	
	Size size = getContentSize();
	if ((pos.x > 0 && pos.x < size.width) && (pos.y>0 && pos.y < size.height))
		return true;
	return false;
}

void CMiniMap::MyUpdate(float dt)
{
	// 显示小地图上的目标点
	for (unsigned i = 0; i < g_Heros.size(); i++) {
		Hero* p = g_Heros[i];
		Vec2 pos = p->getPosition();
		//3000:1500=>240:120
		pos = converMainViewToMiniMap(pos);
		Vec2 pos2 = Vec2(4, 4);
		if (p->GetID() >= 1000) {
			pos2 = Vec2(8, 8);
			pos -= Vec2(8, 8);
		}
		if (_mapElements.find(p->GetID()) != _mapElements.end()) {
			DrawNode* e = (DrawNode*)_mapElements[p->GetID()];
			Color4F color = p->getCamp() == BaseSprite::Camp::Me ? Color4F::BLUE : Color4F::RED;
			e->clear();
			e->drawSolidRect(Vec2(0,0), pos2, color);
			e->setPosition(pos);
			e->setVisible(p->isVisible());
		}
		else {
			int camp = p->getCamp();
			CreateElement(pos2, p->GetID(), p->isVisible(), pos, camp == BaseSprite::Camp::Me ? Color4F::BLUE: Color4F::RED);
		}
	}
}

void CMiniMap::CreateElement(Vec2 s, int tag, bool visibility, Vec2 pos, const Color4F& color)
{
	auto e = DrawNode::create();
	Vec2 pos_start = Vec2(0, 0);
	Vec2 pos_end = pos_start + s;
	e->drawSolidRect(pos_start, pos_end, color);
	e->setPosition(pos);
	e->setVisible(visibility);
	addChild(e);
	_mapElements.insert(std::pair<int, Node*>(tag, e));
}

cocos2d::Vec2 CMiniMap::converMainViewToMiniMap(Vec2 pos)
{
	pos = _map->convertToNodeSpace(pos);
	pos.x = pos.x * getContentSize().width / _map->getContentSize().width;
	pos.y = pos.y * getContentSize().height / _map->getContentSize().height;
	return pos;
}

void CMiniMap::onMouseUp(cocos2d::EventMouse* event)
{
	int button_id = event->getMouseButton();
	if (button_id == 1) {
		// 计算比例
		Vec2 posMouse = event->getLocationInView();
		Vec2 pos = getPosition();
		Size s = getContentSize();
		posMouse = convertToNodeSpace(posMouse);
		float x = posMouse.x / s.width;
		float y = posMouse.y / s.height;

	}
}
