#ifndef __TILE_TO_VIEW_H__
#define __TILE_TO_VIEW_H__

#include "cocos2d.h"
USING_NS_CC;

// tile(45度图)到view（指屏幕坐标)的转换
class CTileViewConverter {
public:
	CTileViewConverter(Size tile_size, Size view_size);
	~CTileViewConverter();

	Vec2 tile_to_view(Vec2 pos);
	Vec2 view_to_tile(Vec2 pos);
	Vec2 tile_to_view(float x, float y);
	Vec2 view_to_tile(float x, float y);
private:
	Vec2 coord_rotation(float x1, float y1, float x0, float y0, float angle);
	Vec2 scale_to_view(float x, float y);
	Vec2 scale_to_tile(float x, float y);


	Size _view_size;
	Size _tile_size;
	Vec2 _offset;
};

#endif