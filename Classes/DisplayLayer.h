//
//  DisplayLayer.h
//  PaperGame
//
//  Created by SBKarr on 12/17/12.
//
//

#ifndef __PaperGame__DisplayLayer__
#define __PaperGame__DisplayLayer__

#include "cocos2d.h"

using namespace cocos2d;

class DisplayLayerDelegate {
public:
    virtual void displayPause() {};
};

class DisplayLayer : public CCLayer {
protected:
    enum Tags {
        TagVelocity = 1,
        TagVelocityScale,
        TagAccelerationX,
        TagAccelerationY,
        
        TagButtonJump,
        TagButtonPause,
        TagButtonBreaks,
        
        TagDurability,
        TagShields,
        
        TagBottomBG,
        TagTimer,
        TagScore,
        
        TagDistance,
    };
    int selected;
    
    DisplayLayerDelegate *delegate;
    //CCNode *accelerationX, *accelerationY;
    CCLabelBMFont *velocity, *timer, *score;
public:
    bool init();
    CREATE_FUNC(DisplayLayer);
    
    void setDelegate(DisplayLayerDelegate *d) { delegate = d; }
    
    void setAccelerationX(float value);
    void setAccelerationY(float value);
    void setVelocity(float value);
    void setVelocityScale(float value);
    
    void setShields(float value);
    void setDurability(float value);
    
    void setDistance(float value);
    void setScore(int score);
    
    void setTimer(float timer);
    
    virtual void update(float fDeltha);
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

#endif /* defined(__PaperGame__DisplayLayer__) */
