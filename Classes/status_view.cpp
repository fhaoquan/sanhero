#include "status_view.h"
#include "Utils/Config.h"
#include "Utils/stdafx.h"
#include <string>
#include "Model/Hero.h"

CStatusView::CStatusView()
{

}

CStatusView::~CStatusView()
{

}

CHeroStatusView::CHeroStatusView()
{

}

CHeroStatusView::~CHeroStatusView()
{

}

bool CHeroStatusView::init()
{
	if (!Layer::init()) {
		return false;
	}

	// background
	auto bg = DrawNode::create();
	bg->drawSolidRect(Vec2(0, 0), Size(120,80), Color4F::WHITE);
	addChild(bg);

	auto ui = GUIReader::getInstance()->widgetFromJsonFile(UI_DIALOG_BATTLEHUD);
	this->addChild(ui, 0, "UI");
	Size s = ui->getContentSize();
	ui->setContentSize(Size(100, 80));

	auto btn0 = (Button*)Helper::seekWidgetByName(ui, "Button0");
	auto btn1 = (Button*)Helper::seekWidgetByName(ui, "Button1");
	auto btn2 = (Button*)Helper::seekWidgetByName(ui, "Button2");
	auto btn3 = (Button*)Helper::seekWidgetByName(ui, "Button3");
	auto btn4 = (Button*)Helper::seekWidgetByName(ui, "Button4");
	btn0->setBright(true);
	btn1->setBright(false);
	btn2->setBright(false);
	btn3->setBright(false);
	btn4->setBright(false);


	
	for (const auto child : ui->getChildren())
	{
		std::string strName = child->getName();
		std::transform(strName.begin(), strName.end(), strName.begin(), ::tolower);
		if (strName.find("skill") != std::string::npos)
			child->setVisible(false);
		else
		{
			child->setVisible(false);
		}
	}

	// skill
	//auto skill1 = (Button*)Helper::seekWidgetByName(ui, "Skill1");
	//auto skill2 = (Button*)Helper::seekWidgetByName(ui, "Skill2");
	//skill1->setScale(0.6f, 0.6f);
	//Vec2 pos = skill1->getPosition();
	//pos.x -= 1000;
	//skill1->setPosition(pos);
	//skill2->setScale(0.6f, 0.6f);

	auto mybutton1 = Button::create("images/skill/Skill_30002.png");
	mybutton1->setScale(0.6f, 0.6f);
	mybutton1->setPosition(Vec2(20, 20));
	mybutton1->setName("Skill1");
	addChild(mybutton1);
	mybutton1->addTouchEventListener(CC_CALLBACK_2(CHeroStatusView::skillCallback, this));
	auto mybutton2 = Button::create("images/skill/Skill_30003.png");
	mybutton2->setScale(0.6f, 0.6f);
	mybutton2->setPosition(Vec2(60, 20));
	mybutton2->setName("Skill2");
	addChild(mybutton2);
	mybutton2->addTouchEventListener(CC_CALLBACK_2(CHeroStatusView::skillCallback, this));

	std::string str_btn[] = { "attack", "stop", "move" };
	for (int i = 0; i < 3; i++) {
		auto btn = Button::create(str_btn[i] + ".png");
		btn->setPosition(Vec2(20+i*btn->getContentSize().width, 60));
		addChild(btn);
	}
	return true;
}

void CHeroStatusView::skillCallback(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED) {
		auto btn = (Button*)sender;
		auto name = btn->getName();

		for (auto child : _mapLayer->getChildren()) {
			if (child->getName() == "hero") {
				Hero* p = (Hero*)child;
				if (p->isSelected()) {
					if (name == "Skill1") {
						p->putSkill(1);
					}
					if (name == "Skill2") {
						p->putSkill(2);
					}
				}
			}

		}
	}
}

void CHeroStatusView::setMapLayer(Layer* layer)
{
	_mapLayer = layer;
}
