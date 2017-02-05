#ifndef __BuildProcess_H__
#define __BuildProcess_H__

#include "Classes/Utils/stdafx.h"
#include "src/c_sprite_action.h"

class BuildProcess : public cocos2d::Sprite
{
public:
	CC_CREATE_NO_PARAM(BuildProcess);
    virtual bool set(IBuildAction* pBuildAction);
    
    void showUI();
    void remove();
    void RunProcess(float dt);
    void finish();
    
private:
    float _delta;
    LoadingBar* _loadingBar;
    Text* _timeRest;
    
    IBuildAction* _pBuildAction;
};

#endif // __BuildProcess_H__
