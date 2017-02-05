#include "tile_helper.h"
using namespace tinyxml2;

void getTileObjects(vector<CTileObject> & vecObject)
{
	string filename = FileUtils::getInstance()->fullPathForFilename("TileMaps/wk.tmx");
	Data data = FileUtils::getInstance()->getDataFromFile(filename);
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	XMLElement* root = doc.RootElement();
	XMLElement* group = root->FirstChildElement("objectgroup");
	if (group) {
		XMLElement *child = group->FirstChildElement();
		while (child) {
			const XMLAttribute* attr = child->FirstAttribute();
			bool have_width = false;
			CTileObject obj;
			while (attr)
			{
				float v = atof(attr->Value());
				string n = attr->Name();
				if (n == "x")
					obj.pos.x = v;
				if (n == "y")
					obj.pos.y = v;
				if (n == "width")
					obj.size.width = v;
				if (n == "height")
					obj.size.height = v;
				if (n == "name")
					obj.name = attr->Value();
				attr = attr->Next();
				if (n == "width")
					have_width = true;
			}
			if (have_width)
				vecObject.push_back(obj);
			child = child->NextSiblingElement();
		}


	}
}
