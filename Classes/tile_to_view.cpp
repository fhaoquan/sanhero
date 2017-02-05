#include "tile_to_view.h"
#include <math.h>

#define PI 3.141592653
#define dutorad(X) ((X)/180*PI)
#define radtodu(X) ((X)/PI*180)

CTileViewConverter::CTileViewConverter(Size tile_size, Size view_size)
{
	_tile_size = tile_size;
	_view_size = view_size;
	Vec2 pos = coord_rotation(_tile_size.width, 0, _tile_size.width / 2, _tile_size.height / 2, -45);
	Vec2 pos2 = coord_rotation(0, 0, _tile_size.width / 2, _tile_size.height / 2, -45);
	_offset.set(pos.y, pos2.x);
}

CTileViewConverter::~CTileViewConverter()
{

}

cocos2d::Vec2 CTileViewConverter::tile_to_view(Vec2 pos)
{
	return tile_to_view(pos.x, pos.y);
}

cocos2d::Vec2 CTileViewConverter::tile_to_view(float x, float y)
{
	Vec2 pos_org(_tile_size.width / 2, _tile_size.height / 2);
	Vec2 pos = coord_rotation(x, y, pos_org.x, pos_org.y, -45);
	return scale_to_view(pos.x, pos.y);
}

cocos2d::Vec2 CTileViewConverter::view_to_tile(Vec2 pos)
{
	return view_to_tile(pos.x, pos.y);
}

cocos2d::Vec2 CTileViewConverter::view_to_tile(float x, float y)
{
	Vec2 pos_org(_tile_size.width / 2, _tile_size.height / 2);
	Vec2 pos = scale_to_tile(x, y);
	return coord_rotation(pos.x, pos.y, pos_org.x, pos_org.y, 45);

}

cocos2d::Vec2 CTileViewConverter::coord_rotation(float x1, float y1, float x0, float y0, float angle)
{
	float sn = std::sin(dutorad(angle));
	float cs = std::cos(dutorad(angle));
	float x2 = (x1 - x0)*cs - (y1 - y0)*sn + x0;
	float y2 = (x1 - x0)*sn + (y1 - y0)*cs + y0;
	return Vec2(x2, y2);
}

cocos2d::Vec2 CTileViewConverter::scale_to_view(float x, float y)
{
	x += fabs(_offset.x);
	y += fabs(_offset.y);
	float h = std::sqrt(std::pow(_tile_size.width,2) + std::pow(_tile_size.height,2));
	float w = h;
	float new_x = x * _view_size.width / w;
	float new_y = y * _view_size.height / h; 
	return Vec2(new_x, new_y);
}

cocos2d::Vec2 CTileViewConverter::scale_to_tile(float x, float y)
{
	float h = std::sqrt(std::pow(_tile_size.width, 2) + std::pow(_tile_size.height, 2));
	float w = h;
	float new_x = w * x / _view_size.width;
	float new_y = h*y / _view_size.height;
	new_x -= fabs(_offset.x);
	new_y -= fabs(_offset.y);
	return Vec2(new_x, new_y);
}

