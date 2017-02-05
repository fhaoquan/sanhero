#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "network/SocketIO.h"
#include <vector>
#include "tile_helper.h"

class Building;
class HelloWorld : public cocos2d::Layer, cocos2d::network::SocketIO::SIODelegate
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event  *event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onMouseUp(cocos2d::EventMouse* event);
	void createArmy(cocos2d::Ref* pSender);
	void testSocketIo(cocos2d::Ref* pSender);
	bool onContactBegin(cocos2d::PhysicsContact& contact);

	// SIODelegate
	virtual void onConnect(cocos2d::network::SIOClient* client);
	virtual void onMessage(cocos2d::network::SIOClient* client, const std::string& data);
	virtual void onClose(cocos2d::network::SIOClient* client);
	virtual void onError(cocos2d::network::SIOClient* client, const std::string& data);

	// Server CallBack
	void findAllCallBack(cocos2d::network::SIOClient* client, const std::string& data);
	void createCallBack(cocos2d::network::SIOClient* client, const std::string& data);
	void removeCallBack(cocos2d::network::SIOClient* client, const std::string& data);
	void modifyCallBack(cocos2d::network::SIOClient* client, const std::string& data);

	cocos2d::Vec2 TileConvertToViewPos(cocos2d::Vec2 pos);
	cocos2d::Vec2 ViewConvertToTilePos(cocos2d::Vec2 pos);

	void addSoilder();
	void updatePepole();
	
	vector<cocos2d::Rect> getTileObstacle();
private:
	cocos2d::Sprite* sprite_;
	cocos2d::Vec2 touch_pos_begin_;
	cocos2d::DrawNode* drawTouchFrame_;
	cocos2d::network::SIOClient* _sioClient;
	//void update(float dt);
	cocos2d::Vec2 _TouchStartPos;
	bool HaveSelected();
	cocos2d::TMXTiledMap* getMap();
	cocos2d::Node* getCamera();

	void addObstacle();
	std::vector<CTileObject> _vecTileObject;
	int _soilder_id;
	Building* _mycity;

	bool IsInMapLayer(Vec2 pos);
};

#endif // __HELLOWORLD_SCENE_H__
