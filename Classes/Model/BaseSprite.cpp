#include "BaseSprite.h"


bool BaseSprite::init()
{
    if ( !Sprite::init() ) {
        return false;
    }
	_camp = 0;
    return true;
}

void BaseSprite::setCamp(int camp)
{
	_camp = camp;
}

int BaseSprite::getCamp()
{
	return _camp;
}



