/* 
 * File:   GameLayer.h
 * Author: sbkarr
 *
 * Created on 14 Декабрь 2012 г., 17:56
 */

#ifndef GAMELAYER_H
#define	GAMELAYER_H

#include "cocos2d.h"
#include "DisplayLayer.h"
#include "GamePopup.h"
#include "GameRoot.h"

using namespace cocos2d;

class MovingAverageFunction;

class GameLayer : public CCLayer, public DisplayLayerDelegate, public GameRootDelegate {
protected:
    CCAcceleration *currentAcceleration;
    MovingAverageFunction *accelerationX, *accelerationY;
    
	bool hasCapturedTouch;
	
    GamePopup *popup;
	GameRoot *root;
    DisplayLayer *display;
	CCSprite *background;
    
    void endGameWithStatus(bool win);
    
public:
    ~GameLayer();
	bool init();
	static CCScene * scene();
	CREATE_FUNC(GameLayer);
	
	void update(float fDeltha);
	
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	
    virtual void didAccelerate(CCAcceleration* pAccelerationValue);
    
    virtual void displayPause();
    
    virtual void popupCallback();
    
    virtual void onButtonExitTap(CCObject *obj);
    virtual void onDespawnLabel(CCNode *obj);
    virtual void onCollision(const CCPoint &point, enum FigureType figure, int value, int fid);
	
private:

};

#endif	/* GAMELAYER_H */

