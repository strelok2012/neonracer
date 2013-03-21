//
//  GameMenu.h
//  PaperGame
//
//  Created by SBKarr on 1/11/13.
//
//

#ifndef __PaperGame__GameMenu__
#define __PaperGame__GameMenu__

#include "cocos2d.h"

USING_NS_CC;

#define UD_KEY_CURRENT_MAX_SPEED "CurrentMaxSpeed"
#define UD_KEY_CURRENT_AVG_SPEED "CurrentAvgSpeed"
#define UD_KEY_CURRENT_SCORE "CurrentScore"
#define UD_KEY_CURRENT_TIME "CurrentTime"

#define UD_KEY_BEST_MAX_SPEED "BestMaxSpeed"
#define UD_KEY_BEST_AVG_SPEED "BestAvgSpeed"
#define UD_KEY_BEST_SCORE "BestScore"
#define UD_KEY_BEST_TIME "BestTime"

#define UD_KEY_ACHIVED_FULL_HP "AchivedFullHP"
#define UD_KEY_ACHIVED_NO_COLLISION "AchivedNoCollision"
#define UD_KEY_ACHIVED_NO_BREAKS "AchivedNoBreaks"
#define UD_KEY_ACHIVED_SUCCESS "AchivedSuccess"

#define UD_KEY_COUNT_FULL_HP "CountFullHP"
#define UD_KEY_COUNT_NO_COLLISION "CountNoCollision"
#define UD_KEY_COUNT_NO_BREAKS "CountNoBreaks"
#define UD_KEY_COUNT_SUCCESS "CountSuccess"

class GameMenu : public CCLayer {
protected:
    enum Tags {
        TagPopup,
        
        TagCurrentMaxSpeed,
        TagCurrentAvgSpeed,
        TagCurrentScore,
        TagCurrentTime,
        
        TagDiffMaxSpeed,
        TagDiffAvgSpeed,
        TagDiffScore,
        TagDiffTime,
        
        TagBestMaxSpeed,
        TagBestAvgSpeed,
        TagBestScore,
        TagBestTime,
    };
    
    bool hasCapturedTouch;
    
    void initStatList(const CCPoint &p, int tag);
public:
    static CCScene *scene();
    CREATE_FUNC(GameMenu);
    
    virtual bool init();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual void setLabelString(int tag, const char *format, ...);
    virtual void setLabelTimer(int tag, float timer);
    virtual void setLabelDiff(int tag, float value);
    virtual void setLabelDiffTimer(int tag, float timer);
    
    virtual void onButtonExitTap(CCObject *obj);
    virtual void onButtonPlayTap(CCObject *obj);
};

#endif /* defined(__PaperGame__GameMenu__) */
