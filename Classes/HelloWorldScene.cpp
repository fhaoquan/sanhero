#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Model/Hero.h"
#include "Model/Soilder.h"
#include "Model/BowMan.h"
#include "Classes/mybuilding.h"
USING_NS_CC;
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
using namespace cocostudio;
using namespace cocos2d::ui;
#include "frame_proxy.h"
#include "Model/mytest.h"
#include "minimap.h"
#include "tile_to_view.h"
#include "tile_helper.h"
#include "status_view.h"
#include "move_path.h"

enum
{
	kMouseClick,
	kMiniMap,
	kCamera,
	kTagTileMap,
	kStatusView,
	kPepoleView,

	kArmy1=1000,
	kCity1=2000,
};
int g_kCamera = (int)kCamera;

#include "Utils/Config.h"


Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    //auto scene = Scene::create();
	auto scene = Scene::createWithPhysics();
    
    // 'layer' is an autorelease object
    //auto layer = SanWorld::create();
	auto layer = HelloWorld::create();


    // add layer as a child to scene
    scene->addChild(layer);

	Size visible_size = CCDirector::getInstance()->getWinSize();
	auto camera = Camera::createOrthographic(800,600,0,1);
	camera->setCameraFlag(CameraFlag::USER1);
	camera->setTag(kCamera);
	layer->addChild(camera);
	camera->setPosition(-1400, -200);
	layer->setCameraMask(2);

	//auto ob_layer = Layer::create();
	auto mini_map = CMiniMap::create();
	mini_map->setPosition( visible_size- Size(140, 80));
	//mini_map->setPositionY(mini_map->getPositionY() + 20);
	//mini_map->setPositionX(mini_map->getPositionX() - 100);
	mini_map->setTag(kMiniMap);
	//ob_layer->addChild(mini_map, 10000);
	//scene->addChild(ob_layer);
	scene->addChild(mini_map);
	mini_map->set(camera, layer->getChildByTag(kTagTileMap), Vec2(10, 60));
	mini_map->setContentSize(Size(240, 120));
	
	// add UI
	auto view = CHeroStatusView::create();
	view->setTag(kStatusView);
	view->setContentSize(Size(100, 80));
	view->setPosition(visible_size - Size(130, visible_size.height - 10));
	Size size_view = view->getContentSize();
	scene->addChild(view);
	view->setMapLayer(layer);

	// pepole ui
	auto label = Label::createWithTTF("1/30", "fonts/Marker Felt.ttf", 24);
	Size label_size = label->getContentSize();
	label->setString("5/30");
	label->setTag(kPepoleView);
	label->setPosition(Vec2(label_size.width+2,visible_size.height-label_size.height-2));
	scene->addChild(label);

    // return the scene
    return scene;
}

Vec2 convertToMapPos(Vec2 pos) {
	float x = pos.x;
	float y = 1920 - pos.y;
	return Vec2(x, y);
}

// 45度tile
// OpenGL坐标转成格子坐标
Vec2 staggeredCoordForPosition(TMXTiledMap* tiledMap, Vec2 position)
{
	Size mapSize = tiledMap->getMapSize();
	Size tileSize = tiledMap->getTileSize();
	int y = mapSize.height - 2 - ((2 * (int)position.y) / (int)tileSize.height);
	float x = position.x / tileSize.width - (y % 2) / 2.0f;
	return Vec2(x, y);
}
// tile坐标转成瓦片格子中心的OpenGL坐标
Vec2 positionForStaggeredCoord(TMXTiledMap* tiledMap, const Vec2& StaggeredCoord)
{
	Size mapSize = tiledMap->getMapSize();
	Size tileSize = tiledMap->getTileSize();
	int x = StaggeredCoord.x*tileSize.width + ((int)StaggeredCoord.y % 2)*tileSize.width / 2;
	int y = (mapSize.height - (StaggeredCoord.y + 1))*tileSize.height / 2 - tileSize.height / 2;
	return Vec2(x, y);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    //auto sprite = Sprite::create("HelloWorld.png");

    //// position the sprite on the center of the screen
    //sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    //// add the sprite as a child to this layer
    //this->addChild(sprite, 0);

	// load listener
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	listener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto mouse_listener = EventListenerMouse::create();
	mouse_listener->onMouseMove = [this](Event* event) {
		EventMouse* e = (EventMouse*)event;
		Vec2 pos_e = e->getLocationInView();
		HWND hwnd = FindWindowExA(NULL, NULL, "PythonDocSearch", "Dynamic Dialog Demo");
		bool haveOver = false;
		for (unsigned i = 0; i < g_Heros.size(); i++) {
			Hero* p = g_Heros[i];
			Vec2 anchorPoint = p->getAnchorPoint();
			Size s = p->getContentSize();
			Vec2 pos = p->getPosition();
			pos.x -= s.width;
			pos.y -= s.height;
			Rect rc(pos, s);
			if (rc.containsPoint(e->getLocationInView())) {
				haveOver = true;
				if (IsWindow(hwnd)) {
					pos = p->getPosition();
					PostMessageA(hwnd, WM_USER + 514, MAKEWPARAM(pos_e.y, pos_e.x), MAKELPARAM(pos.y,pos.x));
				}
				break;
			}

		}
		if (!haveOver) {
			if (IsWindow(hwnd)) {
				PostMessageA(hwnd, WM_USER + 514, MAKEWPARAM(pos_e.y, pos_e.x),0);
			}
		}
		else
		{
		}


	};
	mouse_listener->onMouseUp = CC_CALLBACK_1(HelloWorld::onMouseUp, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);
	// setcursor
	InstallFrameMessageHanlder([this](unsigned int x, unsigned int y) {
		for (unsigned i = 0; i < g_Heros.size(); i++) {
			Hero* p = g_Heros[i];
			Vec2 anchorPoint = p->getAnchorPoint();
			Size s = p->getContentSize();
			Vec2 pos = p->getPosition();
			pos.x -= s.width;
			pos.y -= s.height;
			Vec2 pos_point = Vec2(x, y);
			auto camera = getCamera();
			if (camera)
				pos_point += camera->getPosition();
			Rect rc(pos, s);
			//CCLOG("%d,%d", x, y);
			if (rc.containsPoint(pos_point) && !p->isDeath()) {
				return p->getCamp();
			}

		}

		return -1;
	});

	// add map
	auto map = TMXTiledMap::create("TileMaps/wk.tmx");
	//auto map = TMXTiledMap::create("TileMaps/iso-test-vertexz.tmx");
	Size total_tile_map_size = Size(map->getTileSize().width*map->getMapSize().width, map->getTileSize().height*map->getMapSize().height);
	Vec2 cur_anchor_pos = map->getAnchorPoint();
	map->setPosition(-1*total_tile_map_size/2);
	addChild(map, 0, kTagTileMap);
	Size map_size = map->getContentSize();

	addObstacle();


	// draw mouse click point and move line
	auto point_mouse_click = DrawNode::create(1);
	point_mouse_click->drawCircle(Vec2(0, 0), 10, CC_DEGREES_TO_RADIANS(90), 30, false, Color4F::GREEN);
	point_mouse_click->setTag(kMouseClick);
	point_mouse_click->setPosition(Vec2(0, 0));
	addChild(point_mouse_click, 100);

	//// add card
	//auto card1 = MenuItemImage::create("san11res1_112-1.png","san11res1_112-1.png", CC_CALLBACK_1(HelloWorld::createArmy, this));
	//card1->setScale(.5, .5);
	//card1->setPosition(Vec2(100, 60));
	////addChild(card1, 1, 1);
	//auto card2 = MenuItemImage::create("san11res1_108-1.png", "san11res1_108-1.png", CC_CALLBACK_1(HelloWorld::testSocketIo, this));
	//card2->setScale(.5, .5);
	//card2->setPosition(Vec2(220, 60));
	////addChild(card1, 1, 2);

	//auto menu2 = Menu::create(card1,card2, NULL);
	//menu2->setPosition(Vec2::ZERO);
	//this->addChild(menu2, 1);

	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/Anim_Bowman/Anim_Bowman0.plist");
	//auto sprite = Sprite::createWithSpriteFrameName("animation/Anim_Bowman/Anim_Bowman0.png");
	//sprite_= Sprite::create("images/soilder/bowman/bowman_01.png");
	//sprite_->setPosition(Vec2(300, 300));
	//addChild(sprite_, 5, 100);

	drawTouchFrame_ = DrawNode::create();
	addChild(drawTouchFrame_, 10);

	//schedule(schedule_selector(HelloWorld::update), 1.0f);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_HERO_ARAGORN);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_FIGHTER);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_BOWMAN);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_GUNNER);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_MEATSHIELD);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_SKILL_1);
	ArmatureDataManager::getInstance()->addArmatureFileInfo(ANIM_SKILL_2);

	//auto contactListener = EventListenerPhysicsContact::create();
	//contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	// 边界
	//auto node = Node::create();
	//node->addComponent(PhysicsBody::createEdgeBox(visibleSize));
	//node->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	//this->addChild(node, 4);

	// add hero
	Vec2 hero_pos = Vec2(100, 200);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto hero = Hero::create(22, hero_pos);
	addChild(hero, 100);
	Vec2 pos = map->getPosition();
	hero_pos = ViewConvertToTilePos(hero_pos);

	hero_pos = Vec2(140, -10);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto hero2 = Hero::create(23, hero_pos);
	addChild(hero2, 100);
	hero_pos = Vec2(24*32, 1920-28*32);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto hero3 = Hero::create(24, hero_pos);
	addChild(hero3, 100);

	_soilder_id = 300;
	hero_pos = Vec2(10, 20);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto soilder = Soilder::create(_soilder_id++, hero_pos);
	addChild(soilder, 1);

	hero_pos = Vec2(400, 400);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto enemy_soilder = Soilder::create(_soilder_id++, hero_pos);
	enemy_soilder->setCamp(BaseSprite::Camp::Enemy);
	addChild(enemy_soilder, 1);

	hero_pos = Vec2(300, 100);
	hero_pos = TileConvertToViewPos(hero_pos);
	auto enemy_soilder2 = CBowMan::create(_soilder_id++, hero_pos);
	//enemy_soilder2->setCamp(BaseSprite::Camp::Enemy);
	addChild(enemy_soilder2, 1);

	//hero_pos = Vec2(10, 400);
	//hero_pos = TileConvertToViewPos(hero_pos);
	//auto friend_soilder = Soilder::create(2, hero_pos);
	//friend_soilder->setCamp(1);
	//addChild(friend_soilder, 1);
	
	addSoilder();

	// sio
	//_sioClient = network::SocketIO::connect("http://192.168.3.6:3000/", *this);
	//_sioClient->setTag("cocos2d-x Client1");
	//_sioClient->on("findAllCallBack", CC_CALLBACK_2(HelloWorld::findAllCallBack, this));
	//_sioClient->on("createCallBack", CC_CALLBACK_2(HelloWorld::createCallBack, this));
	//_sioClient->on("removeCallBack", CC_CALLBACK_2(HelloWorld::removeCallBack, this));
	//_sioClient->on("modifyCallBack", CC_CALLBACK_2(HelloWorld::modifyCallBack, this));
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}

void HelloWorld::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	_TouchStartPos = touches[0]->getLocation();
	_TouchStartPos += getCamera()->getPosition();
}

void HelloWorld::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	auto touch = touches[0];
	//
	//auto diff = touch->getDelta();
	//for (int i = kCity1; i <= kTagTileMap; i++) {
	//	auto node = getChildByTag(i);
	//	auto currentPos = node->getPosition();
	//	node->setPosition(currentPos + diff);
	//}

	// draw a rect
	drawTouchFrame_->clear();
	Vec2 pos_start = touch->getStartLocation();
	pos_start += getCamera()->getPosition();
	Vec2 pos_end = touch->getLocation();
	pos_end += getCamera()->getPosition();
	drawTouchFrame_->drawRect(pos_start, pos_end, Color4F::YELLOW);

}

void HelloWorld::createArmy(cocos2d::Ref* pSender)
{
	auto army1 = Sprite::create("army1_1.png");
	Vec2 pos = getChildByTag(kCity1)->getPosition();
	army1->setPosition(pos);
	addChild(army1, 2, kArmy1);

	// Animation
	Animation* animation = Animation::create();
	for (int i = 1; i < 76; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "images/soilder/bowman/bowman_%02d.png", i);
		animation->addSpriteFrameWithFile(szName);
	}
	animation->setDelayPerUnit(.15f);
	animation->setRestoreOriginalFrame(true);
	auto action = Animate::create(animation);
	sprite_->runAction(action);
	auto s = Director::getInstance()->getWinSize();

	auto array = PointArray::create(20);

	array->addControlPoint(Vec2(0, 0));
	array->addControlPoint(Vec2(s.width / 2 - 30, 0));
	array->addControlPoint(Vec2(s.width / 2 - 30, s.height/2 - 80));
	array->addControlPoint(Vec2(0, s.height/2 - 80));
	array->addControlPoint(Vec2(0, 0));
	auto action2 = CardinalSplineBy::create(3, array, 0);
	action2->setDuration(10);
	auto reverse = action2->reverse();
	auto seq = Sequence::create(action2, reverse, nullptr);
	sprite_->runAction(seq);

}


void HelloWorld::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event)
{
	drawTouchFrame_->clear();
	//EventMouse* e = (EventMouse*)event;
	//Vec2 pt = e->getLocation();
	//Vec2 pt2 = touches[0]->getLocation();
	//getChildByTag(kMouseClick)->setPosition(convertTouchToNodeSpace(touches[0]));



}

//void HelloWorld::update(float dt)
//{
//	CCLOG("%s %f", __FUNCTION__, dt);
//}

void HelloWorld::onConnect(cocos2d::network::SIOClient* client)
{
	CCLOG("onconnect");
}

void HelloWorld::onMessage(cocos2d::network::SIOClient* client, const std::string& data)
{
	CCLOG("onmessage");
}

void HelloWorld::onClose(cocos2d::network::SIOClient* client)
{
	if (client == _sioClient) {
		_sioClient = nullptr;
	}
}

void HelloWorld::onError(cocos2d::network::SIOClient* client, const std::string& data)
{

}

void HelloWorld::findAllCallBack(cocos2d::network::SIOClient* client, const std::string& data)
{
	for (unsigned int i = 0; i < strlen(data.c_str()); i++) {
		CCLOG("find data %02x", data.at(i));
	}
	
}

void HelloWorld::createCallBack(cocos2d::network::SIOClient* client, const std::string& data)
{
	CCLOG("createCallBack");
}

void HelloWorld::removeCallBack(cocos2d::network::SIOClient* client, const std::string& data)
{
	CCLOG("removeCallBack");
}

void HelloWorld::modifyCallBack(cocos2d::network::SIOClient* client, const std::string& data)
{
	CCLOG("modifyCallBack");
}

void HelloWorld::testSocketIo(cocos2d::Ref* pSender)
{
	_sioClient->emit("create", "{\"a\":1}");
	_sioClient->emit("modify", "{\"a\":2}");
	_sioClient->emit("findAll", "{}");
	_sioClient->emit("remove", "{\"a\":2}");
}

bool HelloWorld::onContactBegin(cocos2d::PhysicsContact& contact)
{
	return true;
}

void HelloWorld::onMouseUp(cocos2d::EventMouse* event)
{
	// mouse left click
	int button_id = event->getMouseButton();
	if (button_id == 0) {
		// Judge select sprite
		Vec2 pos1 = event->getLocationInView();
		if (!IsInMapLayer(pos1))
			return;
		pos1 += getCamera()->getPosition();
		Size s = Size(pos1 - _TouchStartPos);
		float x = MIN(pos1.x, _TouchStartPos.x);
		float y = MIN(pos1.y, _TouchStartPos.y);
		float width = MAX(fabs(s.width), 1);
		float height = MAX(fabs(s.height), 1);
		Rect rectTouch(x, y, width, height);
		bool have_selected = false;
		for (unsigned i = 0; i < g_Heros.size(); i++) {
			Hero* p = g_Heros[i];
			Vec2 pos = p->getPosition();
			s = p->getContentSize();
			Rect rcMe(pos.x - s.width, pos.y - s.height, fabs(s.width), fabs(s.height));
			if (rectTouch.intersectsRect(rcMe) && p->getCamp() == BaseSprite::Camp::Me) {
				have_selected = true;
				break;
			}
		}
		if (have_selected) {
			for (unsigned i = 0; i < g_Heros.size(); i++) {
				Hero* p = g_Heros[i];
				Vec2 pos = p->getPosition();
				s = p->getContentSize();
				Rect rcMe(pos.x - s.width, pos.y - s.height, fabs(s.width), fabs(s.height));
				if (rectTouch.intersectsRect(rcMe) && p->getCamp() == BaseSprite::Camp::Me) {
					p->setSelect(true);
				}
				else
				{
					p->setSelect(false);
				}
			}
		}

	}

	// Right
	if (button_id == 1) {
		auto camera = getChildByTag(kCamera);
		Vec2 pos = event->getLocationInView();
		pos += camera->getPosition();
		if (HaveSelected())
			getChildByTag(kMouseClick)->setPosition(pos);

		// 人物的移动及攻击都统一到这里处理
		bool bFollow = false;
		Hero* pFollowTarget = nullptr;
		vector<Hero*> vecSelected;
		for (auto child : getChildren()) {
			if (child->getName() == "hero" || child->getName() == "soilder") {
				Hero* p = (Hero*)child;
				if (p->isSelected()) {
					vecSelected.push_back(p);
				}
				if (!bFollow) {
					bFollow = p->IsClickMe(pos);
					if (bFollow)
						pFollowTarget = p;
				}
			}
		}
		if (bFollow) {
			for (unsigned i = 0; i < vecSelected.size(); i++) {
				vecSelected[i]->run(pFollowTarget);
			}
		}
		else
		{
			if (vecSelected.size() > 2) {
				// 集体行走
				vector<Vec2> vecSelected_pos;
				for (unsigned i = 0; i < vecSelected.size(); i++) {
					vecSelected_pos.push_back(vecSelected[i]->getCenter());
				}
				vecSelected_pos = MovePath::Move_Path(vecSelected_pos, pos);
				for (unsigned i = 0; i < vecSelected.size(); i++) {
					CTarget target(vecSelected_pos[i]);
					vecSelected[i]->run(&target);
				}
			}
			else
			{
				for (unsigned i = 0; i < vecSelected.size(); i++) {
					CTarget target(pos);
					vecSelected[i]->run(&target);
				}
			}
		}

	}
}

bool HelloWorld::HaveSelected()
{
	for (auto hero : g_Heros) {
		if (hero->IsSelected())
			return true;
	}
	return false;
}

cocos2d::Vec2 HelloWorld::TileConvertToViewPos(Vec2 pos)
{
	CTileViewConverter converter(Size(1920, 1920), getChildByTag(kTagTileMap)->getContentSize());
	return converter.tile_to_view(pos.x, pos.y) + getChildByTag(kTagTileMap)->getPosition();

}

cocos2d::Vec2 HelloWorld::ViewConvertToTilePos(Vec2 pos)
{
	CTileViewConverter converter(Size(1920, 1920), getChildByTag(kTagTileMap)->getContentSize());
	pos -= getChildByTag(kTagTileMap)->getPosition();
	return converter.view_to_tile(pos.x, pos.y) ;

}

cocos2d::TMXTiledMap* HelloWorld::getMap()
{
	return (TMXTiledMap*)getChildByTag(kTagTileMap);
}

cocos2d::Node* HelloWorld::getCamera()
{
	return getChildByTag(kCamera);
}

// 添加障碍层
void HelloWorld::addObstacle()
{
	getTileObjects(_vecTileObject);

	//add city1
	//TMXObjectGroup* group = map->getObjectGroup("objects");
	//ValueMap city1Point = group->getObject("city1");
	//float x = city1Point["x"].asDouble();
	//float y = city1Point["y"].asFloat();
	//float width = city1Point["width"].asFloat();
	float map_height = ((TMXTiledMap*)getMap())->getTileSize().height*getMap()->getMapSize().height;
	for (unsigned i = 0; i < 2; i++) {
		CTileObject& obj = _vecTileObject[i];
		float x = obj.pos.x;
		float y = map_height-obj.pos.y;

		char sz[100];
		sprintf(sz, "City1_%d.png", i + 1);
		//auto city1 = Sprite::create(sz);
		Vec2 pos = TileConvertToViewPos(Vec2(x, y));
		//city1->setPosition(pos);
		auto city1 = Building::create(1000+i, pos);
		if (i == 0) {
			city1->setCamp(BaseSprite::Camp::Enemy);
		}
		else
		{
			_mycity = city1;
		}
		city1->setTag(kCity1 + i);
		addChild(city1, 0, kCity1+i);
	}


	//崩溃，暂时不用
	//MovePath::FindPath findpath(this);
	//findpath.getPaths(Vec2(100, 100), Vec2(600, 600));
}

bool HelloWorld::IsInMapLayer(Vec2 pos)
{
	auto sence = getParent();
	auto view = sence->getChildByTag(kStatusView);
	Vec2 pos1 = view->getPosition();
	Size size = view->getContentSize();
	Rect rc(pos1, size);
	if (rc.containsPoint(pos))
		return false;

	auto mini_map = sence->getChildByTag(kMiniMap);
	pos1 = mini_map->getPosition();
	size = mini_map->getContentSize();
	rc.setRect(pos1.x-size.width/2, pos1.y-size.height/2, size.width, size.height);
	if (rc.containsPoint(pos))
		return false;

	return true;
}

void HelloWorld::addSoilder()
{
	//Vec2 pos_start = getChildByTag(kCity1+1)->getPosition();
	for (int i = 0; i < 50; i++) {
		auto hero = Soilder::create(_soilder_id++, Vec2(-200, -200));
		hero->setVisible(false);
		//hero->setDeath(true);
		addChild(hero, 100);
	}
	//CTarget target(pos);
	//hero->run(&target);

	for (int i = 0; i < 30; i++) {
		auto bowan = CBowMan::create(100 + _soilder_id++, Vec2(-300, -300));
		bowan->setVisible(false);
		//bowan->setDeath(true);
		addChild(bowan, 100);
	}
}

void HelloWorld::updatePepole()
{
	// 更新人口显示
	int number = _mycity->_pepole._pepole_number;
	int limit = _mycity->_pepole._pepole_limit;
	char szText[100];
	sprintf(szText, "%d/%d", number, limit);
	if (getParent()) {
		Label* label = (Label*)getParent()->getChildByTag(kPepoleView);
		if (label)
			label->setString(szText);

	}
}

vector<cocos2d::Rect> HelloWorld::getTileObstacle()
{
	vector<Rect> vec;
	for (unsigned int i = 0; i < _vecTileObject.size(); i++) {
		Rect rc(_vecTileObject[i].pos, _vecTileObject[i].size);
		vec.push_back(rc);
	}
	return vec;
}












