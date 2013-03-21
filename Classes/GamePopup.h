//
//  GamePopup.h
//  PaperGame
//
//  Created by SBKarr on 1/14/13.
//
//

#ifndef __PaperGame__GamePopup__
#define __PaperGame__GamePopup__

#include "cocos2d.h"

USING_NS_CC;

class GamePopup : public CCNode, public CCLabelProtocol, public CCRGBAProtocol {
protected:
    enum Tags {
        TagLeft,
        TagRight,
        TagBody,
        TagLabel,
    };
    
    bool popupOpened, hasActiveAnimation, showButton;
    
    CCLabelBMFont *label;
    CCNode *button;
public:
    CREATE_FUNC(GamePopup);
    virtual bool init();
    
    virtual void onActionFinished();
    
    virtual void open(float duration);
    virtual void openWithCallback(float duration, CCObject *obj, SEL_CallFunc selector);
    virtual void openWithAction(float duration, CCFiniteTimeAction *a);
    
    virtual void close(float duration);
    virtual void closeWithCallback(float duration, CCObject *obj, SEL_CallFunc selector);
    virtual void closeWithAction(float duration, CCFiniteTimeAction *a);
    
    virtual void switchPopup(float duration);
    virtual void switchPopupWithCallback(float duration, CCObject *obj, SEL_CallFunc selector);
    virtual void switchPopupWithAction(float duration, CCFiniteTimeAction *a);
    
    virtual bool isOpened() {
        return popupOpened;
    }
    
    virtual void setString(const char *label);
    virtual const char* getString(void);
    
    virtual void setColor(const ccColor3B& color);
    virtual ccColor3B getColor(void);
    virtual GLubyte getOpacity(void);
    virtual void setOpacity(GLubyte opacity);
    virtual void setOpacityModifyRGB(bool bValue);
    virtual bool isOpacityModifyRGB(void);
    
    virtual void setButton(CCNode *button);
    
    virtual void setShowButton(bool show);
    
    virtual CCLabelBMFont *getLabel() const { return label; }
};

#endif /* defined(__PaperGame__GamePopup__) */
